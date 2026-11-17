#include "core/SecurityLane.hpp"

#include <numeric>
#include <stdexcept>
#include <utility>

namespace project31 {

SecurityLane::SecurityLane(std::string id, SecurityOfficer officer)
    : id_(std::move(id)), officer_(std::move(officer)) {
    if (id_.find_first_not_of(" \t\n\r\f\v") == std::string::npos) {
        throw std::invalid_argument("Security lane ID must not be empty.");
    }
}

const std::string& SecurityLane::id() const noexcept {
    return id_;
}

const SecurityOfficer& SecurityLane::officer() const noexcept {
    return officer_;
}

const std::deque<Passenger>& SecurityLane::queue() const noexcept {
    return queue_;
}

std::int64_t SecurityLane::workloadMinutes() const noexcept {
    return std::accumulate(
        queue_.begin(),
        queue_.end(),
        std::int64_t{0},
        [](std::int64_t total, const Passenger& passenger) {
            return total + passenger.screeningDurationMinutes();
        }
    );
}

void SecurityLane::enqueue(Passenger passenger) {
    queue_.push_back(std::move(passenger));
}

std::optional<Passenger> SecurityLane::screenNext() {
    if (queue_.empty()) {
        return std::nullopt;
    }
    Passenger passenger = std::move(queue_.front());
    queue_.pop_front();
    return passenger;
}

} // namespace project31
