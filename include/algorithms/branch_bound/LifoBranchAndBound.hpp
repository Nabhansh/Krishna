#pragma once

#include "algorithms/branch_bound/KnapsackBranchAndBound.hpp"

#include <cstdint>
#include <vector>

namespace project31::algorithms {

[[nodiscard]] BranchBoundKnapsackResult solveKnapsackLifoBranchAndBound(
    const std::vector<BranchBoundItem>& items,
    std::int64_t capacity
);

} // namespace project31::algorithms
