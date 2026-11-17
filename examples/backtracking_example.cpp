#include "simulator/AirportOperations.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

int main() {
    const auto assignments = project31::AirportOperations::assignScreeningPositions(4);
    std::cout << "Valid non-attacking screening position assignments: "
              << assignments.size() << '\n';
    for (const auto& assignment : assignments) {
        for (std::size_t row = 0; row < assignment.size(); ++row) {
            std::cout << "row " << row << " -> position " << assignment[row] << '\n';
        }
        std::cout << "--\n";
    }

    const std::vector<std::int64_t> packageCapacities{5, 7, 4, 3};
    const auto selected =
        project31::AirportOperations::selectResourcePackages(packageCapacities, 10);
    if (!selected) {
        std::cout << "No package combination meets the capacity target.\n";
        return 0;
    }
    std::cout << "Resource packages reaching capacity 10:";
    for (std::size_t index : *selected) {
        std::cout << ' ' << index;
    }
    std::cout << '\n';
    return 0;
}
