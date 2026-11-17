#include "algorithms/greedy/ActivitySelection.hpp"
#include "algorithms/greedy/Dijkstra.hpp"
#include "algorithms/greedy/HuffmanCoding.hpp"
#include "algorithms/greedy/IntervalPartitioning.hpp"
#include "algorithms/greedy/Kruskal.hpp"
#include "algorithms/greedy/Prim.hpp"
#include "simulator/AirportOperations.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace project31::algorithms {

TEST(ActivitySelectionTests, SelectsMaximumCompatibleAppointments) {
    const auto selected = selectActivities({
        {"boarding", 1, 4}, {"screen-2", 3, 5}, {"screen-3", 0, 6},
        {"screen-4", 5, 7}, {"screen-5", 3, 9}, {"screen-6", 5, 9},
        {"screen-7", 6, 10}, {"screen-8", 8, 11}, {"screen-9", 8, 12},
        {"screen-10", 2, 14}, {"screen-11", 12, 16}
    });
    ASSERT_EQ(selected.size(), 4U);
    EXPECT_EQ(selected[0].id, "boarding");
    EXPECT_EQ(selected[1].id, "screen-4");
    EXPECT_EQ(selected[2].id, "screen-8");
    EXPECT_EQ(selected[3].id, "screen-11");
}

TEST(ActivitySelectionTests, HandlesEmptyTouchingAndInvalidAppointments) {
    EXPECT_TRUE(selectActivities({}).empty());
    EXPECT_EQ(
        selectActivities({{"a", 0, 2}, {"b", 2, 2}, {"c", 2, 4}}).size(),
        3U
    );
    EXPECT_THROW(selectActivities({{"bad", 4, 3}}), std::invalid_argument);
    EXPECT_THROW(selectActivities({{"same", 0, 1}, {"same", 1, 2}}), std::invalid_argument);
    EXPECT_THROW(selectActivities({{" ", 0, 1}}), std::invalid_argument);
}

TEST(IntervalPartitioningTests, AllocatesMinimumNumberOfScreeningLanes) {
    const IntervalPartition plan = partitionIntervals({
        {"A", 0, 3}, {"B", 1, 4}, {"C", 2, 5}, {"D", 4, 7}, {"E", 5, 8}
    });
    EXPECT_EQ(plan.resourceCount, 3U);
    ASSERT_EQ(plan.assignments.size(), 5U);
    EXPECT_EQ(plan.assignments[0].intervalId, "A");
    EXPECT_EQ(plan.assignments[0].resource, 0U);
    EXPECT_EQ(plan.assignments[3].intervalId, "D");
    EXPECT_EQ(plan.assignments[3].resource, 0U);
    EXPECT_EQ(partitionIntervals({}).resourceCount, 0U);
}

TEST(IntervalPartitioningTests, ValidatesIdsAndIntervalBounds) {
    EXPECT_THROW(partitionIntervals({{"bad", 5, 4}}), std::invalid_argument);
    EXPECT_THROW(
        partitionIntervals({{"duplicate", 0, 1}, {"duplicate", 1, 2}}),
        std::invalid_argument
    );
    EXPECT_THROW(partitionIntervals({{"\t", 0, 1}}), std::invalid_argument);
}

TEST(HuffmanCodingTests, EncodesAndDecodesPrefixFreeCodes) {
    const HuffmanCodec codec = HuffmanCodec::fromText("airport screening");
    const std::string encoded = codec.encode("screening");
    EXPECT_EQ(codec.decode(encoded), "screening");
    for (const auto& [symbol, code] : codec.codes()) {
        (void)symbol;
        EXPECT_FALSE(code.empty());
        for (const auto& [otherSymbol, otherCode] : codec.codes()) {
            if (otherSymbol != symbol) {
                EXPECT_FALSE(otherCode.starts_with(code));
            }
        }
    }
}

TEST(HuffmanCodingTests, HandlesSingleSymbolEmptyInputAndInvalidBits) {
    const HuffmanCodec single = HuffmanCodec::fromText("xxxxx");
    EXPECT_EQ(single.codes().at('x'), "0");
    EXPECT_EQ(single.decode(single.encode("xxx")), "xxx");
    EXPECT_TRUE(HuffmanCodec::fromText("").decode("").empty());
    EXPECT_THROW(single.encode("y"), std::invalid_argument);
    EXPECT_THROW(single.decode("2"), std::invalid_argument);
    EXPECT_THROW(single.decode("1"), std::invalid_argument);
}

TEST(HuffmanCodingTests, RejectsZeroFrequenciesAndIncompleteCodes) {
    EXPECT_THROW(HuffmanCodec::fromFrequencies({{'x', 0}}), std::invalid_argument);
    const HuffmanCodec codec =
        HuffmanCodec::fromFrequencies({{'a', 1}, {'b', 1}, {'c', 10}});
    EXPECT_THROW(codec.decode("0"), std::invalid_argument);
    EXPECT_THROW(
        HuffmanCodec::fromFrequencies({
            {'a', std::numeric_limits<std::uint64_t>::max()},
            {'b', 1}
        }),
        std::overflow_error
    );
}

TEST(DijkstraTests, FindsAirportRoutesAndMarksDisconnectedDestinations) {
    const auto distances = dijkstraShortestPaths(
        5,
        {{0, 1, 4}, {0, 2, 1}, {2, 1, 2}, {1, 3, 1}, {2, 3, 5}},
        0
    );
    EXPECT_EQ(distances[0], 0);
    EXPECT_EQ(distances[1], 3);
    EXPECT_EQ(distances[2], 1);
    EXPECT_EQ(distances[3], 4);
    EXPECT_FALSE(distances[4].has_value());
}

