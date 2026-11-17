#include "simulator/AirportOperations.hpp"

#include <utility>

namespace project31 {

std::vector<algorithms::Activity> AirportOperations::scheduleScreeningActivities(
    std::vector<algorithms::Activity> appointments
) {
    return algorithms::selectActivities(std::move(appointments));
}

algorithms::IntervalPartition AirportOperations::allocateScreeningLanes(
    std::vector<algorithms::ScreeningInterval> screeningIntervals
) {
    return algorithms::partitionIntervals(std::move(screeningIntervals));
}

std::vector<std::optional<std::int64_t>> AirportOperations::planAirportRoutes(
    std::size_t airportCount,
    const std::vector<algorithms::DirectedWeightedEdge>& routes,
    std::size_t departureAirport
) {
    return algorithms::dijkstraShortestPaths(airportCount, routes, departureAirport);
}

algorithms::SpanningForest AirportOperations::planInfrastructureWithPrim(
    std::size_t siteCount,
    const std::vector<algorithms::UndirectedWeightedEdge>& connections
) {
    return algorithms::primMinimumSpanningForest(siteCount, connections);
}

algorithms::SpanningForest AirportOperations::planInfrastructureWithKruskal(
    std::size_t siteCount,
    std::vector<algorithms::UndirectedWeightedEdge> connections
) {
    return algorithms::kruskalMinimumSpanningForest(siteCount, std::move(connections));
}

algorithms::HuffmanCodec AirportOperations::eventLogCodec(const std::string& eventLog) {
    return algorithms::HuffmanCodec::fromText(eventLog);
}

algorithms::KnapsackResult AirportOperations::optimizeAirportUpgrades(
    const std::vector<std::int64_t>& resourceWeights,
    const std::vector<std::int64_t>& operationalValues,
    std::int64_t budget
) {
    return algorithms::solveKnapsack01(resourceWeights, operationalValues, budget);
}

algorithms::MatrixChainResult AirportOperations::planAnalyticsMultiplication(
    const std::vector<std::int64_t>& matrixDimensions
) {
    return algorithms::matrixChainMultiplication(matrixDimensions);
}

algorithms::LongestCommonSubsequenceResult AirportOperations::compareOperationalEventHistories(
    const std::string& firstHistory,
    const std::string& secondHistory
) {
    return algorithms::longestCommonSubsequence(firstHistory, secondHistory);
}

algorithms::OptimalBinarySearchTreeResult AirportOperations::buildOperationalLookupTree(
    const std::vector<std::string>& sortedUniqueKeys,
    const std::vector<double>& successfulProbabilities,
    const std::vector<double>& unsuccessfulProbabilities
) {
    return algorithms::optimalBinarySearchTree(
        sortedUniqueKeys,
        successfulProbabilities,
        unsuccessfulProbabilities
    );
}

algorithms::BellmanFordResult AirportOperations::planAirportRoutesWithBellmanFord(
    std::size_t airportCount,
    const std::vector<algorithms::DirectedWeightedEdge>& routes,
    std::size_t departureAirport
) {
    return algorithms::bellmanFordShortestPaths(airportCount, routes, departureAirport);
}

algorithms::FloydWarshallResult AirportOperations::planAllAirportRoutesWithFloydWarshall(
    std::size_t airportCount,
    const std::vector<algorithms::DirectedWeightedEdge>& routes
) {
    return algorithms::floydWarshallShortestPaths(airportCount, routes);
}

std::vector<std::vector<std::size_t>> AirportOperations::assignScreeningPositions(
    std::size_t positionCount
) {
    return algorithms::solveNQueens(positionCount);
}

std::optional<std::vector<std::size_t>> AirportOperations::selectResourcePackages(
    const std::vector<std::int64_t>& packageCapacities,
    std::int64_t targetCapacity
) {
    return algorithms::findSubsetSum(packageCapacities, targetCapacity);
}

algorithms::BranchBoundKnapsackResult AirportOperations::selectEquipmentFifoBranchAndBound(
    const std::vector<algorithms::BranchBoundItem>& equipment,
    std::int64_t weightLimit
) {
    return algorithms::solveKnapsackFifoBranchAndBound(equipment, weightLimit);
}

algorithms::BranchBoundKnapsackResult AirportOperations::selectEquipmentLifoBranchAndBound(
    const std::vector<algorithms::BranchBoundItem>& equipment,
    std::int64_t weightLimit
) {
    return algorithms::solveKnapsackLifoBranchAndBound(equipment, weightLimit);
}

} // namespace project31
