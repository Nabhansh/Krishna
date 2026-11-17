#include "simulator/AirportScenario.hpp"
#include "simulator/AirportOperations.hpp"
#include "simulator/CommandLineInterface.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace project31 {
namespace {

constexpr const char* kCsvHeader =
    "passenger_id,name,arrival_minute,screening_duration_minutes,priority\n";

AirportScenario loadText(
    const std::string& records,
    ScenarioConfiguration configuration = {2U, 2U, {}},
    const std::string& name = "Test scenario"
) {
    std::istringstream input(std::string(kCsvHeader) + records);
    return AirportScenarioRunner::loadCsv(input, name, std::move(configuration));
}

std::filesystem::path temporaryBasePath() {
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    return std::filesystem::temp_directory_path() /
        ("project31-scenario-" + std::to_string(ticks));
}

std::int64_t bruteForceMinimumWait(const AirportScenario& scenario, std::size_t laneCount) {
    std::int64_t best = std::numeric_limits<std::int64_t>::max();
    const std::size_t assignmentCount = 1U << scenario.passengers.size();
    for (std::size_t assignment = 0; assignment < assignmentCount; ++assignment) {
        std::vector<std::int64_t> nextFree(laneCount, 0);
        std::int64_t totalWait = 0;
        for (std::size_t index = 0; index < scenario.passengers.size(); ++index) {
            const ScenarioPassenger& passenger = scenario.passengers[index];
            const std::size_t lane = (assignment >> index) & 1U;
            const std::int64_t start = std::max<std::int64_t>(
                passenger.arrivalMinute,
                nextFree[lane]
            );
            totalWait += start - passenger.arrivalMinute;
            nextFree[lane] = start + passenger.screeningDurationMinutes;
        }
        best = std::min(best, totalWait);
    }
    return best;
}

struct ReferencePassengerService {
    std::string id;
    std::size_t laneIndex;
    std::int64_t startMinute;
    std::int64_t completionMinute;
    std::int64_t waitMinutes;
};

struct ReferenceLaneMetrics {
    std::size_t passengerCount{};
    std::int64_t busyMinutes{};
    std::int64_t availableMinutes{};
    double utilizationPercent{};
};

struct ReferenceRun {
    std::vector<ReferencePassengerService> passengers;
    std::vector<ReferenceLaneMetrics> lanes;
    std::int64_t totalWaitMinutes{};
    std::int64_t maximumWaitMinutes{};
    std::int64_t makespanMinutes{};
    double meanWaitMinutes{};
    double throughputPerHour{};
};

bool referenceServiceFits(
    const AirportScenario& scenario,
    std::size_t laneIndex,
    std::int64_t start,
    int duration
) {
    const std::int64_t finish = start + duration;
    for (const LaneOutage& outage : scenario.configuration.outages) {
        if (outage.laneIndex == laneIndex &&
            start < outage.endMinute && outage.startMinute < finish) {
            return false;
        }
    }
    for (const OfficerOutage& outage : scenario.configuration.officerOutages) {
        if (outage.officerIndex == laneIndex &&
            start < outage.endMinute && outage.startMinute < finish) {
            return false;
        }
    }
    return true;
}

bool referenceResourceAvailable(
    const AirportScenario& scenario,
    std::size_t laneIndex,
    std::int64_t minute
) {
    for (const LaneOutage& outage : scenario.configuration.outages) {
        if (outage.laneIndex == laneIndex &&
            outage.startMinute <= minute && minute < outage.endMinute) {
            return false;
        }
    }
    for (const OfficerOutage& outage : scenario.configuration.officerOutages) {
        if (outage.officerIndex == laneIndex &&
            outage.startMinute <= minute && minute < outage.endMinute) {
            return false;
        }
    }
    return true;
}

int referencePriorityRank(
    const ScenarioPassenger& passenger,
    std::int64_t now,
    std::int64_t agingInterval
) {
    const int baseRank = passenger.priority - 1;
    const std::int64_t agePromotions =
        (now - passenger.arrivalMinute) / agingInterval;
    return agePromotions >= baseRank
        ? 0
        : baseRank - static_cast<int>(agePromotions);
}

bool referenceLaneEligible(
    const ScenarioPassenger& passenger,
    std::size_t laneIndex
) {
    return passenger.eligibleLaneIndices.empty() ||
        std::find(
            passenger.eligibleLaneIndices.begin(),
            passenger.eligibleLaneIndices.end(),
            laneIndex
        ) != passenger.eligibleLaneIndices.end();
}

ReferenceRun simulateReferenceByMinute(
    const AirportScenario& input,
    AssignmentStrategy strategy
) {
    AirportScenario scenario = input;
    std::stable_sort(
        scenario.passengers.begin(),
        scenario.passengers.end(),
        [](const ScenarioPassenger& left, const ScenarioPassenger& right) {
            return left.arrivalMinute < right.arrivalMinute;
        }
    );
    const std::size_t passengerCount = scenario.passengers.size();
    const std::size_t laneCount = scenario.configuration.laneCount;
    ReferenceRun result;
    result.lanes.resize(laneCount);
    result.passengers.resize(passengerCount);
    if (passengerCount == 0U) {
        return result;
    }

    std::int64_t firstArrival = scenario.passengers.front().arrivalMinute;
    std::int64_t lastRelevantTime = firstArrival;
    std::int64_t totalScreeningMinutes = 0;
    for (const ScenarioPassenger& passenger : scenario.passengers) {
        lastRelevantTime = std::max<std::int64_t>(
            lastRelevantTime,
            passenger.arrivalMinute
        );
        totalScreeningMinutes += passenger.screeningDurationMinutes;
    }
    for (const LaneOutage& outage : scenario.configuration.outages) {
        lastRelevantTime = std::max<std::int64_t>(lastRelevantTime, outage.endMinute);
    }
    for (const OfficerOutage& outage : scenario.configuration.officerOutages) {
        lastRelevantTime = std::max<std::int64_t>(lastRelevantTime, outage.endMinute);
    }
    const std::int64_t timeLimit = lastRelevantTime + totalScreeningMinutes + 1;

    std::vector<bool> arrived(passengerCount, false);
    std::vector<bool> waiting(passengerCount, false);
    std::vector<std::optional<std::size_t>> activePassenger(laneCount);
    std::vector<std::int64_t> finishTime(laneCount, 0);
    std::vector<std::int64_t> cumulativeWork(laneCount, 0);
    std::size_t roundRobinLane = 0U;
    std::size_t completedCount = 0U;
    std::int64_t now = firstArrival;

    while (completedCount < passengerCount && now <= timeLimit) {
        for (std::size_t lane = 0; lane < laneCount; ++lane) {
            if (activePassenger[lane] && finishTime[lane] == now) {
                activePassenger[lane].reset();
                ++completedCount;
            }
        }
        for (std::size_t index = 0; index < passengerCount; ++index) {
            if (!arrived[index] && scenario.passengers[index].arrivalMinute == now) {
                arrived[index] = true;
                waiting[index] = true;
            }
        }

        while (true) {
            std::optional<std::size_t> selectedPassenger;
            std::tuple<int, std::int64_t, std::size_t> selectedKey{
                std::numeric_limits<int>::max(),
                std::numeric_limits<std::int64_t>::max(),
                std::numeric_limits<std::size_t>::max()
            };
            for (std::size_t lane = 0; lane < laneCount; ++lane) {
                if (activePassenger[lane] ||
                    !referenceResourceAvailable(scenario, lane, now)) {
                    continue;
                }
                std::optional<std::size_t> laneHead;
                std::tuple<int, std::int64_t, std::size_t> laneHeadKey{
                    std::numeric_limits<int>::max(),
                    std::numeric_limits<std::int64_t>::max(),
                    std::numeric_limits<std::size_t>::max()
                };
                for (std::size_t index = 0; index < passengerCount; ++index) {
                    const ScenarioPassenger& passenger = scenario.passengers[index];
                    if (!waiting[index] || !referenceLaneEligible(passenger, lane) ||
                        !referenceServiceFits(
                            scenario,
                            lane,
                            now,
                            passenger.screeningDurationMinutes
                        )) {
                        continue;
                    }
                    const auto key = std::tuple(
                        referencePriorityRank(
                            passenger,
                            now,
                            scenario.configuration.priorityAgingIntervalMinutes
                        ),
                        static_cast<std::int64_t>(passenger.arrivalMinute),
                        index
                    );
                    if (!laneHead || key < laneHeadKey) {
                        laneHead = index;
                        laneHeadKey = key;
                    }
                }
                if (laneHead && (!selectedPassenger || laneHeadKey < selectedKey)) {
                    selectedPassenger = laneHead;
                    selectedKey = laneHeadKey;
                }
            }
            if (!selectedPassenger) {
                break;
            }

            const ScenarioPassenger& passenger =
                scenario.passengers[*selectedPassenger];
            std::vector<std::size_t> candidateLanes;
            for (std::size_t lane = 0; lane < laneCount; ++lane) {
                if (!activePassenger[lane] &&
                    referenceResourceAvailable(scenario, lane, now) &&
                    referenceLaneEligible(passenger, lane) &&
                    referenceServiceFits(
                        scenario,
                        lane,
                        now,
                        passenger.screeningDurationMinutes
                    )) {
                    candidateLanes.push_back(lane);
                }
            }
            if (candidateLanes.empty()) {
                throw std::logic_error("Reference scheduler found no eligible lane.");
            }

            std::size_t selectedLane = candidateLanes.front();
            if (strategy == AssignmentStrategy::RoundRobin) {
                for (std::size_t offset = 0; offset < laneCount; ++offset) {
                    const std::size_t candidate =
                        (roundRobinLane + offset) % laneCount;
                    if (std::find(
                            candidateLanes.begin(),
                            candidateLanes.end(),
                            candidate
                        ) != candidateLanes.end()) {
                        selectedLane = candidate;
                        break;
                    }
                }
                roundRobinLane = (selectedLane + 1U) % laneCount;
            } else {
                for (const std::size_t lane : candidateLanes) {
                    if (cumulativeWork[lane] < cumulativeWork[selectedLane]) {
                        selectedLane = lane;
                    }
                }
            }

            waiting[*selectedPassenger] = false;
            const std::int64_t finish =
                now + passenger.screeningDurationMinutes;
            finishTime[selectedLane] = finish;
            activePassenger[selectedLane] = *selectedPassenger;
            cumulativeWork[selectedLane] += passenger.screeningDurationMinutes;
            result.passengers[*selectedPassenger] = {
                passenger.id,
                selectedLane,
                now,
                finish,
                now - passenger.arrivalMinute
            };
            ReferenceLaneMetrics& laneMetrics = result.lanes[selectedLane];
            ++laneMetrics.passengerCount;
            laneMetrics.busyMinutes += passenger.screeningDurationMinutes;
            result.totalWaitMinutes += now - passenger.arrivalMinute;
            result.maximumWaitMinutes = std::max(
                result.maximumWaitMinutes,
                now - passenger.arrivalMinute
            );
        }
        ++now;
    }
    if (completedCount != passengerCount) {
        throw std::logic_error("Reference scheduler failed to complete all passengers.");
    }

    const std::int64_t lastCompletion = std::max_element(
        result.passengers.begin(),
        result.passengers.end(),
        [](const auto& left, const auto& right) {
            return left.completionMinute < right.completionMinute;
        }
    )->completionMinute;
    result.makespanMinutes = lastCompletion - firstArrival;
    result.meanWaitMinutes =
        static_cast<double>(result.totalWaitMinutes) /
        static_cast<double>(passengerCount);
    result.throughputPerHour =
        static_cast<double>(passengerCount) * 60.0 /
        static_cast<double>(result.makespanMinutes);
    for (std::size_t lane = 0; lane < laneCount; ++lane) {
        for (std::int64_t minute = firstArrival; minute < lastCompletion; ++minute) {
            if (referenceResourceAvailable(scenario, lane, minute)) {
                ++result.lanes[lane].availableMinutes;
            }
        }
        if (result.lanes[lane].availableMinutes > 0) {
            result.lanes[lane].utilizationPercent =
                100.0 * static_cast<double>(result.lanes[lane].busyMinutes) /
                static_cast<double>(result.lanes[lane].availableMinutes);
        }
    }
    return result;
}

void expectMatchesReference(
    const AirportScenario& scenario,
    AssignmentStrategy strategy
) {
    const ReferenceRun expected = simulateReferenceByMinute(scenario, strategy);
    const ScenarioRunResult actual =
        AirportScenarioRunner::runStrategy(scenario, strategy);
    ASSERT_EQ(actual.passengers.size(), expected.passengers.size());
    ASSERT_EQ(actual.lanes.size(), expected.lanes.size());
    EXPECT_EQ(actual.totalWaitMinutes, expected.totalWaitMinutes);
    EXPECT_EQ(actual.maximumWaitMinutes, expected.maximumWaitMinutes);
    EXPECT_EQ(actual.makespanMinutes, expected.makespanMinutes);
    EXPECT_NEAR(actual.meanWaitMinutes, expected.meanWaitMinutes, 1e-9);
    EXPECT_NEAR(actual.throughputPerHour, expected.throughputPerHour, 1e-9);
    EXPECT_EQ(actual.completedPassengerCount, scenario.passengers.size());
    for (std::size_t index = 0; index < expected.passengers.size(); ++index) {
        const PassengerServiceRecord& service = actual.passengers[index];
        const ReferencePassengerService& reference = expected.passengers[index];
        EXPECT_EQ(service.passenger.id, reference.id);
        EXPECT_EQ(service.laneId, "Lane-" + std::to_string(reference.laneIndex + 1U));
        EXPECT_EQ(service.serviceStartMinute, reference.startMinute);
        EXPECT_EQ(service.completionMinute, reference.completionMinute);
        EXPECT_EQ(service.waitMinutes, reference.waitMinutes);
        EXPECT_GE(service.serviceStartMinute, service.passenger.arrivalMinute);
    }
    for (std::size_t lane = 0; lane < expected.lanes.size(); ++lane) {
        EXPECT_EQ(actual.lanes[lane].passengerCount,
                  expected.lanes[lane].passengerCount);
        EXPECT_EQ(actual.lanes[lane].busyMinutes,
                  expected.lanes[lane].busyMinutes);
        EXPECT_EQ(actual.lanes[lane].availableMinutes,
                  expected.lanes[lane].availableMinutes);
        EXPECT_NEAR(actual.lanes[lane].utilizationPercent,
                    expected.lanes[lane].utilizationPercent,
                    1e-9);
    }
}

std::vector<std::string> splitSimpleCsvRow(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool insideQuotes = false;
    for (std::size_t index = 0; index < line.size(); ++index) {
        const char character = line[index];
        if (character == '"') {
            if (insideQuotes && index + 1U < line.size() &&
                line[index + 1U] == '"') {
                field.push_back('"');
                ++index;
            } else {
                insideQuotes = !insideQuotes;
            }
        } else if (character == ',' && !insideQuotes) {
            fields.push_back(std::move(field));
            field.clear();
        } else {
            field.push_back(character);
        }
    }
    fields.push_back(std::move(field));
    return fields;
}

} // namespace

