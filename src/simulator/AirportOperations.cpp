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

} // namespace project31
