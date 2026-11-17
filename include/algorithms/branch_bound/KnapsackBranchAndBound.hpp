#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace project31::algorithms {

struct BranchBoundItem {
    std::string identifier;
    std::int64_t weight;
    std::int64_t value;
};

struct BranchBoundKnapsackResult {
    std::int64_t maximumValue;
    std::vector<std::size_t> selectedIndices;
};

} // namespace project31::algorithms
