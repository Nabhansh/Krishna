#include "simulator/AirportOperations.hpp"

#include <iostream>
#include <vector>

int main() {
    const std::vector<project31::algorithms::BranchBoundItem> equipment{
        {"scanner-upgrade", 4, 8},
        {"trace-detector", 3, 6},
        {"lane-sensor", 5, 10},
        {"radio-set", 2, 4},
        {"thermal-camera", 6, 9}
    };
    const auto fifo =
        project31::AirportOperations::selectEquipmentFifoBranchAndBound(equipment, 10);
    const auto lifo =
        project31::AirportOperations::selectEquipmentLifoBranchAndBound(equipment, 10);
    const auto leastCost =
        project31::AirportOperations::selectEquipmentLeastCostBranchAndBound(
            equipment, 10
        );

    std::cout << "Screening equipment allocation (weight limit 10)\n";
    std::cout << "FIFO value: " << fifo.maximumValue << '\n';
    std::cout << "LIFO value: " << lifo.maximumValue << '\n';
    std::cout << "Least-cost best-first value: " << leastCost.maximumValue << '\n'
              << "Selected package identifiers:";
    for (std::size_t index : leastCost.selectedIndices) {
        std::cout << ' ' << equipment[index].identifier;
    }
    std::cout << '\n';
    return 0;
}
