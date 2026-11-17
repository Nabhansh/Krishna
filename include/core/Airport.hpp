#pragma once

#include "core/Passenger.hpp"
#include "core/PassengerIndex.hpp"
#include "simulator/QueueManager.hpp"
#include "simulator/SimulationEvent.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace project31 {

struct ArrivalResult {
    std::string laneId;
    std::int64_t estimatedWaitMinutes;
};

class Airport {
public:
    explicit Airport(std::string name);

    [[nodiscard]] const std::string& name() const noexcept;
    [[nodiscard]] QueueManager& queueManager() noexcept;
    [[nodiscard]] const QueueManager& queueManager() const noexcept;
    [[nodiscard]] const std::vector<SimulationEvent>& events() const noexcept;
    [[nodiscard]] const Passenger* findPassenger(const std::string& passengerId) const noexcept;
    [[nodiscard]] std::vector<const Passenger*> passengersByPriority(int priority) const;
    [[nodiscard]] std::vector<std::string> passengerIdsSortedBy(
        PassengerIdSort algorithm
    ) const;

    void registerPassenger(Passenger passenger, int timestampMinutes);
    [[nodiscard]] ArrivalResult passengerArrives(
        const std::string& passengerId,
        int timestampMinutes
    );
    [[nodiscard]] bool screenNext(
        const std::string& laneId,
        int timestampMinutes
    );

private:
    std::string name_;
    QueueManager queueManager_;
    std::unordered_map<std::string, Passenger> passengers_;
    PassengerIndex passengerIndex_;
    std::vector<SimulationEvent> events_;
};

} // namespace project31
