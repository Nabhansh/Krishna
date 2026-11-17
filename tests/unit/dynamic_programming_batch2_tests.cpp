#include "algorithms/dynamic_programming/LongestCommonSubsequence.hpp"
#include "algorithms/dynamic_programming/OptimalBinarySearchTree.hpp"
#include "simulator/AirportOperations.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace project31::algorithms {
namespace {

bool isSubsequence(const std::string& candidate, const std::string& text) {
    std::size_t next = 0;
    for (char symbol : text) {
        if (next < candidate.size() && candidate[next] == symbol) {
            ++next;
        }
    }
    return next == candidate.size();
}

double bruteForceObstCost(
    const std::vector<double>& successful,
    const std::vector<double>& unsuccessful,
    std::size_t begin,
    std::size_t end,
    std::size_t depth
) {
    if (begin == end) {
        return unsuccessful[begin] * static_cast<double>(depth);
    }
    double best = std::numeric_limits<double>::infinity();
    for (std::size_t root = begin; root < end; ++root) {
        const double candidate =
            successful[root] * static_cast<double>(depth) +
            bruteForceObstCost(successful, unsuccessful, begin, root, depth + 1) +
            bruteForceObstCost(successful, unsuccessful, root + 1, end, depth + 1);
        if (candidate < best) {
            best = candidate;
        }
    }
    return best;
}

void appendInOrder(
    const OptimalBinarySearchTreeNode* node,
    std::vector<std::string>& keys
) {
    if (node == nullptr) {
        return;
    }
    appendInOrder(node->left.get(), keys);
    keys.push_back(node->key);
    appendInOrder(node->right.get(), keys);
}

double calculateTreeCost(
    const OptimalBinarySearchTreeNode* node,
    const std::vector<double>& successful,
    const std::vector<double>& unsuccessful,
    std::size_t begin,
    std::size_t end,
    std::size_t depth
) {
    if (node == nullptr) {
        EXPECT_EQ(begin, end);
        return unsuccessful[begin] * static_cast<double>(depth);
    }
    EXPECT_GE(node->keyIndex, begin);
    EXPECT_LT(node->keyIndex, end);
    const std::size_t root = node->keyIndex;
    return successful[root] * static_cast<double>(depth) +
        calculateTreeCost(
            node->left.get(), successful, unsuccessful, begin, root, depth + 1
        ) +
        calculateTreeCost(
            node->right.get(), successful, unsuccessful, root + 1, end, depth + 1
        );
}

} // namespace

TEST(LongestCommonSubsequenceTests, HandlesEmptyIdenticalAndRepeatedCharacterInputs) {
    const auto bothEmpty = longestCommonSubsequence("", "");
    EXPECT_EQ(bothEmpty.length, 0U);
    EXPECT_TRUE(bothEmpty.subsequence.empty());

    const auto oneEmpty = longestCommonSubsequence("ARRIVAL", "");
    EXPECT_EQ(oneEmpty.length, 0U);
    EXPECT_TRUE(oneEmpty.subsequence.empty());

    const auto identical = longestCommonSubsequence("AABBA", "AABBA");
    EXPECT_EQ(identical.length, 5U);
    EXPECT_EQ(identical.subsequence, "AABBA");

    const auto repeated = longestCommonSubsequence("AABBA", "ABABA");
    EXPECT_EQ(repeated.length, 4U);
    EXPECT_EQ(repeated.subsequence, "ABBA");
}

TEST(LongestCommonSubsequenceTests, ReconstructsOneOfSeveralValidLongestSequences) {
    const std::string first = "ABCBDAB";
    const std::string second = "BDCABA";
    const auto result = longestCommonSubsequence(first, second);

    EXPECT_EQ(result.length, 4U);
    EXPECT_EQ(result.subsequence.size(), result.length);
    EXPECT_TRUE(isSubsequence(result.subsequence, first));
    EXPECT_TRUE(isSubsequence(result.subsequence, second));

    const std::vector<std::string> validSolutions{"BCAB", "BCBA", "BDAB"};
    bool isKnownOptimalSolution = false;
    for (const std::string& valid : validSolutions) {
        if (result.subsequence == valid) {
            isKnownOptimalSolution = true;
        }
    }
    EXPECT_TRUE(isKnownOptimalSolution);
}

