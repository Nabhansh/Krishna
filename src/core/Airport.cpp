#include "core/Airport.hpp"

#include "algorithms/searching/BinarySearch.hpp"

#include <stdexcept>
#include <utility>

namespace project31 {

Airport::Airport(std::string name) : name_(std::move(name)) {
    if (name_.find_first_not_of(" \t\n\r\f\v") == std::string::npos) {
        throw std::invalid_argument("Airport name must not be empty.");
    }
}

const std::string& Airport::name() const noexcept {
    return name_;
}

QueueManager& Airport::queueManager() noexcept {
    return queueManager_;
}

const QueueManager& Airport::queueManager() const noexcept {
    return queueManager_;
}

const std::vector<SimulationEvent>& Airport::events() const noexcept {
    return events_;
}

const Passenger* Airport::findPassenger(const std::string& passengerId) const noexcept {
    if (!algorithms::iterativeBinarySearch(
            passengerIndex_.passengerIdsSorted(),
            passengerId
        )) {
        return nullptr;
    }
    const auto found = passengers_.find(passengerId);
    return found == passengers_.end() ? nullptr : &found->second;
}

std::vector<const Passenger*> Airport::passengersByPriority(int priority) const {
    const std::vector<std::string> ids = passengerIndex_.idsForPriority(priority);
    std::vector<const Passenger*> passengers;
    passengers.reserve(ids.size());
    for (const std::string& id : ids) {
        const auto found = passengers_.find(id);
        if (found == passengers_.end()) {
            throw std::logic_error("Passenger priority index contains an unknown ID.");
        }
        passengers.push_back(&found->second);
    }
    return passengers;
}

std::vector<std::string> Airport::passengerIdsSortedBy(PassengerIdSort algorithm) const {
    return passengerIndex_.passengerIdsSortedWith(algorithm);
}

void Airport::registerPassenger(Passenger passenger, int timestampMinutes) {
    if (timestampMinutes < 0) {
        throw std::invalid_argument("Event time must not be negative.");
    }
    const std::vector<std::string> errors = passenger.validationErrors();
    if (!errors.empty()) {
        throw std::invalid_argument(errors.front());
    }
    const std::string passengerId = passenger.id();
    const auto [iterator, inserted] = passengers_.emplace(passengerId, std::move(passenger));
    if (!inserted) {
        throw std::invalid_argument("Passenger ID is already registered: " + passengerId);
    }
    passengerIndex_.add(iterator->second);
    events_.push_back({
        SimulationEventType::PassengerRegistered,
        timestampMinutes,
        iterator->first,
        "Passenger registered."
    });
}

ArrivalResult Airport::passengerArrives(
    const std::string& passengerId,
    int timestampMinutes
) {
    if (timestampMinutes < 0) {
        throw std::invalid_argument("Event time must not be negative.");
    }
    const auto found = passengers_.find(passengerId);
    if (found == passengers_.end()) {
        throw std::out_of_range("Passenger is not registered: " + passengerId);
    }
    if (found->second.status() != PassengerStatus::Registered) {
        throw std::logic_error("Passenger has already arrived or completed screening.");
    }

    const QueueAssignment assignment = queueManager_.enqueue(found->second);
    found->second.markQueued();
    events_.push_back({
        SimulationEventType::PassengerArrived,
        timestampMinutes,
        passengerId,
        "Passenger joined security lane " + assignment.laneId + "."
    });
    return {assignment.laneId, assignment.estimatedWaitMinutes};
}

bool Airport::screenNext(const std::string& laneId, int timestampMinutes) {
    if (timestampMinutes < 0) {
        throw std::invalid_argument("Event time must not be negative.");
    }
    const std::optional<Passenger> passenger = queueManager_.screenNext(laneId);
    if (!passenger) {
        return false;
    }

    const auto found = passengers_.find(passenger->id());
    if (found == passengers_.end()) {
        throw std::logic_error("Security queue contains an unregistered passenger.");
    }
    found->second.markScreened();
    events_.push_back({
        SimulationEventType::PassengerScreened,
        timestampMinutes,
        passenger->id(),
        "Passenger screened in security lane " + laneId + "."
    });
    return true;
}

} // namespace project31