TEST(AirportScenarioCsvTests, LoadsQuotedPassengerFieldsAndMapsRiskCategories) {
    const AirportScenario scenario = loadText(
        "P1,\"Ada \"\"Ace\"\", Lovelace\",0,4,1\n"
        "P2,Grace Hopper,2,5,2\n"
        "P3,Katherine Johnson,3,3,3\n"
    );

    ASSERT_EQ(scenario.passengers.size(), 3U);
    EXPECT_EQ(scenario.passengers[0].name, "Ada \"Ace\", Lovelace");
    EXPECT_EQ(
        riskCategoryForPriority(scenario.passengers[0].priority),
        PassengerRiskCategory::High
    );
    EXPECT_EQ(
        riskCategoryForPriority(scenario.passengers[1].priority),
        PassengerRiskCategory::Elevated
    );
    EXPECT_EQ(
        riskCategoryForPriority(scenario.passengers[2].priority),
        PassengerRiskCategory::Standard
    );
}

TEST(AirportScenarioCsvTests, OrdersUnsortedArrivalsStably) {
    const AirportScenario scenario = loadText(
        "P2,Later,3,2,2\n"
        "P1,First,1,2,1\n"
        "P3,Same time,1,4,3\n"
    );
    ASSERT_EQ(scenario.passengers.size(), 3U);
    EXPECT_EQ(scenario.passengers[0].id, "P1");
    EXPECT_EQ(scenario.passengers[1].id, "P3");
    EXPECT_EQ(scenario.passengers[2].id, "P2");
}