TEST(OptimalBinarySearchTreeTests, HandlesEmptyAndSingleKeyTrees) {
    const auto empty = optimalBinarySearchTree({}, {}, {1.0});
    EXPECT_DOUBLE_EQ(empty.expectedCost, 1.0);
    EXPECT_EQ(empty.root, nullptr);

    const auto single = optimalBinarySearchTree(
        {"flight-42"},
        {0.6},
        {0.2, 0.2}
    );
    EXPECT_NEAR(single.expectedCost, 1.4, 1e-12);
    ASSERT_NE(single.root, nullptr);
    EXPECT_EQ(single.root->key, "flight-42");
    EXPECT_EQ(single.root->keyIndex, 0U);
    EXPECT_EQ(single.root->left, nullptr);
    EXPECT_EQ(single.root->right, nullptr);
}

TEST(OptimalBinarySearchTreeTests, MatchesBruteForceAndReconstructsTheKnownOptimalTree) {
    const std::vector<std::string> keys{"10", "20", "30", "40", "50"};
    const std::vector<double> successful{0.15, 0.10, 0.05, 0.10, 0.20};
    const std::vector<double> unsuccessful{0.05, 0.10, 0.05, 0.05, 0.05, 0.10};

    const auto result = optimalBinarySearchTree(keys, successful, unsuccessful);
    EXPECT_NEAR(result.expectedCost, 2.75, 1e-12);
    ASSERT_NE(result.root, nullptr);
    EXPECT_EQ(result.root->key, "20");

    std::vector<std::string> reconstructedKeys;
    appendInOrder(result.root.get(), reconstructedKeys);
    EXPECT_EQ(reconstructedKeys, keys);
    EXPECT_NEAR(
        calculateTreeCost(
            result.root.get(), successful, unsuccessful, 0, keys.size(), 1
        ),
        result.expectedCost,
        1e-12
    );

    const std::vector<std::string> smallKeys{"a", "b", "c"};
    const std::vector<double> smallSuccessful{0.25, 0.10, 0.20};
    const std::vector<double> smallUnsuccessful{0.10, 0.05, 0.15, 0.15};
    const auto small = optimalBinarySearchTree(
        smallKeys, smallSuccessful, smallUnsuccessful
    );
    EXPECT_NEAR(
        small.expectedCost,
        bruteForceObstCost(smallSuccessful, smallUnsuccessful, 0, smallKeys.size(), 1),
        1e-12
    );
}

TEST(OptimalBinarySearchTreeTests, RejectsInvalidKeysAndProbabilityArrays) {
    EXPECT_THROW(
        optimalBinarySearchTree({"b", "a"}, {0.4, 0.3}, {0.1, 0.1, 0.1}),
        std::invalid_argument
    );
    EXPECT_THROW(
        optimalBinarySearchTree({"a", "a"}, {0.4, 0.3}, {0.1, 0.1, 0.1}),
        std::invalid_argument
    );
    EXPECT_THROW(optimalBinarySearchTree({"a"}, {}, {0.5, 0.5}), std::invalid_argument);
    EXPECT_THROW(optimalBinarySearchTree({"a"}, {0.5}, {0.5}), std::invalid_argument);
    EXPECT_THROW(
        optimalBinarySearchTree({"a"}, {0.5}, {0.1, 0.1}),
        std::invalid_argument
    );
    EXPECT_THROW(
        optimalBinarySearchTree({"a"}, {-0.1}, {0.5, 0.6}),
        std::invalid_argument
    );
    EXPECT_THROW(
        optimalBinarySearchTree({"a"}, {1.1}, {0.0, 0.0}),
        std::invalid_argument
    );
    EXPECT_THROW(
        optimalBinarySearchTree(
            {"a"},
            {std::numeric_limits<double>::infinity()},
            {0.0, 0.0}
        ),
        std::invalid_argument
    );
    EXPECT_THROW(
        optimalBinarySearchTree(
            {"a"},
            {0.5},
            {0.5, std::numeric_limits<double>::quiet_NaN()}
        ),
        std::invalid_argument
    );
}

} // namespace project31::algorithms

namespace project31 {

TEST(AirportOperationsDynamicProgrammingTests, ComparesHistoriesAndBuildsLookupTree) {
    const auto common = AirportOperations::compareOperationalEventHistories(
        "ARRIVE SCREEN QUEUE BOARD",
        "ARRIVE QUEUE SCREEN BOARD"
    );
    EXPECT_TRUE(common.length > 0U);

    const auto lookup = AirportOperations::buildOperationalLookupTree(
        {"event-arrival", "event-board", "event-screen"},
        {0.3, 0.2, 0.3},
        {0.05, 0.05, 0.05, 0.05}
    );
    ASSERT_NE(lookup.root, nullptr);
    EXPECT_EQ(lookup.root->key, "event-board");
    EXPECT_NEAR(lookup.expectedCost, 2.0, 1e-12);
}

} // namespace project31
