#include "algorithms/divide_conquer/MaximumSubarray.hpp"

#include <limits>
#include <stdexcept>

namespace project31::algorithms {
namespace {

std::int64_t checkedAdd(std::int64_t left, std::int64_t right) {
    if ((right > 0 && left > std::numeric_limits<std::int64_t>::max() - right) ||
        (right < 0 && left < std::numeric_limits<std::int64_t>::min() - right)) {
        throw std::overflow_error("Maximum-subarray sum exceeds the supported range.");
    }
    return left + right;
}

bool isBetter(const MaximumSubarrayResult& candidate, const MaximumSubarrayResult& best) {
    if (candidate.sum != best.sum) {
        return candidate.sum > best.sum;
    }
    if (candidate.begin != best.begin) {
        return candidate.begin < best.begin;
    }
    return candidate.end < best.end;
}

MaximumSubarrayResult solve(
    const std::vector<std::int64_t>& values,
    std::size_t begin,
    std::size_t end
) {
    if (end - begin == 1) {
        return {values[begin], begin, end};
    }

    const std::size_t middle = begin + (end - begin) / 2;
    const MaximumSubarrayResult left = solve(values, begin, middle);
    const MaximumSubarrayResult right = solve(values, middle, end);

    std::int64_t suffixSum = std::numeric_limits<std::int64_t>::min();
    std::int64_t running = 0;
    std::size_t suffixBegin = middle;
    for (std::size_t index = middle; index > begin; --index) {
        running = checkedAdd(running, values[index - 1]);
        if (running > suffixSum) {
            suffixSum = running;
            suffixBegin = index - 1;
        }
    }

    std::int64_t prefixSum = std::numeric_limits<std::int64_t>::min();
    running = 0;
    std::size_t prefixEnd = middle;
    for (std::size_t index = middle; index < end; ++index) {
        running = checkedAdd(running, values[index]);
        if (running > prefixSum) {
            prefixSum = running;
            prefixEnd = index + 1;
        }
    }
    const MaximumSubarrayResult crossing{
        checkedAdd(suffixSum, prefixSum),
        suffixBegin,
        prefixEnd
    };

    MaximumSubarrayResult best = left;
    if (isBetter(right, best)) {
        best = right;
    }
    if (isBetter(crossing, best)) {
        best = crossing;
    }
    return best;
}

} // namespace

std::optional<MaximumSubarrayResult> maximumSubarray(
    const std::vector<std::int64_t>& values
) {
    if (values.empty()) {
        return std::nullopt;
    }
    return solve(values, 0, values.size());
}

} // namespace project31::algorithms