TEST(AirportScenarioCsvTests, RejectsInvalidRowsAndUnsupportedHeaders) {
    std::istringstream wrongHeader("wrong,header\nP1,Name,0,3,1\n");
    EXPECT_THROW(
        AirportScenarioRunner::loadCsv(
            wrongHeader,
            "Bad header",
            {2U, 2U, {}}
        ),
        std::invalid_argument
    );
    EXPECT_THROW(loadText("P1,Name,0,0,1\n"), std::invalid_argument);
    EXPECT_THROW(loadText("P1,Name,0,3,4\n"), std::invalid_argument);
    EXPECT_THROW(loadText("P1,Name,0,3,1\nP1,Duplicate,1,2,2\n"), std::invalid_argument);
    EXPECT_THROW(loadText("P1,\"Unclosed,0,3,1\n"), std::invalid_argument);
}

TEST(AirportScenarioCsvTests, RejectsEmptyMissingAndReorderedCsvColumns) {
    std::istringstream emptyFile;
    EXPECT_THROW(
        AirportScenarioRunner::loadCsv(emptyFile, "Empty file", {1U, 1U, {}}),
        std::invalid_argument
    );
    std::istringstream missingHeaderColumn(
        "passenger_id,name,arrival_minute,screening_duration_minutes\n"
    );
    EXPECT_THROW(
        AirportScenarioRunner::loadCsv(
            missingHeaderColumn,
            "Missing column",
            {1U, 1U, {}}
        ),
        std::invalid_argument
    );
    std::istringstream reorderedHeader(
        "name,passenger_id,arrival_minute,screening_duration_minutes,priority\n"
        "Ada,P1,0,1,1\n"
    );
    EXPECT_THROW(
        AirportScenarioRunner::loadCsv(
            reorderedHeader,
            "Reordered columns",
            {1U, 1U, {}}
        ),
        std::invalid_argument
    );
    EXPECT_THROW(loadText("P1,Ada,0,1\n"), std::invalid_argument);
}

