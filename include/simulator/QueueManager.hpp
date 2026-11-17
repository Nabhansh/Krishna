#pragma once

#include "core/SecurityLane.hpp"
#include "simulator/QueueWorkloadIndex.hpp"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace project31 {

struct LaneQueueStatus {
    std::string laneId;
    std::string officerName;
    std::size_t queuedPassengers;
    std::int64_t workloadMinutes;
};

struct QueueAssignment {
    std::string laneId;
    std::int64_t estimatedWaitMinutes;
};

class QueueManager {
public:
    void addLane(SecurityLane lane);

    [[nodiscard]] QueueAssignment enqueue(Passenger passenger);
    [[nodiscard]] std::optional<Passenger> screenNext(const std::string& laneId);
    [[nodiscard]] std::int64_t estimatedWaitMinutes(const std::string& laneId) const;
    [[nodiscard]] std::vector<LaneQueueStatus> laneStatuses() const;
    [[nodiscard]] std::size_t laneCount() const noexcept;

private:
    [[nodiscard]] SecurityLane& laneById(const std::string& laneId);
    [[nodiscard]] const SecurityLane& laneById(const std::string& laneId) const;

    std::vector<SecurityLane> lanes_;
    QueueWorkloadIndex workloadIndex_;
};

} // namespace project31
