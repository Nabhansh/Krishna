#include "simulator/AirportOperations.hpp"

#include <iostream>

int main() {
    const auto schedule =
        project31::AirportOperations::reconcileMaintenanceSchedule({3, 5}, {7, 9});

    std::cout << "Inspection and calibration cycles next align at day "
              << schedule.solution << " (combined cycle "
              << schedule.combinedModulus << " days).\n";
    return 0;
}
