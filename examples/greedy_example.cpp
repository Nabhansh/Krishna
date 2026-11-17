#include "simulator/AirportOperations.hpp"

#include <iostream>

int main() {
    using namespace project31;
    using namespace project31::algorithms;

    const auto schedule = AirportOperations::scheduleScreeningActivities({
        {"domestic-1", 0, 20},
        {"international-1", 15, 45},
        {"domestic-2", 20, 40}
    });
    std::cout << "Selected screening appointments: " << schedule.size() << '\n';

    const IntervalPartition lanes = AirportOperations::allocateScreeningLanes({
        {"flight-A", 0, 35},
        {"flight-B", 10, 50},
        {"flight-C", 35, 65}
    });
    std::cout << "Minimum screening lanes required: " << lanes.resourceCount << '\n';

    const auto routes = AirportOperations::planAirportRoutes(
        4,
        {{0, 1, 12}, {0, 2, 5}, {2, 1, 3}, {1, 3, 8}, {2, 3, 20}},
        0
    );
    std::cout << "Shortest route from terminal 0 to terminal 3: "
              << *routes[3] << " minutes\n";

    const std::vector<UndirectedWeightedEdge> infrastructure{
        {0, 1, 8}, {0, 2, 5}, {1, 2, 3}, {1, 3, 6}, {2, 3, 4}
    };
    const auto prim = AirportOperations::planInfrastructureWithPrim(4, infrastructure);
    const auto kruskal =
        AirportOperations::planInfrastructureWithKruskal(4, infrastructure);
    std::cout << "Airport infrastructure plan cost (Prim/Kruskal): "
              << prim.totalWeight << '/' << kruskal.totalWeight << '\n';

    const auto logCodec = AirportOperations::eventLogCodec(
        "P001 arrived; P001 screened; P002 arrived;"
    );
    const std::string encoded = logCodec.encode("P001 arrived; P001 screened;");
    std::cout << "Compressed event-log round trip: "
              << logCodec.decode(encoded) << '\n';
    return 0;
}
