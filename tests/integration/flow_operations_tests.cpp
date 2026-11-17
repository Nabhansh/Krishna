#include "simulator/AirportOperations.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <utility>
#include <vector>

namespace project31 {

TEST(AirportFlowOperationsTests, PlansPassengerFlowAcrossCheckpointCapacities) {
    const std::vector<algorithms::CapacityEdge> connections{
        {0, 1, 8}, {0, 2, 5}, {1, 2, 3}, {1, 3, 5}, {2, 3, 7}
    };
    const auto result =
        AirportOperations::planCheckpointFlow(4, connections, 0, 3);

    EXPECT_EQ(result.maximumFlow, 12);
    EXPECT_EQ(result.edgeFlows.size(), connections.size());
}

TEST(AirportFlowOperationsTests, AssignsOfficersToCompatibleScreeningLanes) {
    const std::vector<algorithms::BipartiteEdge> allowedAssignments{
        {0, 0}, {0, 1}, {1, 0}, {2, 1}, {2, 2}
    };
    const auto result = AirportOperations::assignOfficersToScreeningLanes(
        3,
        3,
        allowedAssignments
    );

    ASSERT_EQ(result.cardinality, 3U);
    EXPECT_EQ(result.matchedPairs.size(), 3U);
    EXPECT_EQ(result.matchedPairs[0], (std::pair<std::size_t, std::size_t>{0, 1}));
    EXPECT_EQ(result.matchedPairs[1], (std::pair<std::size_t, std::size_t>{1, 0}));
    EXPECT_EQ(result.matchedPairs[2], (std::pair<std::size_t, std::size_t>{2, 2}));
}

} // namespace project31
