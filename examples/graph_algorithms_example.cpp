#include "simulator/AirportOperations.hpp"

#include <iostream>
#include <vector>

namespace {

void printPath(const std::vector<std::size_t>& path) {
    for (std::size_t index = 0; index < path.size(); ++index) {
        if (index != 0) {
            std::cout << " -> ";
        }
        std::cout << path[index];
    }
    std::cout << '\n';
}

} // namespace

int main() {
    using project31::algorithms::DirectedWeightedEdge;
    const std::vector<DirectedWeightedEdge> facilityRoutes{
        {0, 1, 6}, {0, 2, 2}, {2, 1, -1}, {1, 3, 3}, {2, 3, 7}, {3, 4, 1}
    };

    const auto singleSource =
        project31::AirportOperations::planAirportRoutesWithBellmanFord(
            5, facilityRoutes, 0
        );
    const auto allPairs =
        project31::AirportOperations::planAllAirportRoutesWithFloydWarshall(
            5, facilityRoutes
        );
    const auto route = singleSource.pathTo(4);
    std::cout << "Bellman-Ford terminal-to-service cost: "
              << *singleSource.distances[4] << "\nRoute: ";
    printPath(*route);
    std::cout << "Floyd-Warshall all-pairs cost: "
              << *allPairs.distances[0][4] << '\n';
    return 0;
}
