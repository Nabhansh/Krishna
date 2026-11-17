#include "algorithms/branch_bound/FifoBranchAndBound.hpp"
#include "algorithms/branch_bound/LeastCostBranchAndBound.hpp"
#include "algorithms/branch_bound/LifoBranchAndBound.hpp"
#include "algorithms/dynamic_programming/Knapsack.hpp"
#include "simulator/AirportOperations.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace project31::algorithms {
namespace {

const std::vector<BranchBoundItem> kEquipment{
    {"scanner-upgrade", 4, 8},
    {"trace-detector", 3, 6},
    {"lane-sensor", 5, 10},
    {"radio-set", 2, 4}
};

} // namespace

TEST(LeastCostBranchAndBoundTests, FindsTheMaximumEquipmentValueAndOriginalIndices) {
    const BranchBoundKnapsackResult result =
        solveKnapsackLeastCostBranchAndBound(kEquipment, 10);
    EXPECT_EQ(result.maximumValue, 20);
    EXPECT_EQ(result.selectedIndices, (std::vector<std::size_t>{1, 2, 3}));
}

TEST(LeastCostBranchAndBoundTests, MatchesFifoLifoAndDynamicProgrammingKnapsack) {
    const std::vector<std::vector<BranchBoundItem>> instances{
        {},
        {{"a", 1, 2}, {"b", 2, 5}, {"c", 3, 7}},
        {{"a", 2, 5}, {"b", 3, 6}, {"c", 4, 8}, {"d", 5, 9}},
        {{"a", 1, 0}, {"b", 2, 4}, {"c", 2, 4}, {"d", 5, 10}}
    };
    for (const auto& items : instances) {
        std::vector<std::int64_t> weights;
        std::vector<std::int64_t> values;
        for (const BranchBoundItem& item : items) {
            weights.push_back(item.weight);
            values.push_back(item.value);
        }
        for (std::int64_t capacity = 0; capacity <= 10; ++capacity) {
            const auto dynamic = solveKnapsack01(weights, values, capacity);
            const auto fifo = solveKnapsackFifoBranchAndBound(items, capacity);
            const auto lifo = solveKnapsackLifoBranchAndBound(items, capacity);
            const auto leastCost =
                solveKnapsackLeastCostBranchAndBound(items, capacity);
            EXPECT_EQ(leastCost.maximumValue, dynamic.maximumValue);
            EXPECT_EQ(leastCost.maximumValue, fifo.maximumValue);
            EXPECT_EQ(leastCost.maximumValue, lifo.maximumValue);
            std::int64_t selectedWeight = 0;
            std::int64_t selectedValue = 0;
            for (std::size_t index : leastCost.selectedIndices) {
                ASSERT_LT(index, items.size());
                selectedWeight += items[index].weight;
                selectedValue += items[index].value;
            }
            EXPECT_LE(selectedWeight, capacity);
            EXPECT_EQ(selectedValue, leastCost.maximumValue);
        }
    }
}

TEST(LeastCostBranchAndBoundTests, HandlesEmptyZeroCapacityAndInfeasibleItems) {
    const auto empty = solveKnapsackLeastCostBranchAndBound({}, 5);
    EXPECT_EQ(empty.maximumValue, 0);
    EXPECT_TRUE(empty.selectedIndices.empty());

    const auto zeroCapacity = solveKnapsackLeastCostBranchAndBound(kEquipment, 0);
    EXPECT_EQ(zeroCapacity.maximumValue, 0);
    EXPECT_TRUE(zeroCapacity.selectedIndices.empty());

    const auto noFit =
        solveKnapsackLeastCostBranchAndBound({{"large", 50, 100}}, 4);
    EXPECT_EQ(noFit.maximumValue, 0);
    EXPECT_TRUE(noFit.selectedIndices.empty());
}

TEST(LeastCostBranchAndBoundTests, RejectsInvalidInputAndDetectsValueOverflow) {
    EXPECT_THROW(solveKnapsackLeastCostBranchAndBound({}, -1), std::invalid_argument);
    EXPECT_THROW(
        solveKnapsackLeastCostBranchAndBound({{"weight", 0, 1}}, 1),
        std::invalid_argument
    );
    EXPECT_THROW(
        solveKnapsackLeastCostBranchAndBound({{"value", 1, -1}}, 1),
        std::invalid_argument
    );
    EXPECT_THROW(
        solveKnapsackLeastCostBranchAndBound({{" ", 1, 1}}, 1),
        std::invalid_argument
    );
    EXPECT_THROW(
        solveKnapsackLeastCostBranchAndBound(
            {{"same", 1, 1}, {"same", 2, 2}},
            3
        ),
        std::invalid_argument
    );
    EXPECT_THROW(
        solveKnapsackLeastCostBranchAndBound(
            {{"max", 1, std::numeric_limits<std::int64_t>::max()},
             {"one-more", 1, 1}},
            2
        ),
        std::overflow_error
    );
}

TEST(LeastCostBranchAndBoundTests, UsesDeterministicTieBreaking) {
    const std::vector<BranchBoundItem> tied{
        {"alpha", 1, 1}, {"beta", 1, 1}, {"gamma", 2, 2}
    };
    const auto first = solveKnapsackLeastCostBranchAndBound(tied, 2);
    const auto second = solveKnapsackLeastCostBranchAndBound(tied, 2);
    EXPECT_EQ(first.maximumValue, 2);
    EXPECT_EQ(first.selectedIndices, second.selectedIndices);
}

} // namespace project31::algorithms

namespace project31 {

TEST(AirportLeastCostBranchBoundTests, AddsBestFirstEquipmentPlanningStrategy) {
    const std::vector<algorithms::BranchBoundItem> equipment{
        {"scanner-upgrade", 4, 8},
        {"trace-detector", 3, 6},
        {"lane-sensor", 5, 10},
        {"radio-set", 2, 4}
    };
    const auto result =
        AirportOperations::selectEquipmentLeastCostBranchAndBound(equipment, 10);
    EXPECT_EQ(result.maximumValue, 20);
    EXPECT_EQ(result.selectedIndices, (std::vector<std::size_t>{1, 2, 3}));
}

} // namespace project31
