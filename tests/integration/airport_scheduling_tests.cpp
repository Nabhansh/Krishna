#include "simulator/AirportOperations.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace project31 {
namespace {

AirportScenario scenarioFromCsv(
    const std::string& records,
    ScenarioConfiguration configuration = {2U, 2U, {}},
    bool hasEligibilityColumn = false
) {
    std::string csv =
        "passenger_id,name,arrival_minute,screening_duration_minutes,priority";
    if (hasEligibilityColumn) {
        csv += ",eligible_lanes";
    }
    csv += '\n';
    csv += records;
    std::istringstream input(csv);
    return AirportScenarioRunner::loadCsv(
        input,
        "Scheduling test",
        std::move(configuration)
    );
}

const PassengerServiceRecord& serviceFor(
    const ScenarioRunResult& result,
    const std::string& passengerId
) {
    const auto found = std::find_if(
        result.passengers.begin(),
        result.passengers.end(),
        [&passengerId](const PassengerServiceRecord& service) {
            return service.passenger.id == passengerId;
        }
    );
    if (found == result.passengers.end()) {
        throw std::logic_error("Expected passenger service record was not found.");
    }
    return *found;
}

} // namespace

TEST(AirportPrioritySchedulingTests, HighRiskPassengersPrecedeLowerRiskAtDispatch) {
    const AirportScenario scenario = scenarioFromCsv(
        "S,Standard,0,2,3\n"
        "E,Elevated,0,2,2\n"
        "H,High,0,2,1\n",
        {1U, 1U, {}}
    );
    const ScenarioRunResult& run =
        AirportScenarioRunner::compareStrategies(scenario).shortestWorkload;

    EXPECT_EQ(serviceFor(run, "H").serviceStartMinute, 0);
    EXPECT_EQ(serviceFor(run, "E").serviceStartMinute, 2);
    EXPECT_EQ(serviceFor(run, "S").serviceStartMinute, 4);
}

TEST(AirportPrioritySchedulingTests, EqualEffectivePriorityUsesStableFifoOrder) {
    const AirportScenario scenario = scenarioFromCsv(
        "H2,Second record,0,2,1\n"
        "H1,First record,0,2,1\n",
        {1U, 1U, {}}
    );
    const ScenarioRunResult& run =
        AirportScenarioRunner::compareStrategies(scenario).shortestWorkload;

    EXPECT_EQ(serviceFor(run, "H2").serviceStartMinute, 0);
    EXPECT_EQ(serviceFor(run, "H1").serviceStartMinute, 2);
}

TEST(AirportPrioritySchedulingTests, SimultaneousArrivalsAreBatchedBeforeDispatch) {
    const AirportScenario scenario = scenarioFromCsv(
        "S,Standard arrived first,0,2,3\n"
        "H,High arrived second,0,2,1\n",
        {1U, 1U, {}}
    );
    const ScenarioRunResult& run =
        AirportScenarioRunner::compareStrategies(scenario).shortestWorkload;

    EXPECT_EQ(serviceFor(run, "H").serviceStartMinute, 0);
    EXPECT_EQ(serviceFor(run, "S").serviceStartMinute, 2);
}

TEST(AirportPrioritySchedulingTests, AgingRaisesWaitingPassengerAtConfiguredIntervals) {
    ScenarioConfiguration configuration{1U, 1U, {}};
    configuration.priorityAgingIntervalMinutes = 2;
    const AirportScenario scenario = scenarioFromCsv(
        "BLOCK,Initial service,0,5,1\n"
        "WAIT,Waiting standard,0,1,3\n"
        "NEW,New high risk,5,1,1\n",
        configuration
    );
    const ScenarioRunResult& run =
        AirportScenarioRunner::compareStrategies(scenario).shortestWorkload;

    EXPECT_EQ(serviceFor(run, "WAIT").serviceStartMinute, 5);
    EXPECT_EQ(serviceFor(run, "NEW").serviceStartMinute, 6);
}

TEST(AirportPrioritySchedulingTests, AgingPreventsStarvationDuringHighRiskArrivals) {
    ScenarioConfiguration configuration{1U, 1U, {}};
    configuration.priorityAgingIntervalMinutes = 2;
    const AirportScenario scenario = scenarioFromCsv(
        "BLOCK,Initial high risk,0,5,1\n"
        "WAIT,Waiting standard,0,1,3\n"
        "H1,High arrival one,5,1,1\n"
        "H2,High arrival two,6,1,1\n"
        "H3,High arrival three,7,1,1\n",
        configuration
    );
    const ScenarioRunResult& run =
        AirportScenarioRunner::compareStrategies(scenario).shortestWorkload;

    EXPECT_EQ(serviceFor(run, "WAIT").serviceStartMinute, 5);
    EXPECT_LT(
        serviceFor(run, "WAIT").serviceStartMinute,
        serviceFor(run, "H3").serviceStartMinute
    );
}

