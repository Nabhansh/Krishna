#include "algorithms/branch_bound/FifoBranchAndBound.hpp"
#include "algorithms/branch_bound/LifoBranchAndBound.hpp"
#include "algorithms/dynamic_programming/Knapsack.hpp"
#include "simulator/AirportOperations.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace project31::algorithms {
namespace {

const std::vector<BranchBoundItem> kEquipmentItems{
    {"scanner-upgrade", 4, 8},
    {"trace-detector", 3, 6},
    {"lane-sensor", 5, 10},
    {"radio-set", 2, 4}
};

std::vector<std::int64_t> weightsOf(const std::vector<BranchBoundItem>& items) {
    std::vector<std::int64_t> weights;
    for (const BranchBoundItem& item : items) {
        weights.push_back(item.weight);
    }
    return weights;
}

std::vector<std::int64_t> valuesOf(const std::vector<BranchBoundItem>& items) {
    std::vector<std::int64_t> values;
    for (const BranchBoundItem& item : items) {
        values.push_back(item.value);
    }
    return values;
}

void expectValidSelection(
    const BranchBoundKnapsackResult& result,
    const std::vector<BranchBoundItem>& items,
    std::int64_t capacity
) {
    std::int64_t totalWeight = 0;
    std::int64_t totalValue = 0;
    std::size_t previousIndex = 0;
    bool firstIndex = true;
    for (std::size_t index : result.selectedIndices) {
        ASSERT_LT(index, items.size());
        if (!firstIndex) {
            EXPECT_LT(previousIndex, index);
        }
        firstIndex = false;
        previousIndex = index;
        totalWeight += items[index].weight;
        totalValue += items[index].value;
    }
    EXPECT_LE(totalWeight, capacity);
    EXPECT_EQ(totalValue, result.maximumValue);
}

} // namespace

TEST(FifoBranchAndBoundTests, FindsTheOptimalEquipmentPackageSelection) {
    const auto result = solveKnapsackFifoBranchAndBound(kEquipmentItems, 10);
    EXPECT_EQ(result.maximumValue, 20);
    EXPECT_EQ(result.selectedIndices, (std::vector<std::size_t>{1, 2, 3}));
    expectValidSelection(result, kEquipmentItems, 10);
}

TEST(LifoBranchAndBoundTests, FindsTheOptimalEquipmentPackageSelection) {
    const auto result = solveKnapsackLifoBranchAndBound(kEquipmentItems, 10);
    EXPECT_EQ(result.maximumValue, 20);
    EXPECT_EQ(result.selectedIndices, (std::vector<std::size_t>{1, 2, 3}));
    expectValidSelection(result, kEquipmentItems, 10);
}

TEST(BranchAndBoundKnapsackTests, HandlesEmptyZeroCapacityAndInfeasibleItems) {
    const auto fifoEmpty = solveKnapsackFifoBranchAndBound({}, 8);
    const auto lifoEmpty = solveKnapsackLifoBranchAndBound({}, 8);
    EXPECT_EQ(fifoEmpty.maximumValue, 0);
    EXPECT_TRUE(fifoEmpty.selectedIndices.empty());
    EXPECT_EQ(lifoEmpty.maximumValue, 0);
    EXPECT_TRUE(lifoEmpty.selectedIndices.empty());

    EXPECT_EQ(
        solveKnapsackFifoBranchAndBound(kEquipmentItems, 0).maximumValue,
        0
    );
    EXPECT_EQ(
        solveKnapsackLifoBranchAndBound({{"oversized", 100, 50}}, 3).maximumValue,
        0
    );
}