TEST(AirportScenarioCsvTests, SupportsHeaderOnlyEmptyPassengerScenario) {
    const AirportScenario scenario = loadText("");
    ASSERT_TRUE(scenario.passengers.empty());
    const ScenarioRunResult run = AirportScenarioRunner::runStrategy(
        scenario,
        AssignmentStrategy::ShortestWorkload
    );
    EXPECT_EQ(run.passengerCount, 0U);
    EXPECT_EQ(run.completedPassengerCount, 0U);
    EXPECT_DOUBLE_EQ(run.meanWaitMinutes, 0.0);
    EXPECT_DOUBLE_EQ(run.throughputPerHour, 0.0);
    EXPECT_EQ(run.makespanMinutes, 0);
}

TEST(AirportScenarioCsvTests, RejectsInvalidTimesDurationsPrioritiesAndNumericOverflow) {
    EXPECT_THROW(loadText("P1,Ada,not-a-time,1,1\n"), std::invalid_argument);
    EXPECT_THROW(loadText("P1,Ada,-1,1,1\n"), std::invalid_argument);
    EXPECT_THROW(loadText("P1,Ada,0,-1,1\n"), std::invalid_argument);
    EXPECT_THROW(loadText("P1,Ada,0,1,0\n"), std::invalid_argument);
    EXPECT_THROW(loadText("P1,Ada,0,1,4\n"), std::invalid_argument);
    EXPECT_THROW(
        loadText("P1,Ada,2147483648,1,1\n"),
        std::invalid_argument
    );

    const int maximumInt = std::numeric_limits<int>::max();
    const AirportScenario largeValueScenario = loadText(
        "P1,Ada," + std::to_string(maximumInt) + "," +
            std::to_string(maximumInt) + ",1\n",
        {1U, 1U, {}}
    );
    const ScenarioRunResult largeValueRun = AirportScenarioRunner::runStrategy(
        largeValueScenario,
        AssignmentStrategy::ShortestWorkload
    );
    EXPECT_EQ(
        largeValueRun.passengers.front().completionMinute,
        static_cast<std::int64_t>(maximumInt) * 2
    );

    ScenarioConfiguration overflowingAging{1U, 1U, {}};
    overflowingAging.priorityAgingIntervalMinutes =
        std::numeric_limits<std::int64_t>::max();
    const AirportScenario overflowScenario = loadText(
        "P1,Ada,1,1,3\n",
        overflowingAging
    );
    EXPECT_THROW(
        AirportScenarioRunner::runStrategy(
            overflowScenario,
            AssignmentStrategy::ShortestWorkload
        ),
        std::overflow_error
    );
}

