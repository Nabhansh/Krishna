#include "core/Airport.hpp"
#include "core/PassengerIndex.hpp"
#include "simulator/QueueWorkloadIndex.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <vector>

namespace project31 {

TEST(PassengerIndexTests, MaintainsSortedIdLookupAndPriorityBuckets) {
    PassengerIndex index;
    index.add(Passenger("P-104", "Grace Hopper", 4, 2));
    index.add(Passenger("P-002", "Ada Lovelace", 3, 1));
    index.add(Passenger("P-019", "Katherine Johnson", 5, 1));

    EXPECT_TRUE(index.containsId("P-002"));
    EXPECT_FALSE(index.containsId("P-999"));
    EXPECT_EQ(
        index.passengerIdsSorted(),
        (std::vector<std::string>{"P-002", "P-019", "P-104"})
    );
    EXPECT_EQ(
        index.idsForPriority(1),
        (std::vector<std::string>{"P-002", "P-019"})
    );
    EXPECT_TRUE(index.idsForPriority(9).empty());
}

TEST(PassengerIndexTests, RejectsDuplicateIds) {
    PassengerIndex index;
    index.add(Passenger("P-001", "Ada Lovelace", 4));

    EXPECT_THROW(index.add(Passenger("P-001", "Grace Hopper", 5)), std::invalid_argument);
    EXPECT_EQ(index.passengerIdsSorted().size(), 1U);
}

TEST(PassengerIndexTests, SortsRegisteredIdsUsingEitherImplementedSort) {
    Airport airport("Index Test Airport");
    airport.registerPassenger(Passenger("P-104", "Grace Hopper", 4), 0);
    airport.registerPassenger(Passenger("P-002", "Ada Lovelace", 3), 0);
    airport.registerPassenger(Passenger("P-019", "Katherine Johnson", 5), 0);

    const std::vector<std::string> expected{"P-002", "P-019", "P-104"};
    EXPECT_EQ(airport.passengerIdsSortedBy(PassengerIdSort::Merge), expected);
    EXPECT_EQ(airport.passengerIdsSortedBy(PassengerIdSort::Quick), expected);
}

TEST(AirportIndexTests, BinarySearchLookupAndPriorityQueriesUseRegisteredPassengers) {
    Airport airport("Passenger Index Airport");
    airport.registerPassenger(Passenger("P-104", "Grace Hopper", 4, 2), 0);
    airport.registerPassenger(Passenger("P-002", "Ada Lovelace", 3, 1), 0);
    airport.registerPassenger(Passenger("P-019", "Katherine Johnson", 5, 1), 0);

    const Passenger* found = airport.findPassenger("P-019");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->name(), "Katherine Johnson");
    EXPECT_EQ(airport.findPassenger("P-999"), nullptr);

    const std::vector<const Passenger*> highestPriority = airport.passengersByPriority(1);
    ASSERT_EQ(highestPriority.size(), 2U);
    EXPECT_EQ(highestPriority[0]->id(), "P-002");
    EXPECT_EQ(highestPriority[1]->id(), "P-019");
    EXPECT_TRUE(airport.passengersByPriority(5).empty());
}

TEST(QueueWorkloadIndexTests, TracksLaneWorkloadThroughArrivalAndScreening) {
    QueueWorkloadIndex index;
    index.addLane("Lane-1");

    EXPECT_EQ(index.workloadMinutes("Lane-1"), 0);
    index.addPassenger("Lane-1", 7);
    index.addPassenger("Lane-1", 4);
    EXPECT_EQ(index.workloadMinutes("Lane-1"), 11);
    index.removePassenger("Lane-1", 7);
    EXPECT_EQ(index.workloadMinutes("Lane-1"), 4);
    index.removePassenger("Lane-1", 4);
    EXPECT_EQ(index.workloadMinutes("Lane-1"), 0);
}

TEST(QueueWorkloadIndexTests, RejectsDuplicateLanesUnknownIdsAndInvalidUpdates) {
    QueueWorkloadIndex index;
    index.addLane("Lane-1");

    EXPECT_THROW(index.addLane("Lane-1"), std::invalid_argument);
    EXPECT_THROW(index.workloadMinutes("missing"), std::out_of_range);
    EXPECT_THROW(index.addPassenger("Lane-1", 0), std::invalid_argument);
    EXPECT_THROW(index.removePassenger("Lane-1", 1), std::logic_error);
}

} // namespace project31
