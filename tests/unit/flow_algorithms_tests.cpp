#include "algorithms/flow/FordFulkerson.hpp"
#include "algorithms/flow/MaximumBipartiteMatching.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace project31::algorithms {
namespace {

void expectValidFlow(
    const FordFulkersonResult& result,
    const std::vector<CapacityEdge>& edges,
    std::size_t vertexCount,
    std::size_t source,
    std::size_t sink
) {
    ASSERT_EQ(result.edgeFlows.size(), edges.size());
    std::vector<std::int64_t> netOutflow(vertexCount, 0);
    for (std::size_t index = 0; index < edges.size(); ++index) {
        const FlowEdgeResult& flow = result.edgeFlows[index];
        EXPECT_EQ(flow.from, edges[index].from);
        EXPECT_EQ(flow.to, edges[index].to);
        EXPECT_EQ(flow.capacity, edges[index].capacity);
        EXPECT_GE(flow.flow, 0);
        EXPECT_LE(flow.flow, flow.capacity);
        netOutflow[flow.from] += flow.flow;
        netOutflow[flow.to] -= flow.flow;
    }
    for (std::size_t vertex = 0; vertex < vertexCount; ++vertex) {
        if (vertex != source && vertex != sink) {
            EXPECT_EQ(netOutflow[vertex], 0);
        }
    }
    EXPECT_EQ(netOutflow[source], result.maximumFlow);
    EXPECT_EQ(netOutflow[sink], -result.maximumFlow);
}

void expectValidMatching(
    const MaximumBipartiteMatchingResult& result,
    std::size_t leftSize,
    std::size_t rightSize,
    const std::vector<BipartiteEdge>& edges
) {
    ASSERT_EQ(result.matchedPairs.size(), result.cardinality);
    std::vector<bool> matchedLeft(leftSize, false);
    std::vector<bool> matchedRight(rightSize, false);
    for (const auto& [left, right] : result.matchedPairs) {
        ASSERT_LT(left, leftSize);
        ASSERT_LT(right, rightSize);
        EXPECT_FALSE(matchedLeft[left]);
        EXPECT_FALSE(matchedRight[right]);
        matchedLeft[left] = true;
        matchedRight[right] = true;
        EXPECT_NE(
            std::find_if(
                edges.begin(),
                edges.end(),
                [left, right](const BipartiteEdge& edge) {
                    return edge.left == left && edge.right == right;
                }
            ),
            edges.end()
        );
    }
}

} // namespace

TEST(FordFulkersonTests, FindsKnownMaximumFlowAndReturnsFeasibleEdgeFlows) {
    const std::vector<CapacityEdge> edges{
        {0, 1, 16}, {0, 2, 13}, {1, 2, 10}, {2, 1, 4}, {1, 3, 12},
        {3, 2, 9}, {2, 4, 14}, {4, 3, 7}, {3, 5, 20}, {4, 5, 4}
    };
    const FordFulkersonResult result = fordFulkersonMaxFlow(6, edges, 0, 5);

    EXPECT_EQ(result.maximumFlow, 23);
    expectValidFlow(result, edges, 6, 0, 5);
}

TEST(FordFulkersonTests, UsesReverseResidualEdgesToRerouteAnEarlierAugmentation) {
    const std::vector<CapacityEdge> edges{
        {0, 1, 1}, {0, 2, 1}, {1, 3, 1}, {1, 4, 1},
        {2, 3, 1}, {3, 5, 1}, {4, 5, 1}
    };
    const FordFulkersonResult result = fordFulkersonMaxFlow(6, edges, 0, 5);

    EXPECT_EQ(result.maximumFlow, 2);
    EXPECT_EQ(result.edgeFlows[2].flow, 0);
    EXPECT_EQ(result.edgeFlows[3].flow, 1);
    EXPECT_EQ(result.edgeFlows[4].flow, 1);
    expectValidFlow(result, edges, 6, 0, 5);
}

TEST(FordFulkersonTests, HandlesParallelAndZeroCapacityEdgesIndependently) {
    const std::vector<CapacityEdge> edges{
        {0, 1, 3}, {0, 1, 4}, {1, 2, 6}, {0, 2, 1}, {0, 3, 0}, {3, 2, 0}
    };
    const FordFulkersonResult result = fordFulkersonMaxFlow(4, edges, 0, 2);

    EXPECT_EQ(result.maximumFlow, 7);
    EXPECT_EQ(result.edgeFlows[0].flow, 3);
    EXPECT_EQ(result.edgeFlows[1].flow, 3);
    EXPECT_EQ(result.edgeFlows[2].flow, 6);
    EXPECT_EQ(result.edgeFlows[3].flow, 1);
    EXPECT_EQ(result.edgeFlows[4].flow, 0);
    EXPECT_EQ(result.edgeFlows[5].flow, 0);
    expectValidFlow(result, edges, 4, 0, 2);
}