TEST(AirportScenarioSimulationTests, ComparesStrategiesAndReportsOperationalMetrics) {
    const AirportScenario scenario = loadText(
        "P001,Ada Lovelace,0,4,2\n"
        "P002,Grace Hopper,2,5,1\n"
        "P003,Katherine Johnson,3,3,2\n"
        "P004,Alan Turing,5,4,3\n"
        "P005,Annie Easley,8,6,1\n",
        {2U, 2U, {}},
        "Normal operation"
    );

    const ScenarioComparison comparison =
        AirportOperations::compareAirportStrategies(scenario);
    const ScenarioRunResult& shortest = comparison.shortestWorkload;
    const ScenarioRunResult& roundRobin = comparison.roundRobin;
    ASSERT_EQ(shortest.passengerCount, 5U);
    EXPECT_EQ(shortest.completedPassengerCount, 5U);
    EXPECT_EQ(shortest.highRiskPassengerCount, 2U);
    EXPECT_EQ(shortest.elevatedRiskPassengerCount, 2U);
    EXPECT_EQ(shortest.standardRiskPassengerCount, 1U);
    EXPECT_DOUBLE_EQ(shortest.meanWaitMinutes, 0.6);
    EXPECT_EQ(shortest.maximumWaitMinutes, 2);
    EXPECT_EQ(shortest.makespanMinutes, 14);
    EXPECT_NEAR(shortest.throughputPerHour, 300.0 / 14.0, 1e-8);
    ASSERT_EQ(shortest.lanes.size(), 2U);
    EXPECT_NEAR(shortest.lanes[0].utilizationPercent, 100.0 * 13.0 / 14.0, 1e-8);
    EXPECT_NEAR(shortest.lanes[1].utilizationPercent, 100.0 * 9.0 / 14.0, 1e-8);
    EXPECT_LE(shortest.meanWaitMinutes, roundRobin.meanWaitMinutes);
    EXPECT_EQ(shortest.staffedOfficerCount, shortest.staffedLaneCount);
    EXPECT_EQ(shortest.passengers[0].laneId, "Lane-1");
    EXPECT_EQ(shortest.passengers[0].officerId, "Officer-1");
    EXPECT_EQ(comparison.projectedLaneCount, 3U);
    EXPECT_LT(
        comparison.additionalLaneWhatIf.meanWaitMinutes,
        comparison.baselineMeanWaitMinutes
    );
    EXPECT_FALSE(comparison.resourceRecommendation.empty());
}

