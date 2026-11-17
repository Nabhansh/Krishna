#pragma once

#include "algorithms/branch_bound/KnapsackBranchAndBound.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace project31::algorithms::branch_bound_detail {

struct Problem {
    const std::vector<BranchBoundItem>& items;
    std::int64_t capacity;
    std::vector<std::size_t> order;
};

struct Node {
    std::size_t level;
    std::int64_t weight;
    std::int64_t value;
    std::vector<std::size_t> selectedIndices;
};

inline int compareFractions(
    std::uint64_t leftNumerator,
    std::uint64_t leftDenominator,
    std::uint64_t rightNumerator,
    std::uint64_t rightDenominator
) {
    bool reversed = false;
    while (true) {
        const std::uint64_t leftWhole = leftNumerator / leftDenominator;
        const std::uint64_t rightWhole = rightNumerator / rightDenominator;
        if (leftWhole != rightWhole) {
            const int comparison = leftWhole < rightWhole ? -1 : 1;
            return reversed ? -comparison : comparison;
        }

        const std::uint64_t leftRemainder = leftNumerator % leftDenominator;
        const std::uint64_t rightRemainder = rightNumerator % rightDenominator;
        if (leftRemainder == 0 || rightRemainder == 0) {
            const int comparison = leftRemainder == rightRemainder
                ? 0
                : (leftRemainder == 0 ? -1 : 1);
            return reversed ? -comparison : comparison;
        }

        leftNumerator = leftDenominator;
        leftDenominator = leftRemainder;
        rightNumerator = rightDenominator;
        rightDenominator = rightRemainder;
        reversed = !reversed;
    }
}

inline Problem prepareProblem(
    const std::vector<BranchBoundItem>& items,
    std::int64_t capacity
) {
    if (capacity < 0) {
        throw std::invalid_argument("Branch-and-bound capacity must not be negative.");
    }
    std::vector<std::size_t> order;
    order.reserve(items.size());
    for (std::size_t index = 0; index < items.size(); ++index) {
        const BranchBoundItem& item = items[index];
        if (item.identifier.find_first_not_of(" \t\n\r\f\v") == std::string::npos) {
            throw std::invalid_argument("Branch-and-bound item identifiers must not be blank.");
        }
        if (item.weight <= 0) {
            throw std::invalid_argument("Branch-and-bound item weights must be positive.");
        }
        if (item.value < 0) {
            throw std::invalid_argument("Branch-and-bound item values must not be negative.");
        }
        for (std::size_t prior = 0; prior < index; ++prior) {
            if (items[prior].identifier == item.identifier) {
                throw std::invalid_argument("Branch-and-bound item identifiers must be unique.");
            }
        }
        order.push_back(index);
    }

    const auto hasHigherPriority = [&items](std::size_t left, std::size_t right) {
        const BranchBoundItem& leftItem = items[left];
        const BranchBoundItem& rightItem = items[right];
        const int ratioOrder = compareFractions(
            static_cast<std::uint64_t>(leftItem.value),
            static_cast<std::uint64_t>(leftItem.weight),
            static_cast<std::uint64_t>(rightItem.value),
            static_cast<std::uint64_t>(rightItem.weight)
        );
        if (ratioOrder != 0) {
            return ratioOrder > 0;
        }
        if (leftItem.identifier != rightItem.identifier) {
            return leftItem.identifier < rightItem.identifier;
        }
        return left < right;
    };

    for (std::size_t index = 1; index < order.size(); ++index) {
        const std::size_t item = order[index];
        std::size_t position = index;
        while (position > 0 && hasHigherPriority(item, order[position - 1])) {
            order[position] = order[position - 1];
            --position;
        }
        order[position] = item;
    }
    return {items, capacity, std::move(order)};
}

inline long double upperBound(const Problem& problem, const Node& node) {
    long double bound = node.value == 0
        ? 0.0L
        : std::nextafter(
            static_cast<long double>(node.value),
            std::numeric_limits<long double>::infinity()
        );
    std::int64_t remaining = problem.capacity - node.weight;
    for (std::size_t level = node.level; level < problem.order.size(); ++level) {
        const BranchBoundItem& item = problem.items[problem.order[level]];
        if (item.weight <= remaining) {
            if (item.value > 0) {
                bound = std::nextafter(
                    bound + std::nextafter(
                        static_cast<long double>(item.value),
                        std::numeric_limits<long double>::infinity()
                    ),
                    std::numeric_limits<long double>::infinity()
                );
            }
            remaining -= item.weight;
        } else {
            if (remaining > 0 && item.value > 0) {
                const long double ratio = std::nextafter(
                    std::nextafter(
                        static_cast<long double>(item.value),
                        std::numeric_limits<long double>::infinity()
                    ) /
                        std::nextafter(
                            static_cast<long double>(item.weight),
                            0.0L
                        ),
                    std::numeric_limits<long double>::infinity()
                );
                const long double fractionalValue = std::nextafter(
                    std::nextafter(
                        static_cast<long double>(remaining),
                        std::numeric_limits<long double>::infinity()
                    ) * ratio,
                    std::numeric_limits<long double>::infinity()
                );
                bound = std::nextafter(
                    bound + fractionalValue,
                    std::numeric_limits<long double>::infinity()
                );
            }
            break;
        }
    }
    return bound;
}

inline void consider(
    const Node& node,
    std::int64_t& bestValue,
    std::vector<std::size_t>& bestIndices
) {
    if (node.value > bestValue) {
        bestValue = node.value;
        bestIndices = node.selectedIndices;
    }
}

inline Node excludedChild(const Node& parent) {
    Node child = parent;
    ++child.level;
    return child;
}

inline Node includedChild(
    const Problem& problem,
    const Node& parent
) {
    const std::size_t itemIndex = problem.order[parent.level];
    const BranchBoundItem& item = problem.items[itemIndex];
    if (item.weight > problem.capacity - parent.weight) {
        throw std::logic_error("Cannot include a branch-and-bound item over capacity.");
    }
    if (parent.value > std::numeric_limits<std::int64_t>::max() - item.value) {
        throw std::overflow_error("Branch-and-bound value exceeds int64 range.");
    }
    Node child = parent;
    ++child.level;
    child.weight += item.weight;
    child.value += item.value;
    auto insertion = child.selectedIndices.end();
    while (insertion != child.selectedIndices.begin() &&
           itemIndex < *(insertion - 1)) {
        --insertion;
    }
    child.selectedIndices.insert(insertion, itemIndex);
    return child;
}

inline bool canImprove(const Problem& problem, const Node& node, std::int64_t bestValue) {
    return upperBound(problem, node) > static_cast<long double>(bestValue);
}

} // namespace project31::algorithms::branch_bound_detail