TEST(FordFulkersonTests, ReturnsZeroWhenTheSinkIsDisconnected) {
    const std::vector<CapacityEdge> edges{{0, 1, 5}, {2, 3, 9}};
    const FordFulkersonResult result = fordFulkersonMaxFlow(4, edges, 0, 3);

    EXPECT_EQ(result.maximumFlow, 0);
    for (const FlowEdgeResult& edge : result.edgeFlows) {
        EXPECT_EQ(edge.flow, 0);
    }
    expectValidFlow(result, edges, 4, 0, 3);
}

TEST(FordFulkersonTests, ValidatesGraphAndDefinesSourceEqualToSinkAsZeroFlow) {
    EXPECT_THROW(fordFulkersonMaxFlow(0, {}, 0, 0), std::invalid_argument);
    EXPECT_THROW(fordFulkersonMaxFlow(2, {}, 2, 1), std::invalid_argument);
    EXPECT_THROW(fordFulkersonMaxFlow(2, {}, 0, 2), std::invalid_argument);
    EXPECT_THROW(
        fordFulkersonMaxFlow(2, {{0, 2, 1}}, 0, 1),
        std::invalid_argument
    );
    EXPECT_THROW(
        fordFulkersonMaxFlow(2, {{0, 1, -1}}, 0, 1),
        std::invalid_argument
    );

    const std::vector<CapacityEdge> edges{{0, 1, 4}, {1, 0, 3}};
    const FordFulkersonResult sameTerminal =
        fordFulkersonMaxFlow(2, edges, 0, 0);
    EXPECT_EQ(sameTerminal.maximumFlow, 0);
    EXPECT_EQ(sameTerminal.edgeFlows.size(), edges.size());
    EXPECT_EQ(sameTerminal.edgeFlows[0].flow, 0);
    EXPECT_EQ(sameTerminal.edgeFlows[1].flow, 0);
}

TEST(FordFulkersonTests, DetectsMaximumFlowOverflow) {
    EXPECT_THROW(
        fordFulkersonMaxFlow(
            2,
            {{0, 1, std::numeric_limits<std::int64_t>::max()}, {0, 1, 1}},
            0,
            1
        ),
        std::overflow_error
    );
}

TEST(MaximumBipartiteMatchingTests, FindsMaximumMatchingAndValidAssignments) {
    const std::vector<BipartiteEdge> edges{
        {0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 0}, {2, 2}
    };
    const MaximumBipartiteMatchingResult result =
        maximumBipartiteMatching(3, 3, edges);

    EXPECT_EQ(result.cardinality, 3U);
    expectValidMatching(result, 3, 3, edges);
}

TEST(MaximumBipartiteMatchingTests, RematchesThroughAnAugmentingPath) {
    const std::vector<BipartiteEdge> edges{{0, 0}, {0, 1}, {1, 0}};
    const MaximumBipartiteMatchingResult result =
        maximumBipartiteMatching(2, 2, edges);

    EXPECT_EQ(result.cardinality, 2U);
    EXPECT_EQ(
        result.matchedPairs,
        (std::vector<std::pair<std::size_t, std::size_t>>{{0, 1}, {1, 0}})
    );
    expectValidMatching(result, 2, 2, edges);
}

TEST(MaximumBipartiteMatchingTests, SupportsCompleteAndUnequalPartitions) {
    std::vector<BipartiteEdge> completeEdges;
    for (std::size_t left = 0; left < 4; ++left) {
        for (std::size_t right = 0; right < 2; ++right) {
            completeEdges.push_back({left, right});
        }
    }
    const MaximumBipartiteMatchingResult result =
        maximumBipartiteMatching(4, 2, completeEdges);

    EXPECT_EQ(result.cardinality, 2U);
    expectValidMatching(result, 4, 2, completeEdges);
}

TEST(MaximumBipartiteMatchingTests, HandlesEmptyPartitionsAndIsolatedVertices) {
    EXPECT_EQ(maximumBipartiteMatching(0, 3, {}).cardinality, 0U);
    EXPECT_EQ(maximumBipartiteMatching(3, 0, {}).cardinality, 0U);
    const MaximumBipartiteMatchingResult isolated =
        maximumBipartiteMatching(3, 4, {});
    EXPECT_EQ(isolated.cardinality, 0U);
    EXPECT_TRUE(isolated.matchedPairs.empty());
}

TEST(MaximumBipartiteMatchingTests, IgnoresDuplicateEdgesDeterministically) {
    const std::vector<BipartiteEdge> edges{
        {0, 0}, {0, 0}, {0, 1}, {1, 0}, {1, 0}
    };
    const auto first = maximumBipartiteMatching(2, 2, edges);
    const auto second = maximumBipartiteMatching(2, 2, edges);

    EXPECT_EQ(first.cardinality, 2U);
    EXPECT_EQ(first.matchedPairs, second.matchedPairs);
    expectValidMatching(first, 2, 2, edges);
}

TEST(MaximumBipartiteMatchingTests, RejectsInvalidAssignmentEndpoints) {
    EXPECT_THROW(maximumBipartiteMatching(1, 1, {{1, 0}}), std::invalid_argument);
    EXPECT_THROW(maximumBipartiteMatching(1, 1, {{0, 1}}), std::invalid_argument);
}

} // namespace project31::algorithms
