#include "algorithms/graph/BellmanFord.hpp"
#include "algorithms/graph/FloydWarshall.hpp"
#include "simulator/AirportOperations.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace project31::algorithms {

TEST(BellmanFordTests, FindsShortestPathsWithNegativeEdgesAndReconstructsPaths) {
    const std::vector<DirectedWeightedEdge> edges{
        {0, 1, 6}, {0, 2, 7}, {1, 2, 8}, {1, 3, 5}, {1, 4, -4},
        {2, 3, -3}, {2, 4, 9}, {3, 1, -2}, {4, 0, 2}, {4, 3, 7}
    };
    const BellmanFordResult result = bellmanFordShortestPaths(5, edges, 0);

    EXPECT_EQ(result.distances[0], 0);
    EXPECT_EQ(result.distances[1], 2);
    EXPECT_EQ(result.distances[2], 7);
    EXPECT_EQ(result.distances[3], 4);
    EXPECT_EQ(result.distances[4], -2);
    EXPECT_EQ(result.pathTo(0), (std::vector<std::size_t>{0}));
    EXPECT_EQ(result.pathTo(4), (std::vector<std::size_t>{0, 2, 3, 1, 4}));
}

TEST(BellmanFordTests, LeavesDisconnectedVerticesUnreachableAndIgnoresUnreachableCycles) {
    const BellmanFordResult result = bellmanFordShortestPaths(
        5,
        {{0, 1, 2}, {1, 2, -1}, {3, 4, -2}, {4, 3, 1}},
        0
    );
    EXPECT_EQ(result.distances[2], 1);
    EXPECT_FALSE(result.distances[3].has_value());
    EXPECT_FALSE(result.pathTo(4).has_value());
}

TEST(BellmanFordTests, DetectsReachableNegativeCycles) {
    EXPECT_THROW(
        bellmanFordShortestPaths(3, {{0, 1, 1}, {1, 2, -2}, {2, 1, 1}}, 0),
        std::domain_error
    );
}

TEST(BellmanFordTests, RejectsInvalidGraphInputsAndPathDestinations) {
    EXPECT_THROW(bellmanFordShortestPaths(0, {}, 0), std::invalid_argument);
    EXPECT_THROW(bellmanFordShortestPaths(2, {}, 2), std::invalid_argument);
    EXPECT_THROW(
        bellmanFordShortestPaths(2, {{0, 2, 1}}, 0),
        std::invalid_argument
    );
    const BellmanFordResult result = bellmanFordShortestPaths(1, {}, 0);
    EXPECT_THROW(result.pathTo(1), std::out_of_range);
}

TEST(BellmanFordTests, DetectsPositiveAndNegativeDistanceOverflow) {
    EXPECT_THROW(
        bellmanFordShortestPaths(
            3,
            {{0, 1, std::numeric_limits<std::int64_t>::max()}, {1, 2, 1}},
            0
        ),
        std::overflow_error
    );
    EXPECT_THROW(
        bellmanFordShortestPaths(
            3,
            {{0, 1, std::numeric_limits<std::int64_t>::min()}, {1, 2, -1}},
            0
        ),
        std::overflow_error
    );
}

TEST(FloydWarshallTests, FindsAllPairsPathsWithNegativeEdgesAndDisconnectedVertices) {
    const FloydWarshallResult result = floydWarshallShortestPaths(
        6,
        {{0, 1, 6}, {0, 2, 2}, {2, 1, -1}, {1, 3, 3}, {2, 3, 7}, {3, 4, 1}}
    );

    EXPECT_EQ(result.distances[0][1], 1);
    EXPECT_EQ(result.distances[0][3], 4);
    EXPECT_EQ(result.distances[0][4], 5);
    EXPECT_EQ(result.path(0, 4), (std::vector<std::size_t>{0, 2, 1, 3, 4}));
    EXPECT_EQ(result.path(0, 0), (std::vector<std::size_t>{0}));
    EXPECT_FALSE(result.distances[0][5].has_value());
    EXPECT_FALSE(result.path(0, 5).has_value());
}

TEST(FloydWarshallTests, DetectsNegativeCyclesIncludingDisconnectedComponents) {
    EXPECT_THROW(
        floydWarshallShortestPaths(4, {{2, 3, -2}, {3, 2, 1}}),
        std::domain_error
    );
}

TEST(FloydWarshallTests, ValidatesGraphInputsAndDetectsDistanceOverflow) {
    EXPECT_TRUE(floydWarshallShortestPaths(0, {}).distances.empty());
    EXPECT_THROW(
        floydWarshallShortestPaths(2, {{0, 2, 1}}),
        std::invalid_argument
    );
    EXPECT_THROW(
        floydWarshallShortestPaths(
            3,
            {{0, 1, std::numeric_limits<std::int64_t>::max()}, {1, 2, 1}}
        ),
        std::overflow_error
    );
    const FloydWarshallResult result = floydWarshallShortestPaths(1, {});
    EXPECT_THROW(result.path(0, 1), std::out_of_range);
}

TEST(ShortestPathCrossCheckTests, BellmanFordMatchesFloydWarshallWithoutNegativeCycles) {
    const std::vector<DirectedWeightedEdge> edges{
        {0, 1, 6}, {0, 2, 2}, {2, 1, -1}, {1, 3, 3}, {2, 3, 7}, {3, 4, 1}
    };
    const FloydWarshallResult allPairs = floydWarshallShortestPaths(6, edges);
    for (std::size_t source = 0; source < 6; ++source) {
        const BellmanFordResult singleSource =
            bellmanFordShortestPaths(6, edges, source);
        for (std::size_t destination = 0; destination < 6; ++destination) {
            EXPECT_EQ(
                singleSource.distances[destination],
                allPairs.distances[source][destination]
            );
        }
    }
}

} // namespace project31::algorithms

namespace project31 {

TEST(AirportGraphOperationsTests, PlansSingleSourceAndAllPairsFacilityRoutes) {
    const std::vector<algorithms::DirectedWeightedEdge> routes{
        {0, 1, 6}, {0, 2, 2}, {2, 1, -1}, {1, 3, 3}, {2, 3, 7}, {3, 4, 1}
    };
    const auto singleSource =
        AirportOperations::planAirportRoutesWithBellmanFord(5, routes, 0);
    const auto allPairs =
        AirportOperations::planAllAirportRoutesWithFloydWarshall(5, routes);

    EXPECT_EQ(singleSource.distances[4], 5);
    EXPECT_EQ(allPairs.distances[0][4], 5);
    EXPECT_EQ(singleSource.pathTo(4), allPairs.path(0, 4));
}

} // namespace project31
