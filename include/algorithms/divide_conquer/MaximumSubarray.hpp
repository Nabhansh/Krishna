#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace project31::algorithms {

struct MaximumSubarrayResult {
    std::int64_t sum;
    std::size_t begin;
    std::size_t end;
};

[[nodiscard]] std::optional<MaximumSubarrayResult> maximumSubarray(
    const std::vector<std::int64_t>& values
);

} // namespace project31::algorithms