TEST(AirportScenarioOracleTests, MatchesBruteForceMinimumWaitForSmallInstance) {
    const AirportScenario scenario = loadText(
        "P1,One,0,6,1\n"
        "P2,Two,0,2,2\n"
        "P3,Three,0,2,3\n",
        {2U, 2U, {}}
    );
    const ScenarioComparison comparison =
        AirportScenarioRunner::compareStrategies(scenario);

    EXPECT_EQ(bruteForceMinimumWait(scenario, 2U), 2);
    EXPECT_EQ(
        comparison.shortestWorkload.totalWaitMinutes,
        bruteForceMinimumWait(scenario, 2U)
    );
}

TEST(AirportScenarioOracleTests, MatchesIndependentMinuteStepReferenceOnGeneratedCases) {
    const std::array<AssignmentStrategy, 2U> strategies{
        AssignmentStrategy::ShortestWorkload,
        AssignmentStrategy::RoundRobin
    };
    for (std::size_t caseIndex = 0; caseIndex < 32U; ++caseIndex) {
        const std::size_t laneCount = 1U + caseIndex % 3U;
        ScenarioConfiguration configuration{
            laneCount,
            laneCount,
            {},
            static_cast<std::int64_t>(1U + caseIndex % 4U),
            {}
        };
        if (caseIndex % 4U == 0U) {
            configuration.outages.push_back({0U, 2, 4});
        }
        if (caseIndex % 5U == 0U) {
            configuration.officerOutages.push_back({laneCount - 1U, 4, 6});
        }
        AirportScenario scenario{
            "Generated oracle case " + std::to_string(caseIndex),
            "deterministic test data",
            configuration,
            {},
            true
        };
        const std::size_t passengerCount = 1U + caseIndex % 5U;
        for (std::size_t index = 0; index < passengerCount; ++index) {
            ScenarioPassenger passenger{
                "P" + std::to_string(caseIndex) + "-" + std::to_string(index),
                "Passenger " + std::to_string(index),
                static_cast<int>((caseIndex * 3U + index * 2U) % 6U),
                static_cast<int>(1U + (caseIndex + index * 3U) % 4U),
                static_cast<int>(1U + (caseIndex + index) % 3U),
                {}
            };
            if ((caseIndex + index) % 3U == 0U) {
                passenger.eligibleLaneIndices.push_back(
                    (caseIndex + index) % laneCount
                );
            }
            scenario.passengers.push_back(std::move(passenger));
        }
        for (const AssignmentStrategy strategy : strategies) {
            SCOPED_TRACE(
                "case=" + std::to_string(caseIndex) +
                ", strategy=" + toString(strategy)
            );
            expectMatchesReference(scenario, strategy);
        }
    }
}

TEST(AirportScenarioOracleTests, AssignmentStrategiesCanProduceDifferentSchedules) {
    const AirportScenario scenario{
        "Strategy divergence",
        "deterministic test data",
        {2U, 2U, {}, 1000, {}},
        {
            {"A", "A", 0, 10, 3, {}},
            {"B", "B", 1, 9, 3, {}},
            {"C", "C", 10, 1, 3, {}}
        },
        true
    };
    const ScenarioRunResult shortest = AirportScenarioRunner::runStrategy(
        scenario,
        AssignmentStrategy::ShortestWorkload
    );
    const ScenarioRunResult roundRobin = AirportScenarioRunner::runStrategy(
        scenario,
        AssignmentStrategy::RoundRobin
    );
    ASSERT_EQ(shortest.passengers.size(), 3U);
    ASSERT_EQ(roundRobin.passengers.size(), 3U);
    EXPECT_EQ(shortest.passengers[2].passenger.id, "C");
    EXPECT_EQ(roundRobin.passengers[2].passenger.id, "C");
    EXPECT_EQ(shortest.passengers[2].laneId, "Lane-2");
    EXPECT_EQ(roundRobin.passengers[2].laneId, "Lane-1");
    EXPECT_EQ(shortest.passengers[2].serviceStartMinute, 10);
    EXPECT_EQ(roundRobin.passengers[2].serviceStartMinute, 10);
    EXPECT_EQ(shortest.totalWaitMinutes, roundRobin.totalWaitMinutes);
    EXPECT_EQ(shortest.makespanMinutes, roundRobin.makespanMinutes);
    EXPECT_DOUBLE_EQ(shortest.throughputPerHour, roundRobin.throughputPerHour);
}

