#include "algorithms/dynamic_programming/MatrixChainMultiplication.hpp"

#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace project31::algorithms {
namespace {

std::optional<std::int64_t> checkedMultiply(std::int64_t left, std::int64_t right) {
    if (left < 0 || right < 0) {
        throw std::logic_error("Matrix-chain dimensions must already be validated.");
    }
    if (right != 0 && left > std::numeric_limits<std::int64_t>::max() / right) {
        return std::nullopt;
    }
    return left * right;
}

std::optional<std::int64_t> checkedCost(
    std::int64_t leftCost,
    std::int64_t rightCost,
    std::int64_t rows,
    std::int64_t inner,
    std::int64_t columns
) {
    const auto matrixProduct = checkedMultiply(rows, inner);
    if (!matrixProduct) {
        return std::nullopt;
    }
    const auto scalarCount = checkedMultiply(*matrixProduct, columns);
    if (!scalarCount ||
        leftCost > std::numeric_limits<std::int64_t>::max() - rightCost) {
        return std::nullopt;
    }
    const std::int64_t subproblemCost = leftCost + rightCost;
    if (subproblemCost > std::numeric_limits<std::int64_t>::max() - *scalarCount) {
        return std::nullopt;
    }
    return subproblemCost + *scalarCount;
}

std::string buildParenthesization(
    const std::vector<std::vector<std::size_t>>& split,
    std::size_t first,
    std::size_t last
) {
    if (first == last) {
        return "A" + std::to_string(first + 1);
    }
    const std::size_t middle = split[first][last];
    return "(" + buildParenthesization(split, first, middle) + " x " +
        buildParenthesization(split, middle + 1, last) + ")";
}

} // namespace

MatrixChainResult matrixChainMultiplication(
    const std::vector<std::int64_t>& dimensions
) {
    if (dimensions.empty()) {
        return {0, ""};
    }
    if (dimensions.size() < 2) {
        throw std::invalid_argument("Matrix chain requires at least two dimensions.");
    }
    for (std::int64_t dimension : dimensions) {
        if (dimension <= 0) {
            throw std::invalid_argument("Matrix dimensions must be positive.");
        }
    }
    const std::size_t matrixCount = dimensions.size() - 1;
    if (matrixCount > std::numeric_limits<std::size_t>::max() / matrixCount) {
        throw std::length_error("Matrix-chain DP table dimensions overflow.");
    }

    // cost[i][j] stores the minimum scalar multiplications for Ai...Aj.
    std::vector<std::vector<std::int64_t>> cost(
        matrixCount,
        std::vector<std::int64_t>(matrixCount, 0)
    );
    std::vector<std::vector<std::size_t>> split(
        matrixCount,
        std::vector<std::size_t>(matrixCount, 0)
    );
    for (std::size_t chainLength = 2; chainLength <= matrixCount; ++chainLength) {
        for (std::size_t first = 0; first + chainLength <= matrixCount; ++first) {
            const std::size_t last = first + chainLength - 1;
            std::optional<std::int64_t> best;
            for (std::size_t middle = first; middle < last; ++middle) {
                const auto candidate = checkedCost(
                    cost[first][middle],
                    cost[middle + 1][last],
                    dimensions[first],
                    dimensions[middle + 1],
                    dimensions[last + 1]
                );
                if (candidate && (!best || *candidate < *best)) {
                    best = candidate;
                    split[first][last] = middle;
                }
            }
            if (!best) {
                throw std::overflow_error("Every matrix-chain parenthesization exceeds int64 cost.");
            }
            cost[first][last] = *best;
        }
    }
    return {
        cost[0][matrixCount - 1],
        buildParenthesization(split, 0, matrixCount - 1)
    };
}

} // namespace project31::algorithms