TEST(BranchAndBoundKnapsackTests, RejectsInvalidCapacitiesAndItems) {
    EXPECT_THROW(solveKnapsackFifoBranchAndBound({}, -1), std::invalid_argument);
    EXPECT_THROW(solveKnapsackLifoBranchAndBound({}, -1), std::invalid_argument);
    EXPECT_THROW(
        solveKnapsackFifoBranchAndBound({{"zero-weight", 0, 5}}, 5),
        std::invalid_argument
    );
    EXPECT_THROW(
        solveKnapsackLifoBranchAndBound({{"negative-value", 1, -5}}, 5),
        std::invalid_argument
    );
    EXPECT_THROW(
        solveKnapsackFifoBranchAndBound({{" ", 1, 5}}, 5),
        std::invalid_argument
    );
    EXPECT_THROW(
        solveKnapsackLifoBranchAndBound(
            {{"duplicate", 1, 1}, {"duplicate", 2, 2}},
            3
        ),
        std::invalid_argument
    );
}

TEST(BranchAndBoundKnapsackTests, DetectsValueOverflowForFeasibleSelections) {
    const std::vector<BranchBoundItem> overflowingItems{
        {"max", 1, std::numeric_limits<std::int64_t>::max()},
        {"one-more", 1, 1}
    };
    EXPECT_THROW(
        solveKnapsackFifoBranchAndBound(overflowingItems, 2),
        std::overflow_error
    );
    EXPECT_THROW(
        solveKnapsackLifoBranchAndBound(overflowingItems, 2),
        std::overflow_error
    );
}

TEST(BranchAndBoundKnapsackTests, CrossChecksAgainstDynamicProgrammingKnapsack) {
    const std::vector<std::vector<BranchBoundItem>> instances{
        {},
        {{"a", 1, 2}, {"b", 2, 5}, {"c", 3, 7}},
        {{"a", 2, 5}, {"b", 3, 6}, {"c", 4, 8}, {"d", 5, 9}},
        {{"a", 1, 0}, {"b", 2, 4}, {"c", 2, 4}, {"d", 5, 10}}
    };
    for (const auto& items : instances) {
        const auto weights = weightsOf(items);
        const auto values = valuesOf(items);
        for (std::int64_t capacity = 0; capacity <= 10; ++capacity) {
            const KnapsackResult dynamic =
                solveKnapsack01(weights, values, capacity);
            const BranchBoundKnapsackResult fifo =
                solveKnapsackFifoBranchAndBound(items, capacity);
            const BranchBoundKnapsackResult lifo =
                solveKnapsackLifoBranchAndBound(items, capacity);
            EXPECT_EQ(fifo.maximumValue, dynamic.maximumValue);
            EXPECT_EQ(lifo.maximumValue, dynamic.maximumValue);
            expectValidSelection(fifo, items, capacity);
            expectValidSelection(lifo, items, capacity);
        }
    }
}

TEST(BranchAndBoundKnapsackTests, ReturnsDeterministicSolutions) {
    EXPECT_EQ(
        solveKnapsackFifoBranchAndBound(kEquipmentItems, 10).selectedIndices,
        solveKnapsackFifoBranchAndBound(kEquipmentItems, 10).selectedIndices
    );
    EXPECT_EQ(
        solveKnapsackLifoBranchAndBound(kEquipmentItems, 10).selectedIndices,
        solveKnapsackLifoBranchAndBound(kEquipmentItems, 10).selectedIndices
    );
}

} // namespace project31::algorithms

namespace project31 {

TEST(AirportBranchAndBoundTests, SelectsEquipmentPackagesThroughBothSearchOrders) {
    const std::vector<algorithms::BranchBoundItem> equipment{
        {"scanner-upgrade", 4, 8},
        {"trace-detector", 3, 6},
        {"lane-sensor", 5, 10},
        {"radio-set", 2, 4}
    };
    const auto fifo = AirportOperations::selectEquipmentFifoBranchAndBound(equipment, 10);
    const auto lifo = AirportOperations::selectEquipmentLifoBranchAndBound(equipment, 10);
    EXPECT_EQ(fifo.maximumValue, 20);
    EXPECT_EQ(lifo.maximumValue, 20);
}

} // namespace project31