TEST(DijkstraTests, RejectsNegativeWeightsAndInvalidGraphInputs) {
    EXPECT_THROW(dijkstraShortestPaths(2, {{0, 1, -1}}, 0), std::invalid_argument);
    EXPECT_THROW(dijkstraShortestPaths(2, {{0, 2, 1}}, 0), std::invalid_argument);
    EXPECT_THROW(dijkstraShortestPaths(0, {}, 0), std::invalid_argument);
}

TEST(DijkstraTests, DetectsPathLengthOverflow) {
    EXPECT_THROW(
        dijkstraShortestPaths(
            3,
            {{0, 1, std::numeric_limits<std::int64_t>::max()}, {1, 2, 1}},
            0
        ),
        std::overflow_error
    );
}

const std::vector<UndirectedWeightedEdge> kAirportLinks{
    {0, 1, 4}, {0, 2, 3}, {1, 2, 1}, {1, 3, 2},
    {2, 3, 4}, {3, 4, 2}, {4, 5, 6}
};

TEST(PrimTests, BuildsMinimumSpanningTree) {
    const SpanningForest forest = primMinimumSpanningForest(6, kAirportLinks);
    EXPECT_EQ(forest.componentCount, 1U);
    EXPECT_EQ(forest.edges.size(), 5U);
    EXPECT_EQ(forest.totalWeight, 14);
}

TEST(PrimTests, ReturnsMinimumSpanningForestForDisconnectedGraph) {
    const SpanningForest forest = primMinimumSpanningForest(
        4,
        {{0, 1, -2}, {2, 3, 5}}
    );
    EXPECT_EQ(forest.componentCount, 2U);
    EXPECT_EQ(forest.totalWeight, 3);
    EXPECT_EQ(forest.edges.size(), 2U);
    EXPECT_EQ(primMinimumSpanningForest(3, {}).componentCount, 3U);
}

TEST(PrimTests, RejectsOutOfRangeVerticesAndWeightOverflow) {
    EXPECT_THROW(primMinimumSpanningForest(2, {{0, 2, 1}}), std::invalid_argument);
    EXPECT_THROW(
        primMinimumSpanningForest(
            3,
            {{0, 1, std::numeric_limits<std::int64_t>::max()},
             {1, 2, std::numeric_limits<std::int64_t>::max()}}
        ),
        std::overflow_error
    );
}

TEST(KruskalTests, BuildsMinimumSpanningTreeIndependently) {
    const SpanningForest forest = kruskalMinimumSpanningForest(6, kAirportLinks);
    EXPECT_EQ(forest.componentCount, 1U);
    EXPECT_EQ(forest.edges.size(), 5U);
    EXPECT_EQ(forest.totalWeight, 14);
}

TEST(KruskalTests, ReturnsMinimumSpanningForestAndHandlesEmptyGraph) {
    const SpanningForest forest = kruskalMinimumSpanningForest(4, {{0, 1, -2}, {2, 3, 5}});
    EXPECT_EQ(forest.componentCount, 2U);
    EXPECT_EQ(forest.totalWeight, 3);
    const SpanningForest empty = kruskalMinimumSpanningForest(0, {});
    EXPECT_EQ(empty.componentCount, 0U);
    EXPECT_TRUE(empty.edges.empty());
}

TEST(KruskalTests, RejectsInvalidVerticesAndDetectsWeightOverflow) {
    EXPECT_THROW(kruskalMinimumSpanningForest(2, {{0, 2, 1}}), std::invalid_argument);
    EXPECT_THROW(
        kruskalMinimumSpanningForest(
            3,
            {{0, 1, std::numeric_limits<std::int64_t>::max()},
             {1, 2, std::numeric_limits<std::int64_t>::max()}}
        ),
        std::overflow_error
    );
}

} // namespace project31::algorithms

namespace project31 {

TEST(AirportOperationsTests, ConnectsGreedyAlgorithmsToOperationalTasks) {
    const auto appointments =
        AirportOperations::scheduleScreeningActivities({{"S1", 0, 2}, {"S2", 2, 4}});
    EXPECT_EQ(appointments.size(), 2U);

    const auto lanes = AirportOperations::allocateScreeningLanes({{"P1", 0, 5}, {"P2", 1, 3}});
    EXPECT_EQ(lanes.resourceCount, 2U);

    const auto routes = AirportOperations::planAirportRoutes(2, {{0, 1, 12}}, 0);
    EXPECT_EQ(routes[1], 12);

    const auto prim = AirportOperations::planInfrastructureWithPrim(2, {{0, 1, 7}});
    const auto kruskal = AirportOperations::planInfrastructureWithKruskal(2, {{0, 1, 7}});
    EXPECT_EQ(prim.totalWeight, kruskal.totalWeight);

    const auto codec = AirportOperations::eventLogCodec("arrival screened");
    EXPECT_EQ(codec.decode(codec.encode("screened")), "screened");

    const auto upgrades =
        AirportOperations::optimizeAirportUpgrades({4, 3, 5}, {8, 6, 10}, 8);
    EXPECT_EQ(upgrades.maximumValue, 16);
    EXPECT_EQ(upgrades.selectedIndices, (std::vector<std::size_t>{0, 1}));

    const auto analytics =
        AirportOperations::planAnalyticsMultiplication({10, 30, 5, 60});
    EXPECT_EQ(analytics.minimumCost, 4500);
    EXPECT_EQ(analytics.parenthesization, "(A1 x (A2 x A3))");
}

} // namespace project31
