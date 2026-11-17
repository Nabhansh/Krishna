#include "simulator/AirportOperations.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

namespace {

void printSelection(
    const char* strategy,
    const project31::algorithms::BranchBoundKnapsackResult& result,
    const std::vector<project31::algorithms::BranchBoundItem>& equipment
) {
    std::cout << strategy << " value: " << result.maximumValue << '\n'
              << "Selected screening equipment:";
    for (std::size_t index : result.selectedIndices) {
        std::cout << ' ' << equipment[index].identifier;
    }
    std::cout << '\n';
}

} // namespace

int main() {
    const std::vector<project31::algorithms::BranchBoundItem> equipment{
        {"scanner-upgrade", 4, 8},
        {"trace-detector", 3, 6},
        {"lane-sensor", 5, 10},
        {"radio-set", 2, 4},
        {"thermal-camera", 6, 9}
    };
    constexpr std::int64_t weightLimit = 10;
    const auto fifo = project31::AirportOperations::selectEquipmentFifoBranchAndBound(
        equipment, weightLimit
    );
    const auto lifo = project31::AirportOperations::selectEquipmentLifoBranchAndBound(
        equipment, weightLimit
    );
    std::cout << "Screening equipment allocation at weight limit "
              << weightLimit << '\n';
    printSelection("FIFO branch-and-bound", fifo, equipment);
    printSelection("LIFO branch-and-bound", lifo, equipment);
    return 0;
}
