#include "algorithms/dynamic_programming/Knapsack.hpp"
#include "algorithms/dynamic_programming/MatrixChainMultiplication.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace project31::algorithms {
namespace {

std::int64_t bruteForceKnapsack(
    const std::vector<std::int64_t>& weights,
    const std::vector<std::int64_t>& values,
    std::int64_t capacity
) {
    std::int64_t best = 0;
    const std::size_t combinations = std::size_t{1} << weights.size();
    for (std::size_t mask = 0; mask < combinations; ++mask) {
        std::int64_t totalWeight = 0;
        std::int64_t totalValue = 0;
        for (std::size_t item = 0; item < weights.size(); ++item) {
            if ((mask & (std::size_t{1} << item)) != 0) {
                totalWeight += weights[item];
                totalValue += values[item];
            }
        }
        if (totalWeight <= capacity && totalValue > best) {
            best = totalValue;
        }
    }
    return best;
}

} // namespace

TEST(KnapsackTests, ReturnsOptimalValueAndReconstructedItems) {
    const KnapsackResult result =
        solveKnapsack01({2, 3, 4, 5}, {3, 4, 5, 6}, 8);

    EXPECT_EQ(result.maximumValue, 10);
    ASSERT_EQ(result.selectedIndices.size(), 2U);
    EXPECT_EQ(result.selectedIndices[0], 1U);
    EXPECT_EQ(result.selectedIndices[1], 3U);
}

TEST(KnapsackTests, HandlesEmptyZeroCapacityAndZeroWeightItems) {
    const KnapsackResult empty = solveKnapsack01({}, {}, 10);
    EXPECT_EQ(empty.maximumValue, 0);
    EXPECT_TRUE(empty.selectedIndices.empty());

    const KnapsackResult zeroCapacity = solveKnapsack01({0, 1}, {5, 100}, 0);
    EXPECT_EQ(zeroCapacity.maximumValue, 5);
    EXPECT_EQ(zeroCapacity.selectedIndices, (std::vector<std::size_t>{0}));

    const KnapsackResult noItemsFit = solveKnapsack01({3, 4}, {8, 9}, 2);
    EXPECT_EQ(noItemsFit.maximumValue, 0);
    EXPECT_TRUE(noItemsFit.selectedIndices.empty());
}

TEST(KnapsackTests, MatchesBruteForceOnSmallInstances) {
    const std::vector<std::vector<std::int64_t>> weights{
        {1, 2, 3}, {0, 3, 2, 4}, {2, 2, 2, 2, 2}, {1, 4, 3, 2, 5, 1}
    };
    const std::vector<std::vector<std::int64_t>> values{
        {2, 5, 7}, {4, 8, 5, 9}, {3, 3, 3, 3, 3}, {2, 9, 6, 4, 10, 1}
    };
    for (std::size_t instance = 0; instance < weights.size(); ++instance) {
        for (std::int64_t capacity = 0; capacity <= 10; ++capacity) {
            const KnapsackResult result =
                solveKnapsack01(weights[instance], values[instance], capacity);
            EXPECT_EQ(
                result.maximumValue,
                bruteForceKnapsack(weights[instance], values[instance], capacity)
            );
            std::int64_t chosenWeight = 0;
            std::int64_t chosenValue = 0;
            for (std::size_t index : result.selectedIndices) {
                ASSERT_LT(index, weights[instance].size());
                chosenWeight += weights[instance][index];
                chosenValue += values[instance][index];
            }
            EXPECT_LE(chosenWeight, capacity);
            EXPECT_EQ(chosenValue, result.maximumValue);
        }
    }
}

TEST(KnapsackTests, RejectsInvalidInputsAndProtectsValueAndTableOverflow) {
    EXPECT_THROW(solveKnapsack01({1}, {}, 1), std::invalid_argument);
    EXPECT_THROW(solveKnapsack01({-1}, {2}, 1), std::invalid_argument);
    EXPECT_THROW(solveKnapsack01({1}, {-2}, 1), std::invalid_argument);
    EXPECT_THROW(solveKnapsack01({1}, {2}, -1), std::invalid_argument);
    EXPECT_THROW(
        solveKnapsack01(
            {1, 1},
            {std::numeric_limits<std::int64_t>::max(), 1},
            2
        ),
        std::overflow_error
    );
    EXPECT_THROW(
        solveKnapsack01({}, {}, std::numeric_limits<std::int64_t>::max()),
        std::length_error
    );
}

TEST(MatrixChainTests, FindsMinimumCostAndOptimalParenthesization) {
    const MatrixChainResult classic = matrixChainMultiplication({10, 30, 5, 60});
    EXPECT_EQ(classic.minimumCost, 4500);
    EXPECT_EQ(classic.parenthesization, "((A1 x A2) x A3)");

    const MatrixChainResult longer =
        matrixChainMultiplication({40, 20, 30, 10, 30});
    EXPECT_EQ(longer.minimumCost, 26000);
    EXPECT_EQ(longer.parenthesization, "((A1 x (A2 x A3)) x A4)");
}

TEST(MatrixChainTests, HandlesEmptyAndSingleMatrixChains) {
    const MatrixChainResult empty = matrixChainMultiplication({});
    EXPECT_EQ(empty.minimumCost, 0);
    EXPECT_TRUE(empty.parenthesization.empty());

    const MatrixChainResult single = matrixChainMultiplication({25, 40});
    EXPECT_EQ(single.minimumCost, 0);
    EXPECT_EQ(single.parenthesization, "A1");
}

TEST(MatrixChainTests, RejectsInvalidDimensionsAndProtectsCostOverflow) {
    EXPECT_THROW(matrixChainMultiplication({10}), std::invalid_argument);
    EXPECT_THROW(matrixChainMultiplication({10, 0, 5}), std::invalid_argument);
    EXPECT_THROW(matrixChainMultiplication({10, -1}), std::invalid_argument);
    EXPECT_THROW(
        matrixChainMultiplication({
            std::numeric_limits<std::int64_t>::max(),
            std::numeric_limits<std::int64_t>::max(),
            std::numeric_limits<std::int64_t>::max()
        }),
        std::overflow_error
    );
}

} // namespace project31::algorithms