TEST(AirportPrioritySchedulingTests, EligibilityAndLaneOutagesConstrainDispatch) {
    const AirportScenario scenario = scenarioFromCsv(
        "OPEN,Open lane passenger,0,2,3,\n"
        "RESTRICTED,Restricted high passenger,0,2,1,Lane-2\n",
        {2U, 2U, {{1U, 0, 5}}},
        true
    );
    const ScenarioRunResult& run =
        AirportScenarioRunner::compareStrategies(scenario).shortestWorkload;

    EXPECT_EQ(serviceFor(run, "OPEN").laneId, "Lane-1");
    EXPECT_EQ(serviceFor(run, "OPEN").serviceStartMinute, 0);
    EXPECT_EQ(serviceFor(run, "RESTRICTED").laneId, "Lane-2");
    EXPECT_EQ(serviceFor(run, "RESTRICTED").serviceStartMinute, 5);
}

TEST(AirportPrioritySchedulingTests, OfficerUnavailabilityBlocksPairedLane) {
    ScenarioConfiguration configuration{1U, 1U, {}};
    configuration.officerOutages.push_back({0U, 0, 4});
    const AirportScenario scenario = scenarioFromCsv(
        "P,Passenger,0,2,1\n",
        configuration
    );
    const ScenarioRunResult& run =
        AirportScenarioRunner::compareStrategies(scenario).shortestWorkload;

    EXPECT_EQ(serviceFor(run, "P").serviceStartMinute, 4);
    ASSERT_EQ(run.lanes.size(), 1U);
    EXPECT_EQ(run.lanes[0].availableMinutes, 2);
    EXPECT_DOUBLE_EQ(run.lanes[0].utilizationPercent, 100.0);
}

TEST(AirportPrioritySchedulingTests, ServiceMayEndAtOutageBoundaryButMayNotOverlapIt) {
    const AirportScenario scenario = scenarioFromCsv(
        "FIRST,Ends at boundary,0,3,1\n"
        "SECOND,Must wait until outage ends,0,2,1\n",
        {1U, 1U, {{0U, 3, 8}}}
    );
    const ScenarioRunResult& run =
        AirportScenarioRunner::compareStrategies(scenario).shortestWorkload;

    EXPECT_EQ(serviceFor(run, "FIRST").completionMinute, 3);
    EXPECT_EQ(serviceFor(run, "SECOND").serviceStartMinute, 8);
}

TEST(AirportPrioritySchedulingTests, EveryPassengerCompletesOnceWithoutLaneOverlap) {
    const AirportScenario scenario = scenarioFromCsv(
        "P1,One,0,4,2\n"
        "P2,Two,0,3,1\n"
        "P3,Three,1,2,3\n"
        "P4,Four,2,5,2\n"
        "P5,Five,3,1,1\n",
        {2U, 2U, {}}
    );
    const ScenarioRunResult& run =
        AirportScenarioRunner::compareStrategies(scenario).shortestWorkload;
    ASSERT_EQ(run.completedPassengerCount, scenario.passengers.size());
    ASSERT_EQ(run.passengers.size(), scenario.passengers.size());

    std::set<std::string> screenedIds;
    std::vector<std::vector<std::pair<std::int64_t, std::int64_t>>> intervals(
        run.lanes.size()
    );
    for (const PassengerServiceRecord& service : run.passengers) {
        EXPECT_TRUE(screenedIds.insert(service.passenger.id).second);
        EXPECT_GE(service.serviceStartMinute, service.passenger.arrivalMinute);
        EXPECT_EQ(
            service.completionMinute - service.serviceStartMinute,
            service.passenger.screeningDurationMinutes
        );
        const std::size_t laneIndex =
            static_cast<std::size_t>(service.laneId.back() - '1');
        intervals[laneIndex].emplace_back(
            service.serviceStartMinute,
            service.completionMinute
        );
    }
    EXPECT_EQ(screenedIds.size(), scenario.passengers.size());
    for (auto& laneIntervals : intervals) {
        std::sort(laneIntervals.begin(), laneIntervals.end());
        for (std::size_t index = 1U; index < laneIntervals.size(); ++index) {
            EXPECT_GE(laneIntervals[index].first, laneIntervals[index - 1U].second);
        }
    }
}

TEST(AirportStaffingEvaluationTests, SelectsMinimumConfigurationWhenTargetsAreMet) {
    const AirportScenario scenario = scenarioFromCsv(
        "P,Passenger,0,4,1\n",
        {1U, 1U, {}}
    );
    StaffingEvaluationOptions options;
    options.minimumLaneCount = 1U;
    options.maximumLaneCount = 3U;
    options.maximumMeanWaitMinutes = 0.0;
    options.maximumPassengerWaitMinutes = 0;
    options.minimumThroughputPerHour = 15.0;

    const StaffingEvaluation evaluation =
        AirportOperations::recommendAirportStaffing(scenario, options);
    ASSERT_TRUE(evaluation.recommendedCandidateIndex.has_value());
    const StaffingCandidateResult& recommendation =
        evaluation.candidates[*evaluation.recommendedCandidateIndex];
    EXPECT_EQ(recommendation.laneCount, 1U);
    EXPECT_EQ(recommendation.officerCount, 1U);
    EXPECT_TRUE(recommendation.meetsTargets);
}

