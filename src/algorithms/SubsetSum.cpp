#include "algorithms/backtracking/SubsetSum.hpp"

#include <stdexcept>
#include <vector>

namespace project31::algorithms {
namespace {

bool searchSubset(
    const std::vector<std::int64_t>& values,
    std::size_t index,
    std::int64_t remaining,
    std::vector<std::size_t>& selected
) {
    if (remaining == 0) {
        return true;
    }
    if (index == values.size()) {
        return false;
    }

    if (values[index] <= remaining) {
        selected.push_back(index);
        if (searchSubset(values, index + 1, remaining - values[index], selected)) {
            return true;
        }
        selected.pop_back();
    }
    return searchSubset(values, index + 1, remaining, selected);
}

} // namespace

std::optional<std::vector<std::size_t>> findSubsetSum(
    const std::vector<std::int64_t>& values,
    std::int64_t target
) {
    if (target < 0) {
        throw std::invalid_argument("Subset Sum target must not be negative.");
    }
    for (std::int64_t value : values) {
        if (value < 0) {
            throw std::invalid_argument("Subset Sum values must not be negative.");
        }
    }
    if (target == 0) {
        return std::vector<std::size_t>{};
    }

    // Cap the reachable total at target so adding large inputs cannot overflow.
    std::int64_t cappedTotal = 0;
    for (std::int64_t value : values) {
        if (value >= target - cappedTotal) {
            cappedTotal = target;
            break;
        }
        cappedTotal += value;
    }
    if (cappedTotal < target) {
        return std::nullopt;
    }

    std::vector<std::size_t> selected;
    if (searchSubset(values, 0, target, selected)) {
        return selected;
    }
    return std::nullopt;
}

} // namespace project31::algorithms
