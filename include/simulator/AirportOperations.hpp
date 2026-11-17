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
#include "algorithms/graph/BellmanFord.hpp"
#include "algorithms/graph/FloydWarshall.hpp"
#include "algorithms/backtracking/NQueens.hpp"
#include "algorithms/backtracking/SubsetSum.hpp"
#include "algorithms/branch_bound/FifoBranchAndBound.hpp"
#include "algorithms/branch_bound/LifoBranchAndBound.hpp"

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
    [[nodiscard]] static algorithms::BellmanFordResult planAirportRoutesWithBellmanFord(
        std::size_t airportCount,
        const std::vector<algorithms::DirectedWeightedEdge>& routes,
        std::size_t departureAirport
    );
    [[nodiscard]] static algorithms::FloydWarshallResult planAllAirportRoutesWithFloydWarshall(
        std::size_t airportCount,
        const std::vector<algorithms::DirectedWeightedEdge>& routes
    );
    [[nodiscard]] static std::vector<std::vector<std::size_t>> assignScreeningPositions(
        std::size_t positionCount
    );
    [[nodiscard]] static std::optional<std::vector<std::size_t>> selectResourcePackages(
        const std::vector<std::int64_t>& packageCapacities,
        std::int64_t targetCapacity
    );
    [[nodiscard]] static algorithms::BranchBoundKnapsackResult selectEquipmentFifoBranchAndBound(
        const std::vector<algorithms::BranchBoundItem>& equipment,
        std::int64_t weightLimit
    );
    [[nodiscard]] static algorithms::BranchBoundKnapsackResult selectEquipmentLifoBranchAndBound(
        const std::vector<algorithms::BranchBoundItem>& equipment,
        std::int64_t weightLimit
    );
};

} // namespace project31
