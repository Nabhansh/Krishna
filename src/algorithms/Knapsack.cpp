#include "algorithms/dynamic_programming/Knapsack.hpp"

#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace project31::algorithms {

KnapsackResult solveKnapsack01(
    const std::vector<std::int64_t>& weights,
    const std::vector<std::int64_t>& values,
    std::int64_t capacity
) {
    if (capacity < 0) {
        throw std::invalid_argument("Knapsack capacity must not be negative.");
    }
    if (weights.size() != values.size()) {
        throw std::invalid_argument("Knapsack weight and value counts must match.");
    }
    for (std::size_t index = 0; index < weights.size(); ++index) {
        if (weights[index] < 0 || values[index] < 0) {
            throw std::invalid_argument("Knapsack weights and values must not be negative.");
        }
    }
    if (static_cast<std::uint64_t>(capacity) >
        static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max() - 1)) {
        throw std::length_error("Knapsack capacity is too large for a DP table.");
    }

    const std::size_t columns = static_cast<std::size_t>(capacity) + 1;
    const std::size_t rows = weights.size() + 1;
    if (columns != 0 &&
        rows > std::numeric_limits<std::size_t>::max() / columns) {
        throw std::length_error("Knapsack DP table dimensions overflow.");
    }
    if (columns > std::vector<std::int64_t>().max_size() ||
        rows > std::vector<std::vector<std::int64_t>>().max_size()) {
        throw std::length_error("Knapsack DP table exceeds vector capacity.");
    }

    // dp[i][c] is the best value using only the first i items at capacity c.
    std::vector<std::vector<std::int64_t>> dp(
        rows,
        std::vector<std::int64_t>(columns, 0)
    );
    for (std::size_t item = 1; item < rows; ++item) {
        const std::uint64_t itemWeight = static_cast<std::uint64_t>(weights[item - 1]);
        const std::int64_t value = values[item - 1];
        for (std::size_t currentCapacity = 0; currentCapacity < columns; ++currentCapacity) {
            dp[item][currentCapacity] = dp[item - 1][currentCapacity];
            if (itemWeight <= currentCapacity) {
                const std::size_t weight = static_cast<std::size_t>(itemWeight);
                const std::int64_t previous = dp[item - 1][currentCapacity - weight];
                if (previous > std::numeric_limits<std::int64_t>::max() - value) {
                    throw std::overflow_error("Knapsack value exceeds int64 range.");
                }
                const std::int64_t withItem = previous + value;
                if (withItem > dp[item][currentCapacity]) {
                    dp[item][currentCapacity] = withItem;
                }
            }
        }
    }

    std::vector<std::size_t> selected;
    std::size_t remainingCapacity = columns - 1;
    for (std::size_t item = weights.size(); item > 0; --item) {
        // A changed table value means item-1 was chosen; the previous row
        // guarantees that this item is used at most once.
        if (dp[item][remainingCapacity] != dp[item - 1][remainingCapacity]) {
            selected.push_back(item - 1);
            remainingCapacity -= static_cast<std::size_t>(
                static_cast<std::uint64_t>(weights[item - 1])
            );
        }
    }
    for (std::size_t left = 0, right = selected.size(); left < right / 2; ++left) {
        std::swap(selected[left], selected[right - left - 1]);
    }
    return {dp.back().back(), std::move(selected)};
}

} // namespace project31::algorithms
