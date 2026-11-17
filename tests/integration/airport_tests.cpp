#include "core/Airport.hpp"
#include "core/SecurityLane.hpp"
#include "core/SecurityOfficer.hpp"

#include <gtest/gtest.h>

namespace project31 {
namespace {

Airport makeAirport() {
    Airport airport("Test Airport");
    airport.queueManager().addLane(
        SecurityLane("L1", SecurityOfficer("O1", "Officer One"))
    );
    airport.queueManager().addLane(
        SecurityLane("L2", SecurityOfficer("O2", "Officer Two"))
    );
    return airport;
}

} // namespace

TEST(AirportTests, RoutesArrivalsToLeastLoadedLaneAndCalculatesWait) {
    Airport airport = makeAirport();
    airport.registerPassenger(Passenger("P1", "First Passenger", 6), 0);
    airport.registerPassenger(Passenger("P2", "Second Passenger", 3), 0);
    airport.registerPassenger(Passenger("P3", "Third Passenger", 2), 1);

    const ArrivalResult first = airport.passengerArrives("P1", 2);
    const ArrivalResult second = airport.passengerArrives("P2", 3);
    const ArrivalResult third = airport.passengerArrives("P3", 4);

    EXPECT_EQ(first.laneId, "L1");
    EXPECT_EQ(first.estimatedWaitMinutes, 0);
    EXPECT_EQ(second.laneId, "L2");
    EXPECT_EQ(second.estimatedWaitMinutes, 0);
    EXPECT_EQ(third.laneId, "L2");
    EXPECT_EQ(third.estimatedWaitMinutes, 3);
    EXPECT_EQ(airport.queueManager().estimatedWaitMinutes("L2"), 5);
}

TEST(AirportTests, ScreeningUpdatesStatusAndReleasesQueueWorkload) {
    Airport airport = makeAirport();
    airport.registerPassenger(Passenger("P1", "First Passenger", 5), 0);
    const ArrivalResult arrival = airport.passengerArrives("P1", 1);
    EXPECT_EQ(arrival.estimatedWaitMinutes, 0);

    EXPECT_TRUE(airport.screenNext("L1", 6));
    ASSERT_NE(airport.findPassenger("P1"), nullptr);
    EXPECT_EQ(airport.findPassenger("P1")->status(), PassengerStatus::Screened);
    EXPECT_FALSE(airport.screenNext("L1", 7));
    EXPECT_EQ(airport.queueManager().estimatedWaitMinutes("L1"), 0);
    EXPECT_EQ(airport.events().size(), 3U);
}

TEST(AirportTests, RejectsInvalidAndDuplicateRegistrations) {
    Airport airport = makeAirport();

    EXPECT_THROW(
        airport.registerPassenger(Passenger(" ", "Name", 2), 0),
        std::invalid_argument
    );
    airport.registerPassenger(Passenger("P1", "Name", 2), 0);
    EXPECT_THROW(
        airport.registerPassenger(Passenger("P1", "Other Name", 2), 0),
        std::invalid_argument
    );
}

TEST(AirportTests, DoesNotMutatePassengerWhenArrivalCannotBeRouted) {
    Airport airport("No Lanes Airport");
    airport.registerPassenger(Passenger("P1", "Passenger One", 2), 0);

    EXPECT_THROW((void)airport.passengerArrives("P1", 1), std::logic_error);
    ASSERT_NE(airport.findPassenger("P1"), nullptr);
    EXPECT_EQ(airport.findPassenger("P1")->status(), PassengerStatus::Registered);
}

TEST(AirportTests, QueueWorkloadUsesWideIntegerArithmetic) {
    Airport airport = makeAirport();
    airport.registerPassenger(Passenger("P1", "First Passenger", 2'000'000'000), 0);
    airport.registerPassenger(Passenger("P2", "Second Passenger", 2'000'000'000), 0);
    airport.registerPassenger(Passenger("P3", "Third Passenger", 2'000'000'000), 0);

    const ArrivalResult first = airport.passengerArrives("P1", 1);
    const ArrivalResult second = airport.passengerArrives("P2", 2);
    EXPECT_EQ(first.estimatedWaitMinutes, 0);
    EXPECT_EQ(second.estimatedWaitMinutes, 0);
    const ArrivalResult third = airport.passengerArrives("P3", 3);

    EXPECT_EQ(third.laneId, "L1");
    EXPECT_EQ(third.estimatedWaitMinutes, 2'000'000'000);
    EXPECT_EQ(airport.queueManager().estimatedWaitMinutes("L1"), 4'000'000'000LL);
}

} // namespace project31
