#pragma once

#include <string>

namespace project31 {

enum class SimulationEventType {
    PassengerRegistered,
    PassengerArrived,
    PassengerScreened
};

struct SimulationEvent {
    SimulationEventType type;
    int timestampMinutes;
    std::string passengerId;
    std::string description;
};

} // namespace project31
