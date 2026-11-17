#include "simulator/AirportOperations.hpp"

#include <iostream>
#include <vector>

int main() {
    const std::vector<project31::algorithms::CapacityEdge> connections{
        {0, 1, 8}, {0, 2, 5}, {1, 2, 3}, {1, 3, 5}, {2, 3, 7}
    };
    const auto result =
        project31::AirportOperations::planCheckpointFlow(4, connections, 0, 3);

    std::cout << "Maximum checkpoint throughput: " << result.maximumFlow << '\n';
    std::cout << "Connection flows:\n";
    for (const auto& edge : result.edgeFlows) {
        std::cout << "  " << edge.from << " -> " << edge.to << ": "
                  << edge.flow << '/' << edge.capacity << '\n';
    }
    return 0;
}
