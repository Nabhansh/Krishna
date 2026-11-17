#include "algorithms/divide_conquer/CelebrityProblem.hpp"
#include "algorithms/divide_conquer/ClassicalMatrixMultiplication.hpp"
#include "algorithms/divide_conquer/FastExponentiation.hpp"
#include "algorithms/divide_conquer/MaximumSubarray.hpp"
#include "algorithms/divide_conquer/StrassenMatrixMultiplication.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace project31::algorithms {

TEST(MaximumSubarrayTests, FindsMaximumContiguousRangeAndHandlesAllNegativeData) {
    const auto result = maximumSubarray({-2, 1, -3, 4, -1, 2, 1, -5, 4});
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->sum, 6);
    EXPECT_EQ(result->begin, 3U);
    EXPECT_EQ(result->end, 7U);

    const auto negative = maximumSubarray({-8, -3, -6});
    ASSERT_TRUE(negative.has_value());
    EXPECT_EQ(negative->sum, -3);
    EXPECT_EQ(negative->begin, 1U);
    EXPECT_EQ(negative->end, 2U);
    EXPECT_FALSE(maximumSubarray({}).has_value());
}

TEST(MaximumSubarrayTests, DetectsUnrepresentableSums) {
    EXPECT_THROW(
        maximumSubarray({
            std::numeric_limits<std::int64_t>::max(),
            1
        }),
        std::overflow_error
    );
}

TEST(FastExponentiationTests, HandlesPositiveNegativeAndBoundaryExponents) {
    EXPECT_DOUBLE_EQ(fastExponentiation(2.0, 10), 1024.0);
    EXPECT_DOUBLE_EQ(fastExponentiation(2.0, -3), 0.125);
    EXPECT_DOUBLE_EQ(
        fastExponentiation(-1.0, std::numeric_limits<std::int64_t>::min()),
        1.0
    );
    EXPECT_DOUBLE_EQ(fastExponentiation(0.0, 0), 1.0);
}

TEST(FastExponentiationTests, RejectsInvalidAndOverflowingResults) {
    EXPECT_THROW(fastExponentiation(0.0, -1), std::invalid_argument);
    EXPECT_THROW(fastExponentiation(INFINITY, 2), std::invalid_argument);
    EXPECT_THROW(fastExponentiation(1e200, 3), std::overflow_error);
}

TEST(CelebrityProblemTests, FindsCelebrityAndHandlesNoSolution) {
    EXPECT_EQ(
        findCelebrity({{0, 1, 1}, {0, 0, 0}, {0, 1, 0}}),
        1U
    );
    EXPECT_FALSE(findCelebrity({{0, 1}, {1, 0}}).has_value());
    EXPECT_FALSE(findCelebrity({}).has_value());
}

TEST(CelebrityProblemTests, ValidatesRelationMatrix) {
    EXPECT_THROW(findCelebrity({{0, 1}, {0}}), std::invalid_argument);
    EXPECT_THROW(findCelebrity({{0, 2}, {0, 0}}), std::invalid_argument);
}

TEST(ClassicalMatrixMultiplicationTests, MultipliesRectangularMatrices) {
    const Matrix result = classicalMatrixMultiply(
        {{1, 2, 3}, {4, 5, 6}},
        {{7, 8}, {9, 10}, {11, 12}}
    );
    EXPECT_EQ(result, (Matrix{{58, 64}, {139, 154}}));
}

TEST(ClassicalMatrixMultiplicationTests, RejectsInvalidMatrixShapes) {
    EXPECT_THROW(classicalMatrixMultiply({{1, 2}, {3}}, {{1}, {2}}), std::invalid_argument);
    EXPECT_THROW(classicalMatrixMultiply({{1, 2}}, {{1, 2}}), std::invalid_argument);
}

TEST(StrassenMatrixMultiplicationTests, MatchesClassicalForRectangularAndPaddedInputs) {
    const Matrix left{{1, 2, 3}, {4, 5, 6}};
    const Matrix right{{7, 8}, {9, 10}, {11, 12}};
    EXPECT_EQ(strassenMatrixMultiply(left, right), classicalMatrixMultiply(left, right));

    Matrix identity(33, std::vector<double>(33, 0.0));
    Matrix values(33, std::vector<double>(33, 0.0));
    for (std::size_t row = 0; row < 33; ++row) {
        identity[row][row] = 1.0;
        for (std::size_t column = 0; column < 33; ++column) {
            values[row][column] = static_cast<double>(row + column);
        }
    }
    EXPECT_EQ(strassenMatrixMultiply(identity, values), values);
}

TEST(StrassenMatrixMultiplicationTests, RejectsIncompatibleAndRaggedMatrices) {
    EXPECT_THROW(strassenMatrixMultiply({{1, 2}}, {{1, 2}}), std::invalid_argument);
    EXPECT_THROW(strassenMatrixMultiply({{1, 2}, {3}}, {{1}, {2}}), std::invalid_argument);
}

} // namespace project31::algorithms
