#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace project31 {

class QueueWorkloadIndex {
public:
    void addLane(const std::string& laneId);
    void addPassenger(const std::string& laneId, int screeningDurationMinutes);
    void removePassenger(const std::string& laneId, int screeningDurationMinutes);

    [[nodiscard]] std::int64_t workloadMinutes(const std::string& laneId) const;

private:
    std::unordered_map<std::string, std::int64_t> workloads_;
};

} // namespace project31