TEST(AirportStaffingEvaluationTests, AdditionalLaneCanMeetWaitTarget) {
    const AirportScenario scenario = scenarioFromCsv(
        "P1,One,0,4,1\n"
        "P2,Two,0,4,1\n",
        {1U, 1U, {}}
    );
    StaffingEvaluationOptions options;
    options.minimumLaneCount = 1U;
    options.maximumLaneCount = 2U;
    options.maximumMeanWaitMinutes = 0.0;
    options.maximumPassengerWaitMinutes = 0;

    const StaffingEvaluation evaluation =
        AirportScenarioRunner::evaluateStaffing(scenario, options);
    ASSERT_TRUE(evaluation.recommendedCandidateIndex.has_value());
    EXPECT_EQ(
        evaluation.candidates[*evaluation.recommendedCandidateIndex].laneCount,
        2U
    );
    EXPECT_GT(evaluation.candidates.front().run.meanWaitMinutes, 0.0);
}

TEST(AirportStaffingEvaluationTests, ExcludesConfigurationsOverResourceBudget) {
    const AirportScenario scenario = scenarioFromCsv(
        "P,Passenger,0,1,1\n",
        {1U, 1U, {}}
    );
    StaffingEvaluationOptions options;
    options.minimumLaneCount = 1U;
    options.maximumLaneCount = 3U;
    options.maximumCost = 3U;

    const StaffingEvaluation evaluation =
        AirportScenarioRunner::evaluateStaffing(scenario, options);
    ASSERT_EQ(evaluation.candidates.size(), 1U);
    EXPECT_EQ(evaluation.candidates.front().resourceCost, 2U);
    ASSERT_TRUE(evaluation.recommendedCandidateIndex.has_value());
    EXPECT_EQ(evaluation.candidates[*evaluation.recommendedCandidateIndex].laneCount, 1U);
}

TEST(AirportStaffingEvaluationTests, RejectsStaffingBoundsAboveResourceCapacity) {
    const AirportScenario scenario = scenarioFromCsv(
        "P,Passenger,0,1,1\n",
        {1U, 1U, {}}
    );
    StaffingEvaluationOptions options;
    options.maximumLaneCount = 129U;

    EXPECT_THROW(
        AirportScenarioRunner::evaluateStaffing(scenario, options),
        std::invalid_argument
    );
}

TEST(AirportStaffingEvaluationTests, ReportsNoRecommendationWhenNoCandidateMeetsTargets) {
    const AirportScenario scenario = scenarioFromCsv(
        "P,Passenger,0,1,1\n",
        {1U, 1U, {}}
    );
    StaffingEvaluationOptions options;
    options.minimumLaneCount = 1U;
    options.maximumLaneCount = 2U;
    options.maximumMeanWaitMinutes = 0.0;
    options.minimumThroughputPerHour = 61.0;

    const StaffingEvaluation evaluation =
        AirportScenarioRunner::evaluateStaffing(scenario, options);
    ASSERT_EQ(evaluation.candidates.size(), 2U);
    EXPECT_FALSE(evaluation.recommendedCandidateIndex.has_value());
    EXPECT_FALSE(evaluation.candidates[0].meetsTargets);
    EXPECT_FALSE(evaluation.candidates[1].meetsTargets);
}

TEST(AirportStaffingEvaluationTests, RecommendationsAreDeterministic) {
    const AirportScenario scenario = scenarioFromCsv(
        "P1,One,0,4,1\n"
        "P2,Two,0,3,2\n"
        "P3,Three,2,5,3\n",
        {1U, 1U, {}}
    );
    StaffingEvaluationOptions options;
    options.minimumLaneCount = 1U;
    options.maximumLaneCount = 4U;
    options.maximumMeanWaitMinutes = 1.0;
    const StaffingEvaluation first =
        AirportScenarioRunner::evaluateStaffing(scenario, options);
    const StaffingEvaluation second =
        AirportScenarioRunner::evaluateStaffing(scenario, options);

    EXPECT_EQ(first.recommendedCandidateIndex, second.recommendedCandidateIndex);
    ASSERT_EQ(first.candidates.size(), second.candidates.size());
    for (std::size_t index = 0; index < first.candidates.size(); ++index) {
        EXPECT_EQ(first.candidates[index].laneCount, second.candidates[index].laneCount);
        EXPECT_EQ(first.candidates[index].resourceCost, second.candidates[index].resourceCost);
        EXPECT_EQ(first.candidates[index].run.totalWaitMinutes,
                  second.candidates[index].run.totalWaitMinutes);
        EXPECT_EQ(first.candidates[index].meetsTargets,
                  second.candidates[index].meetsTargets);
    }
}

} // namespace project31
