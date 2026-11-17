#include "algorithms/backtracking/NQueens.hpp"
#include "algorithms/backtracking/SubsetSum.hpp"
#include "simulator/AirportOperations.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace project31::algorithms {
namespace {

bool validQueensBoard(const std::vector<std::size_t>& columns) {
    for (std::size_t first = 0; first < columns.size(); ++first) {
        for (std::size_t second = first + 1; second < columns.size(); ++second) {
            if (columns[first] == columns[second]) {
                return false;
            }
            const std::size_t rowDistance = second - first;
            const std::size_t columnDistance =
                columns[first] > columns[second]
                    ? columns[first] - columns[second]
                    : columns[second] - columns[first];
            if (rowDistance == columnDistance) {
                return false;
            }
        }
    }
    return true;
}

bool bruteForceSubsetExists(
    const std::vector<std::int64_t>& values,
    std::int64_t target
) {
    const std::size_t subsets = std::size_t{1} << values.size();
    for (std::size_t mask = 0; mask < subsets; ++mask) {
        std::int64_t sum = 0;
        for (std::size_t index = 0; index < values.size(); ++index) {
            if ((mask & (std::size_t{1} << index)) != 0) {
                sum += values[index];
            }
        }
        if (sum == target) {
            return true;
        }
    }
    return false;
}

} // namespace

TEST(NQueensTests, HandlesSmallBoardSizesAndKnownSolutionCounts) {
    const auto one = solveNQueens(1);
    ASSERT_EQ(one.size(), 1U);
    EXPECT_EQ(one.front(), (std::vector<std::size_t>{0}));
    EXPECT_TRUE(solveNQueens(2).empty());
    EXPECT_TRUE(solveNQueens(3).empty());

    const auto four = solveNQueens(4);
    EXPECT_EQ(four.size(), 2U);
    EXPECT_EQ(solveNQueens(8).size(), 92U);
}

TEST(NQueensTests, EveryEnumeratedBoardHasNonattackingQueens) {
    for (std::size_t size = 1; size <= 8; ++size) {
        const auto solutions = solveNQueens(size);
        for (const auto& board : solutions) {
            ASSERT_EQ(board.size(), size);
            EXPECT_TRUE(validQueensBoard(board));
        }
    }
}

TEST(NQueensTests, RejectsInvalidAndUnboundedBoardSizes) {
    EXPECT_THROW(solveNQueens(0), std::invalid_argument);
    EXPECT_THROW(
        solveNQueens(kMaximumEnumeratedQueens + 1),
        std::length_error
    );
}

TEST(SubsetSumTests, FindsIndicesAndPreservesDuplicatesAndZeros) {
    const auto result = findSubsetSum({2, 3, 7}, 10);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, (std::vector<std::size_t>{1, 2}));

    const auto duplicateValues = findSubsetSum({4, 4, 9}, 8);
    ASSERT_TRUE(duplicateValues.has_value());
    ASSERT_EQ(duplicateValues->size(), 2U);
    EXPECT_EQ((*duplicateValues)[0], 0U);
    EXPECT_EQ((*duplicateValues)[1], 1U);

    const auto withZero = findSubsetSum({0, 5, 0}, 5);
    ASSERT_TRUE(withZero.has_value());
    EXPECT_EQ((*withZero).back(), 1U);
}

TEST(SubsetSumTests, HandlesEmptyZeroAndImpossibleTargets) {
    const auto emptyZero = findSubsetSum({}, 0);
    ASSERT_TRUE(emptyZero.has_value());
    EXPECT_TRUE(emptyZero->empty());
    EXPECT_FALSE(findSubsetSum({}, 1).has_value());
    EXPECT_FALSE(findSubsetSum({2, 4, 6}, 20).has_value());

    const auto zeroTarget = findSubsetSum({0, 0, 3}, 0);
    ASSERT_TRUE(zeroTarget.has_value());
    EXPECT_TRUE(zeroTarget->empty());
}

TEST(SubsetSumTests, MatchesBruteForceOnSmallInstances) {
    const std::vector<std::vector<std::int64_t>> instances{
        {}, {1}, {1, 2, 3}, {0, 2, 2, 5}, {4, 7, 9}
    };
    for (const auto& values : instances) {
        for (std::int64_t target = 0; target <= 14; ++target) {
            const auto result = findSubsetSum(values, target);
            EXPECT_EQ(
                result.has_value(),
                bruteForceSubsetExists(values, target)
            );
            if (result) {
                std::int64_t total = 0;
                for (std::size_t index : *result) {
                    ASSERT_LT(index, values.size());
                    total += values[index];
                }
                EXPECT_EQ(total, target);
            }
        }
    }
}

TEST(SubsetSumTests, RejectsNegativeValuesAndTargetsAndAvoidsSumOverflow) {
    EXPECT_THROW(findSubsetSum({1, -1}, 0), std::invalid_argument);
    EXPECT_THROW(findSubsetSum({1, 2}, -1), std::invalid_argument);

    const auto maxValue = findSubsetSum(
        {std::numeric_limits<std::int64_t>::max(), 1},
        std::numeric_limits<std::int64_t>::max()
    );
    ASSERT_TRUE(maxValue.has_value());
    EXPECT_EQ(*maxValue, (std::vector<std::size_t>{0}));
}

} // namespace project31::algorithms

namespace project31 {

TEST(AirportBacktrackingTests, AssignsPositionsAndSelectsExactCapacityPackages) {
    const auto positions = AirportOperations::assignScreeningPositions(4);
    EXPECT_EQ(positions.size(), 2U);
    ASSERT_FALSE(positions.empty());
    for (std::size_t first = 0; first < positions.front().size(); ++first) {
        for (std::size_t second = first + 1; second < positions.front().size(); ++second) {
            EXPECT_NE(positions.front()[first], positions.front()[second]);
            EXPECT_NE(
                second - first,
                positions.front()[first] > positions.front()[second]
                    ? positions.front()[first] - positions.front()[second]
                    : positions.front()[second] - positions.front()[first]
            );
        }
    }

    const auto packages =
        AirportOperations::selectResourcePackages({5, 7, 4, 3}, 10);
    ASSERT_TRUE(packages.has_value());
    std::int64_t capacity = 0;
    const std::vector<std::int64_t> packageSizes{5, 7, 4, 3};
    for (std::size_t index : *packages) {
        ASSERT_LT(index, packageSizes.size());
        capacity += packageSizes[index];
    }
    EXPECT_EQ(capacity, 10);
}

} // namespace project31
