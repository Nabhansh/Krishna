#include "algorithms/number_theory/ChineseRemainderTheorem.hpp"
#include "simulator/AirportOperations.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace project31::algorithms {

TEST(ChineseRemainderTheoremTests, SolvesTheStandardThreeCongruenceExample) {
    const ChineseRemainderResult result =
        solveChineseRemainderTheorem({2, 3, 2}, {3, 5, 7});

    EXPECT_EQ(result.solution, 23);
    EXPECT_EQ(result.combinedModulus, 105);
}

TEST(ChineseRemainderTheoremTests, SolvesMultipleCongruencesAndNegativeResidues) {
    const ChineseRemainderResult result =
        solveChineseRemainderTheorem({-1, -2, 2, 1}, {5, 7, 3, 11});

    EXPECT_EQ(result.solution, 89);
    EXPECT_EQ(result.combinedModulus, 1155);
    EXPECT_EQ(result.solution % 5, 4);
    EXPECT_EQ(result.solution % 7, 5);
    EXPECT_EQ(result.solution % 3, 2);
    EXPECT_EQ(result.solution % 11, 1);
}

TEST(ChineseRemainderTheoremTests, HandlesUnitModuliAndEmptySystems) {
    const ChineseRemainderResult empty = solveChineseRemainderTheorem({}, {});
    EXPECT_EQ(empty.solution, 0);
    EXPECT_EQ(empty.combinedModulus, 1);

    const ChineseRemainderResult unit =
        solveChineseRemainderTheorem({123, -1}, {1, 3});
    EXPECT_EQ(unit.solution, 2);
    EXPECT_EQ(unit.combinedModulus, 3);
}

TEST(ChineseRemainderTheoremTests, RejectsNonCoprimeOrInvalidModuli) {
    EXPECT_THROW(
        solveChineseRemainderTheorem({1, 2}, {4, 6}),
        std::invalid_argument
    );
    EXPECT_THROW(
        solveChineseRemainderTheorem({1}, {0}),
        std::invalid_argument
    );
    EXPECT_THROW(
        solveChineseRemainderTheorem({1}, {-7}),
        std::invalid_argument
    );
    EXPECT_THROW(
        solveChineseRemainderTheorem({1, 2}, {3}),
        std::invalid_argument
    );
}

TEST(ChineseRemainderTheoremTests, DetectsCombinedModulusOverflow) {
    EXPECT_THROW(
        solveChineseRemainderTheorem(
            {0, 0},
            {3, std::numeric_limits<std::int64_t>::max()}
        ),
        std::overflow_error
    );
}

TEST(ChineseRemainderTheoremTests, ReturnsTheSameNormalizedResultDeterministically) {
    const auto first = solveChineseRemainderTheorem({-4, 8, 3}, {5, 7, 11});
    const auto second = solveChineseRemainderTheorem({-4, 8, 3}, {5, 7, 11});

    EXPECT_EQ(first.solution, second.solution);
    EXPECT_EQ(first.combinedModulus, second.combinedModulus);
    EXPECT_GE(first.solution, 0);
    EXPECT_LT(first.solution, first.combinedModulus);
}

TEST(ChineseRemainderTheoremTests, NormalizesTheMinimumSignedResidueSafely) {
    const auto result = solveChineseRemainderTheorem(
        {std::numeric_limits<std::int64_t>::min()},
        {7}
    );
    EXPECT_EQ(result.solution, 6);
    EXPECT_EQ(result.combinedModulus, 7);
}

TEST(ChineseRemainderTheoremTests, HandlesTheLargestPositiveModulus) {
    const std::int64_t largest = std::numeric_limits<std::int64_t>::max();
    const auto result = solveChineseRemainderTheorem(
        {std::numeric_limits<std::int64_t>::min()},
        {largest}
    );
    EXPECT_EQ(result.solution, largest - 1);
    EXPECT_EQ(result.combinedModulus, largest);
}

} // namespace project31::algorithms
