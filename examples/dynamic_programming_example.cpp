#include "algorithms/dynamic_programming/Knapsack.hpp"
#include "algorithms/dynamic_programming/MatrixChainMultiplication.hpp"
#include "simulator/AirportOperations.hpp"

#include <iostream>

int main() {
    const auto upgrades = project31::AirportOperations::optimizeAirportUpgrades(
        {4, 3, 5, 2},
        {8, 6, 10, 4},
        8
    );
    std::cout << "Best airport upgrade value: " << upgrades.maximumValue << '\n'
              << "Selected upgrade item indices:";
    for (std::size_t index : upgrades.selectedIndices) {
        std::cout << ' ' << index;
    }
    std::cout << '\n';

    const auto analytics =
        project31::AirportOperations::planAnalyticsMultiplication({30, 35, 15, 5, 10, 20, 25});
    std::cout << "Analytics matrix chain cost: " << analytics.minimumCost << '\n'
              << "Order: " << analytics.parenthesization << '\n';
    return 0;
}
