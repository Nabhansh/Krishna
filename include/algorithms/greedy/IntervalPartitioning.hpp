#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace project31::algorithms {

struct ScreeningInterval {
    std::string id;
    std::int64_t start;
    std::int64_t finish;
};

struct IntervalAssignment {
    std::string intervalId;
    std::size_t resource;
};

struct IntervalPartition {
    std::size_t resourceCount;
    std::vector<IntervalAssignment> assignments;
};

[[nodiscard]] IntervalPartition partitionIntervals(
    std::vector<ScreeningInterval> intervals
);

} // namespace project31::algorithms
