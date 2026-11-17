#include "algorithms/dynamic_programming/OptimalBinarySearchTree.hpp"

#include <cmath>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace project31::algorithms {
namespace {

using Table = std::vector<std::vector<double>>;
using RootTable = std::vector<std::vector<std::size_t>>;

std::unique_ptr<OptimalBinarySearchTreeNode> buildTree(
    const std::vector<std::string>& keys,
    const RootTable& roots,
    std::size_t begin,
    std::size_t end
) {
    if (begin == end) {
        return nullptr;
    }
    const std::size_t rootIndex = roots[begin][end];
    auto node = std::make_unique<OptimalBinarySearchTreeNode>();
    node->keyIndex = rootIndex;
    node->key = keys[rootIndex];
    node->left = buildTree(keys, roots, begin, rootIndex);
    node->right = buildTree(keys, roots, rootIndex + 1, end);
    return node;
}

} // namespace

OptimalBinarySearchTreeResult optimalBinarySearchTree(
    const std::vector<std::string>& sortedUniqueKeys,
    const std::vector<double>& successfulProbabilities,
    const std::vector<double>& unsuccessfulProbabilities
) {
    const std::size_t keyCount = sortedUniqueKeys.size();
    if (keyCount == std::numeric_limits<std::size_t>::max()) {
        throw std::length_error("Optimal BST table dimensions exceed the supported range.");
    }
    if (successfulProbabilities.size() != keyCount ||
        unsuccessfulProbabilities.size() != keyCount + 1) {
        throw std::invalid_argument(
            "Optimal BST requires n successful and n+1 unsuccessful probabilities."
        );
    }
    for (std::size_t index = 1; index < keyCount; ++index) {
        if (!(sortedUniqueKeys[index - 1] < sortedUniqueKeys[index])) {
            throw std::invalid_argument("Optimal BST keys must be sorted and unique.");
        }
    }
    const auto validateProbabilities = [](const std::vector<double>& probabilities) {
        for (double probability : probabilities) {
            if (!std::isfinite(probability) || probability < 0.0 || probability > 1.0) {
                throw std::invalid_argument(
                    "Optimal BST probabilities must be finite values in [0, 1]."
                );
            }
        }
    };
    validateProbabilities(successfulProbabilities);
    validateProbabilities(unsuccessfulProbabilities);
    double probabilityTotal = 0.0;
    for (double probability : successfulProbabilities) {
        probabilityTotal += probability;
    }
    for (double probability : unsuccessfulProbabilities) {
        probabilityTotal += probability;
    }
    if (!std::isfinite(probabilityTotal) ||
        std::abs(probabilityTotal - 1.0) > 1e-9) {
        throw std::invalid_argument("Optimal BST probabilities must sum to 1.");
    }

    const std::size_t dimension = keyCount + 1;
    if (dimension > std::numeric_limits<std::size_t>::max() / dimension ||
        dimension > Table().max_size() ||
        dimension > std::vector<double>().max_size() ||
        dimension > RootTable().max_size() ||
        dimension > std::vector<std::size_t>().max_size()) {
        throw std::length_error("Optimal BST table dimensions exceed the supported range.");
    }

    // Intervals are half-open [begin, end); empty intervals contain dummy keys.
    Table costs(dimension, std::vector<double>(dimension, 0.0));
    Table weights(dimension, std::vector<double>(dimension, 0.0));
    RootTable roots(dimension, std::vector<std::size_t>(dimension, 0));
    for (std::size_t index = 0; index <= keyCount; ++index) {
        costs[index][index] = unsuccessfulProbabilities[index];
        weights[index][index] = unsuccessfulProbabilities[index];
    }

    for (std::size_t length = 1; length <= keyCount; ++length) {
        for (std::size_t begin = 0; begin + length <= keyCount; ++begin) {
            const std::size_t end = begin + length;
            const double intervalWeight =
                weights[begin][end - 1] +
                successfulProbabilities[end - 1] +
                unsuccessfulProbabilities[end];
            if (!std::isfinite(intervalWeight)) {
                throw std::overflow_error("Optimal BST probability sum overflowed.");
            }
            weights[begin][end] = intervalWeight;

            std::optional<double> bestCost;
            for (std::size_t root = begin; root < end; ++root) {
                const double candidate =
                    costs[begin][root] +
                    costs[root + 1][end] +
                    intervalWeight;
                if (!std::isfinite(candidate)) {
                    continue;
                }
                if (!bestCost || candidate < *bestCost) {
                    bestCost = candidate;
                    roots[begin][end] = root;
                }
            }
            if (!bestCost) {
                throw std::overflow_error("Optimal BST expected cost overflowed.");
            }
            costs[begin][end] = *bestCost;
        }
    }

    return {
        costs[0][keyCount],
        buildTree(sortedUniqueKeys, roots, 0, keyCount)
    };
}

} // namespace project31::algorithms