TEST(AirportScenarioSimulationTests, RespectsLaneOutagesAndStaffingConstraints) {
    const AirportScenario scenario = loadText(
        "P1,One,0,3,1\n"
        "P2,Two,0,3,2\n"
        "P3,Three,0,3,3\n",
        {3U, 3U, {{0U, 0, 5U}}}
    );
    const ScenarioComparison comparison =
        AirportScenarioRunner::compareStrategies(scenario);

    EXPECT_EQ(comparison.roundRobin.passengers[0].laneId, "Lane-2");
    EXPECT_EQ(comparison.roundRobin.passengers[0].serviceStartMinute, 0);
    for (const PassengerServiceRecord& passenger : comparison.roundRobin.passengers) {
        if (passenger.laneId == "Lane-1") {
            EXPECT_GE(passenger.serviceStartMinute, 5);
        }
    }
    EXPECT_THROW(
        loadText("P1,One,0,3,1\n", {2U, 1U, {}}),
        std::invalid_argument
    );
    EXPECT_THROW(
        loadText("P1,One,0,3,1\n", {2U, 3U, {}}),
        std::invalid_argument
    );
    EXPECT_THROW(
        loadText("P1,One,0,3,1\n", {2U, 2U, {{2U, 1, 4}}}),
        std::out_of_range
    );
}

TEST(AirportScenarioSimulationTests, RejectsOverlappingOutagesAndAdversarialFixture) {
    EXPECT_THROW(
        loadText(
            "P1,One,0,3,1\n",
            {2U, 2U, {{0U, 1, 4}, {0U, 3, 5}}}
        ),
        std::invalid_argument
    );
    EXPECT_THROW(
        AirportScenarioRunner::loadCsvFile(
            std::filesystem::path("data") / "adversarial.csv",
            "Invalid fixture",
            {2U, 2U, {}}
        ),
        std::invalid_argument
    );
}

