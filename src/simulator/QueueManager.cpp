#include "simulator/QueueManager.hpp"

#include <limits>
#include <stdexcept>
#include <utility>

namespace project31 {

void QueueManager::addLane(SecurityLane lane) {
    for (const SecurityLane& existing : lanes_) {
        if (existing.id() == lane.id()) {
            throw std::invalid_argument("Security lane IDs must be unique.");
        }
    }
    workloadIndex_.addLane(lane.id());
    lanes_.push_back(std::move(lane));
}

QueueAssignment QueueManager::enqueue(Passenger passenger) {
    if (lanes_.empty()) {
        throw std::logic_error("Cannot route a passenger because no security lanes exist.");
    }

    SecurityLane* selected = &lanes_.front();
    std::int64_t wait = workloadIndex_.workloadMinutes(selected->id());
    for (SecurityLane& lane : lanes_) {
        const std::int64_t workload = workloadIndex_.workloadMinutes(lane.id());
        if (workload < wait) {
            selected = &lane;
            wait = workload;
        }
    }
    const std::int64_t duration = passenger.screeningDurationMinutes();
    if (wait > std::numeric_limits<std::int64_t>::max() - duration) {
        throw std::overflow_error("Security lane workload exceeds the supported range.");
    }
    const std::string laneId = selected->id();
    passenger.markQueued();
    selected->enqueue(std::move(passenger));
    workloadIndex_.addPassenger(laneId, selected->queue().back().screeningDurationMinutes());
    return {laneId, wait};
}

std::optional<Passenger> QueueManager::screenNext(const std::string& laneId) {
    std::optional<Passenger> passenger = laneById(laneId).screenNext();
    if (passenger) {
        workloadIndex_.removePassenger(laneId, passenger->screeningDurationMinutes());
    }
    return passenger;
}

std::int64_t QueueManager::estimatedWaitMinutes(const std::string& laneId) const {
    (void)laneById(laneId);
    return workloadIndex_.workloadMinutes(laneId);
}

std::vector<LaneQueueStatus> QueueManager::laneStatuses() const {
    std::vector<LaneQueueStatus> statuses;
    statuses.reserve(lanes_.size());
    for (const SecurityLane& lane : lanes_) {
        statuses.push_back({
            lane.id(),
            lane.officer().name(),
            lane.queue().size(),
            workloadIndex_.workloadMinutes(lane.id())
        });
    }
    return statuses;
}

std::size_t QueueManager::laneCount() const noexcept {
    return lanes_.size();
}

SecurityLane& QueueManager::laneById(const std::string& laneId) {
    for (SecurityLane& lane : lanes_) {
        if (lane.id() == laneId) {
            return lane;
        }
    }
    throw std::out_of_range("Unknown security lane: " + laneId);
}

const SecurityLane& QueueManager::laneById(const std::string& laneId) const {
    for (const SecurityLane& lane : lanes_) {
        if (lane.id() == laneId) {
            return lane;
        }
    }
    throw std::out_of_range("Unknown security lane: " + laneId);
}

} // namespace project31
