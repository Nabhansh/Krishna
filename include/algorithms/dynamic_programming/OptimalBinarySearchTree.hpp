#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace project31::algorithms {

struct OptimalBinarySearchTreeNode {
    std::size_t keyIndex;
    std::string key;
    std::unique_ptr<OptimalBinarySearchTreeNode> left;
    std::unique_ptr<OptimalBinarySearchTreeNode> right;
};

struct OptimalBinarySearchTreeResult {
    double expectedCost;
    std::unique_ptr<OptimalBinarySearchTreeNode> root;
};

[[nodiscard]] OptimalBinarySearchTreeResult optimalBinarySearchTree(
    const std::vector<std::string>& sortedUniqueKeys,
    const std::vector<double>& successfulProbabilities,
    const std::vector<double>& unsuccessfulProbabilities
);

} // namespace project31::algorithms
