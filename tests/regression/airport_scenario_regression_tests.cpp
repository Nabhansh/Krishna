#include "simulator/AirportScenario.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>

namespace project31 {

TEST(AirportScenarioRegressionTests, NormalFixtureMetricsRemainDeterministic) {
    const AirportScenario scenario = AirportScenarioRunner::loadCsvFile(
        std::filesystem::path("data") / "normal.csv",
        "Normal operation",
        {2U, 2U, {}}
    );
    const ScenarioComparison comparison =
        AirportScenarioRunner::compareStrategies(scenario);

    EXPECT_EQ(comparison.shortestWorkload.passengerCount, 5U);
    EXPECT_EQ(comparison.shortestWorkload.completedPassengerCount, 5U);
    EXPECT_EQ(comparison.shortestWorkload.totalWaitMinutes, 3);
    EXPECT_DOUBLE_EQ(comparison.shortestWorkload.meanWaitMinutes, 0.6);
    EXPECT_EQ(comparison.shortestWorkload.maximumWaitMinutes, 2);
    EXPECT_EQ(comparison.shortestWorkload.makespanMinutes, 14);
    EXPECT_EQ(comparison.shortestWorkload.lanes[0].passengerCount, 3U);
    EXPECT_EQ(comparison.shortestWorkload.lanes[1].passengerCount, 2U);
    EXPECT_EQ(comparison.shortestWorkload.highRiskPassengerCount, 2U);
}

TEST(AirportScenarioRegressionTests, AgedPassengerWinsAgainstLaterHighRiskArrival) {
    ScenarioConfiguration configuration{1U, 1U, {}};
    configuration.priorityAgingIntervalMinutes = 2;
    std::istringstream input(
        "passenger_id,name,arrival_minute,screening_duration_minutes,priority\n"
        "BLOCK,Initial service,0,5,1\n"
        "WAIT,Waiting standard,0,1,3\n"
        "NEW,Later high risk,5,1,1\n"
    );
    const AirportScenario scenario = AirportScenarioRunner::loadCsv(
        input,
        "Priority aging regression",
        configuration
    );
    const ScenarioComparison comparison =
        AirportScenarioRunner::compareStrategies(scenario);
    const auto findService = [&comparison](const std::string& id)
        -> const PassengerServiceRecord& {
        for (const PassengerServiceRecord& service :
             comparison.shortestWorkload.passengers) {
            if (service.passenger.id == id) {
                return service;
            }
        }
        throw std::logic_error("Expected regression passenger was not screened.");
    };

    EXPECT_EQ(findService("WAIT").serviceStartMinute, 5);
    EXPECT_EQ(findService("NEW").serviceStartMinute, 6);
    EXPECT_EQ(comparison.shortestWorkload.completedPassengerCount, 3U);
}

} // namespace project31
