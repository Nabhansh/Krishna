#include "simulator/QueueWorkloadIndex.hpp"

#include <limits>
#include <stdexcept>

namespace project31 {

void QueueWorkloadIndex::addLane(const std::string& laneId) {
    const auto [iterator, inserted] = workloads_.emplace(laneId, 0);
    (void)iterator;
    if (!inserted) {
        throw std::invalid_argument("Security lane IDs must be unique.");
    }
}

void QueueWorkloadIndex::addPassenger(
    const std::string& laneId,
    int screeningDurationMinutes
) {
    if (screeningDurationMinutes <= 0) {
        throw std::invalid_argument("Screening duration must be greater than zero.");
    }
    auto found = workloads_.find(laneId);
    if (found == workloads_.end()) {
        throw std::out_of_range("Unknown security lane: " + laneId);
    }
    const std::int64_t duration = screeningDurationMinutes;
    if (found->second > std::numeric_limits<std::int64_t>::max() - duration) {
        throw std::overflow_error("Security lane workload exceeds the supported range.");
    }
    found->second += duration;
}

void QueueWorkloadIndex::removePassenger(
    const std::string& laneId,
    int screeningDurationMinutes
) {
    auto found = workloads_.find(laneId);
    if (found == workloads_.end()) {
        throw std::out_of_range("Unknown security lane: " + laneId);
    }
    if (screeningDurationMinutes <= 0 || found->second < screeningDurationMinutes) {
        throw std::logic_error("Cannot remove screening work absent from the queue index.");
    }
    found->second -= screeningDurationMinutes;
}

std::int64_t QueueWorkloadIndex::workloadMinutes(const std::string& laneId) const {
    const auto found = workloads_.find(laneId);
    if (found == workloads_.end()) {
        throw std::out_of_range("Unknown security lane: " + laneId);
    }
    return found->second;
}

} // namespace project31
