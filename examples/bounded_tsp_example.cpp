#include "simulator/AirportOperations.hpp"

#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>

int main() {
    const std::vector<std::vector<std::optional<std::int64_t>>> travelCosts{
        {std::nullopt, 10, 15, 20},
        {5, std::nullopt, 9, 10},
        {6, 13, std::nullopt, 12},
        {8, 8, 9, std::nullopt}
    };
    const auto tour = project31::AirportOperations::planAirportInspectionTour(
        travelCosts
    );
    if (!tour) {
        std::cout << "No round-trip inspection route visits every location.\n";
        return 0;
    }
    std::cout << "Minimum inspection tour cost: " << tour->totalCost << "\nRoute:";
    for (std::size_t location : tour->tour) {
        std::cout << ' ' << location;
    }
    std::cout << '\n';
    return 0;
}
