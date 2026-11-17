#pragma once

#include "algorithms/greedy/ActivitySelection.hpp"
#include "algorithms/greedy/Dijkstra.hpp"
#include "algorithms/greedy/HuffmanCoding.hpp"
#include "algorithms/greedy/IntervalPartitioning.hpp"
#include "algorithms/greedy/Kruskal.hpp"
#include "algorithms/greedy/Prim.hpp"
#include "algorithms/dynamic_programming/Knapsack.hpp"
#include "algorithms/dynamic_programming/LongestCommonSubsequence.hpp"
#include "algorithms/dynamic_programming/MatrixChainMultiplication.hpp"
#include "algorithms/dynamic_programming/OptimalBinarySearchTree.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace project31 {

class AirportOperations {
public:
    [[nodiscard]] static std::vector<algorithms::Activity> scheduleScreeningActivities(
        std::vector<algorithms::Activity> appointments
    );
    [[nodiscard]] static algorithms::IntervalPartition allocateScreeningLanes(
        std::vector<algorithms::ScreeningInterval> screeningIntervals
    );
    [[nodiscard]] static std::vector<std::optional<std::int64_t>> planAirportRoutes(
        std::size_t airportCount,
        const std::vector<algorithms::DirectedWeightedEdge>& routes,
        std::size_t departureAirport
    );
    [[nodiscard]] static algorithms::SpanningForest planInfrastructureWithPrim(
        std::size_t siteCount,
        const std::vector<algorithms::UndirectedWeightedEdge>& connections
    );
    [[nodiscard]] static algorithms::SpanningForest planInfrastructureWithKruskal(
        std::size_t siteCount,
        std::vector<algorithms::UndirectedWeightedEdge> connections
    );
    [[nodiscard]] static algorithms::HuffmanCodec eventLogCodec(
        const std::string& eventLog
    );
    [[nodiscard]] static algorithms::KnapsackResult optimizeAirportUpgrades(
        const std::vector<std::int64_t>& resourceWeights,
        const std::vector<std::int64_t>& operationalValues,
        std::int64_t budget
    );
    [[nodiscard]] static algorithms::MatrixChainResult planAnalyticsMultiplication(
        const std::vector<std::int64_t>& matrixDimensions
    );
    [[nodiscard]] static algorithms::LongestCommonSubsequenceResult compareOperationalEventHistories(
        const std::string& firstHistory,
        const std::string& secondHistory
    );
    [[nodiscard]] static algorithms::OptimalBinarySearchTreeResult buildOperationalLookupTree(
        const std::vector<std::string>& sortedUniqueKeys,
        const std::vector<double>& successfulProbabilities,
        const std::vector<double>& unsuccessfulProbabilities
    );
};

} // namespace project31