TEST(AirportScenarioSimulationTests, GeneratesDeterministicLargeSyntheticWorkloads) {
    const ScenarioConfiguration configuration{4U, 4U, {}};
    const AirportScenario first = AirportScenarioRunner::makeSyntheticScenario(
        "Synthetic workload",
        5'000U,
        configuration
    );
    const AirportScenario second = AirportScenarioRunner::makeSyntheticScenario(
        "Synthetic workload",
        5'000U,
        configuration
    );
    ASSERT_TRUE(first.syntheticInput);
    ASSERT_EQ(first.passengers.size(), 5'000U);
    EXPECT_EQ(first.passengers.front().id, second.passengers.front().id);
    EXPECT_EQ(first.passengers.back().arrivalMinute, second.passengers.back().arrivalMinute);
    EXPECT_EQ(first.inputSource.find("deterministic synthetic"), 0U);
    const ScenarioComparison comparison =
        AirportScenarioRunner::compareStrategies(first);
    EXPECT_EQ(comparison.shortestWorkload.completedPassengerCount, 5'000U);
    EXPECT_EQ(comparison.roundRobin.completedPassengerCount, 5'000U);
    EXPECT_GT(comparison.shortestWorkload.estimatedMemoryBytes, 0U);
}

TEST(AirportScenarioExportTests, WritesCompleteEscapedCsvAndJsonComparisonRecords) {
    const AirportScenario scenario = loadText(
        "P1,\"Ada \"\"Ace\"\", Lovelace\",0,4,1\n"
        "P2,Grace Hopper,2,5,2\n"
    );
    const ScenarioComparison comparison =
        AirportScenarioRunner::compareStrategies(scenario);
    std::ostringstream csv;
    std::ostringstream json;
    AirportScenarioRunner::writeCsv(csv, comparison);
    AirportScenarioRunner::writeJson(json, comparison);

    EXPECT_NE(csv.str().find("record_type,run_id,scenario,strategy"), std::string::npos);
    EXPECT_NE(csv.str().find("\"Ada \"\"Ace\"\", Lovelace\""), std::string::npos);
    EXPECT_NE(csv.str().find("recommendation"), std::string::npos);
    EXPECT_NE(csv.str().find("estimated_memory_bytes"), std::string::npos);
    EXPECT_NE(json.str().find("\"correctness_verified\": true"), std::string::npos);
    EXPECT_NE(json.str().find("\"high_risk_passengers\": 1"), std::string::npos);
    EXPECT_NE(json.str().find("\"officer_id\": \"Officer-1\""), std::string::npos);
    EXPECT_NE(json.str().find("Ada \\\"Ace\\\", Lovelace"), std::string::npos);
    EXPECT_NE(json.str().find("\"elapsed_microseconds\""), std::string::npos);
}

TEST(AirportScenarioExportTests, ExportedPassengerRowsMatchSimulationRecords) {
    const AirportScenario scenario = loadText(
        "P1,Ada,0,2,1\n"
        "P2,Grace,0,1,2\n"
    );
    const ScenarioComparison comparison =
        AirportScenarioRunner::compareStrategies(scenario);
    std::ostringstream csv;
    AirportScenarioRunner::writeCsv(csv, comparison);
    const std::array<const ScenarioRunResult*, 3U> runs{
        &comparison.shortestWorkload,
        &comparison.roundRobin,
        &comparison.additionalLaneWhatIf
    };
    std::size_t checkedPassengers = 0U;
    std::istringstream rows(csv.str());
    std::string line;
    ASSERT_TRUE(std::getline(rows, line));
    const std::vector<std::string> headerFields = splitSimpleCsvRow(line);
    ASSERT_EQ(headerFields.size(), 41U);
    while (std::getline(rows, line)) {
        const std::vector<std::string> fields = splitSimpleCsvRow(line);
        ASSERT_EQ(fields.size(), headerFields.size());
        if (fields.empty() || fields[0] != "passenger") {
            continue;
        }
        const auto run = std::find_if(
            runs.begin(),
            runs.end(),
            [&fields](const ScenarioRunResult* candidate) {
                return candidate->runId == fields[1];
            }
        );
        ASSERT_NE(run, runs.end());
        const auto service = std::find_if(
            (*run)->passengers.begin(),
            (*run)->passengers.end(),
            [&fields](const PassengerServiceRecord& candidate) {
                return candidate.passenger.id == fields[5];
            }
        );
        ASSERT_NE(service, (*run)->passengers.end());
        EXPECT_EQ(fields[9], std::to_string(service->passenger.arrivalMinute));
        EXPECT_EQ(fields[10], std::to_string(service->serviceStartMinute));
        EXPECT_EQ(fields[11], std::to_string(service->completionMinute));
        EXPECT_EQ(fields[12], std::to_string(service->waitMinutes));
        EXPECT_EQ(fields[13], service->laneId);
        EXPECT_EQ(fields[14], service->officerId);
        ++checkedPassengers;
    }
    EXPECT_EQ(checkedPassengers, 3U * scenario.passengers.size());
}

TEST(AirportScenarioExportTests, CsvFieldCounterPreservesTrailingEmptyColumns) {
    const std::vector<std::string> fields = splitSimpleCsvRow("value,also-value,");
    ASSERT_EQ(fields.size(), 3U);
    EXPECT_EQ(fields[0], "value");
    EXPECT_EQ(fields[1], "also-value");
    EXPECT_TRUE(fields[2].empty());
}

TEST(AirportScenarioExportTests, JsonEscapesQuotesBackslashesAndControlCharacters) {
    std::string name = "Quote \" backslash \\ tab\t control ";
    name.push_back('\x01');
    const AirportScenario scenario{
        "JSON escaping",
        "test",
        {1U, 1U, {}},
        {{"P1", name, 0, 1, 1, {}}},
        false
    };
    const ScenarioComparison comparison =
        AirportScenarioRunner::compareStrategies(scenario);
    std::ostringstream json;
    AirportScenarioRunner::writeJson(json, comparison);
    EXPECT_NE(
        json.str().find("Quote \\\" backslash \\\\ tab\\t control \\u0001"),
        std::string::npos
    );
}

TEST(AirportScenarioCliTests, RunsBuiltInScenarioAndExportsBothFormats) {
    const std::filesystem::path basePath = temporaryBasePath();
    std::istringstream input(
        "9\n1\n11\n" + basePath.string() + "\n5\n"
    );
    std::ostringstream output;
    CommandLineInterface commandLine(input, output);

    EXPECT_EQ(commandLine.run(), 0);
    EXPECT_NE(output.str().find("Normal operation"), std::string::npos);
    EXPECT_NE(output.str().find("Completed: 5/5"), std::string::npos);
    EXPECT_TRUE(std::filesystem::exists(basePath.string() + ".csv"));
    EXPECT_TRUE(std::filesystem::exists(basePath.string() + ".json"));
    std::filesystem::remove(basePath.string() + ".csv");
    std::filesystem::remove(basePath.string() + ".json");
}

TEST(AirportScenarioCliTests, LoadsUserCsvAndRunsBothStaffingStrategies) {
    const std::filesystem::path basePath = temporaryBasePath();
    const std::filesystem::path inputPath = basePath.string() + "-input.csv";
    {
        std::ofstream csv(inputPath);
        csv << kCsvHeader
            << "P1,Ada Lovelace,0,4,1\n"
            << "P2,Grace Hopper,2,5,2\n";
    }
    std::istringstream input(
        "10\n" + inputPath.string() + "\n2\n2\n11\n" +
        basePath.string() + "\n5\n"
    );
    std::ostringstream output;
    CommandLineInterface commandLine(input, output);

    EXPECT_EQ(commandLine.run(), 0);
    EXPECT_NE(output.str().find("User-provided CSV"), std::string::npos);
    EXPECT_NE(output.str().find("round-robin"), std::string::npos);
    EXPECT_TRUE(std::filesystem::exists(basePath.string() + ".csv"));
    EXPECT_TRUE(std::filesystem::exists(basePath.string() + ".json"));
    std::filesystem::remove(inputPath);
    std::filesystem::remove(basePath.string() + ".csv");
    std::filesystem::remove(basePath.string() + ".json");
}

} // namespace project31
