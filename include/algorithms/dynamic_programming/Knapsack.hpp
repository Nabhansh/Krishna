#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace project31::algorithms {

struct KnapsackResult {
    std::int64_t maximumValue;
    std::vector<std::size_t> selectedIndices;
};

[[nodiscard]] KnapsackResult solveKnapsack01(
    const std::vector<std::int64_t>& weights,
    const std::vector<std::int64_t>& values,
    std::int64_t capacity
);

} // namespace project31::algorithms
