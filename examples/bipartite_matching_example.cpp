#include "simulator/AirportOperations.hpp"

#include <iostream>
#include <vector>

int main() {
    const std::vector<project31::algorithms::BipartiteEdge> allowedAssignments{
        {0, 0}, {0, 1}, {1, 0}, {2, 1}, {2, 2}
    };
    const auto result = project31::AirportOperations::assignOfficersToScreeningLanes(
        3,
        3,
        allowedAssignments
    );

    std::cout << "Officer-to-lane assignments (" << result.cardinality << "):\n";
    for (const auto& [officer, lane] : result.matchedPairs) {
        std::cout << "  Officer " << officer << " -> Lane " << lane << '\n';
    }
    return 0;
}
