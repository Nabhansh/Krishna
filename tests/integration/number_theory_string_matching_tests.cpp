#include "simulator/AirportOperations.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace project31 {

TEST(AirportSchedulingTests, ReconcilesPeriodicMaintenanceCycles) {
    const auto schedule =
        AirportOperations::reconcileMaintenanceSchedule({3, 5}, {7, 9});

    EXPECT_EQ(schedule.solution, 59);
    EXPECT_EQ(schedule.combinedModulus, 63);
}

TEST(AirportOperationalLogTests, FindsFlightCodesWithBothSearchStrategies) {
    const std::string log =
        "ARRIVAL FLT-217 gate; FLT-817 delayed; FLT-217 departed";
    const std::string flightCode = "FLT-217";
    const std::vector<std::size_t> expected{8, 39};

    EXPECT_EQ(
        AirportOperations::findFlightCodesInLogNaive(log, flightCode),
        expected
    );
    EXPECT_EQ(
        AirportOperations::findFlightCodesInLogRabinKarp(log, flightCode),
        expected
    );
    EXPECT_EQ(
        AirportOperations::findFlightCodesInLogFiniteAutomaton(log, flightCode),
        expected
    );
    EXPECT_EQ(
        AirportOperations::findFlightCodesInLogKnuthMorrisPratt(log, flightCode),
        expected
    );
}

} // namespace project31
