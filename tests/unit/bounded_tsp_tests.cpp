#include "algorithms/dynamic_programming/BoundedTravellingSalesperson.hpp"
#include "simulator/AirportOperations.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

namespace project31::algorithms {
namespace {

using CostMatrix = std::vector<std::vector<std::optional<std::int64_t>>>;

void expectValidTour(
    const BoundedTravellingSalespersonResult& result,
    const CostMatrix& costs,
    std::size_t start
) {
    const std::size_t vertexCount = costs.size();
    EXPECT_EQ(result.tour.size(), vertexCount + 1);
    if (result.tour.empty() || result.tour.size() != vertexCount + 1) {
        return;
    }
    for (std::size_t vertex : result.tour) {
        if (vertex >= vertexCount) {
            ADD_FAILURE() << "Tour contains an out-of-range vertex";
            return;
        }
    }
    EXPECT_EQ(result.tour.front(), start);
    EXPECT_EQ(result.tour.back(), start);

    std::vector<bool> visited(vertexCount, false);
    for (std::size_t index = 0; index < vertexCount; ++index) {
        EXPECT_FALSE(visited[result.tour[index]]);
        visited[result.tour[index]] = true;
    }
    for (bool wasVisited : visited) {
        EXPECT_TRUE(wasVisited);
    }

    std::int64_t total = 0;
    for (std::size_t index = 1; index < result.tour.size(); ++index) {
        const auto edge = costs[result.tour[index - 1]][result.tour[index]];
        if (!edge) {
            ADD_FAILURE() << "Tour uses a missing edge";
            return;
        }
        total += *edge;
    }
    EXPECT_EQ(total, result.totalCost);
}

} // namespace

TEST(BoundedTravellingSalespersonTests, FindsAndReconstructsTheKnownAsymmetricTour) {
    const CostMatrix costs{
        {std::nullopt, 10, 15, 20},
        {5, std::nullopt, 9, 10},
        {6, 13, std::nullopt, 12},
        {8, 8, 9, std::nullopt}
    };
    const auto result = solveBoundedTravellingSalesperson(costs);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->totalCost, 35);
    EXPECT_EQ(
        result->tour,
        (std::vector<std::size_t>{0, 1, 3, 2, 0})
    );
    expectValidTour(*result, costs, 0);
}

TEST(BoundedTravellingSalespersonTests, SupportsSymmetricCostsAndNonzeroStart) {
    const CostMatrix costs{
        {std::nullopt, 10, 15, 20},
        {10, std::nullopt, 35, 25},
        {15, 35, std::nullopt, 30},
        {20, 25, 30, std::nullopt}
    };
    const auto result = solveBoundedTravellingSalesperson(costs, 2);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->totalCost, 80);
    EXPECT_EQ(result->tour.front(), 2U);
    EXPECT_EQ(result->tour.back(), 2U);
    expectValidTour(*result, costs, 2);
}

TEST(BoundedTravellingSalespersonTests, HandlesSingleVertexAndInfeasibleGraphs) {
    const auto single = solveBoundedTravellingSalesperson({{std::nullopt}});
    ASSERT_TRUE(single.has_value());
    EXPECT_EQ(single->totalCost, 0);
    EXPECT_EQ(single->tour, (std::vector<std::size_t>{0, 0}));

    const CostMatrix disconnected{
        {std::nullopt, 2, std::nullopt},
        {std::nullopt, std::nullopt, 3},
        {std::nullopt, std::nullopt, std::nullopt}
    };
    EXPECT_FALSE(solveBoundedTravellingSalesperson(disconnected).has_value());
}

TEST(BoundedTravellingSalespersonTests, RejectsInvalidDimensionsStartsAndCosts) {
    EXPECT_THROW(solveBoundedTravellingSalesperson({}), std::invalid_argument);
    EXPECT_THROW(
        solveBoundedTravellingSalesperson({{std::nullopt, 1}, {1}}),
        std::invalid_argument
    );
    EXPECT_THROW(
        solveBoundedTravellingSalesperson({{std::nullopt, 1}, {1, std::nullopt}}, 2),
        std::invalid_argument
    );
    EXPECT_THROW(
        solveBoundedTravellingSalesperson(
            {{std::nullopt, -1}, {1, std::nullopt}}
        ),
        std::invalid_argument
    );
}

TEST(BoundedTravellingSalespersonTests, DetectsCostOverflow) {
    const auto largest = std::numeric_limits<std::int64_t>::max();
    EXPECT_THROW(
        solveBoundedTravellingSalesperson({
            {std::nullopt, largest},
            {largest, std::nullopt}
        }),
        std::overflow_error
    );
}

TEST(BoundedTravellingSalespersonTests, EnforcesTheDocumentedVertexLimit) {
    CostMatrix maximumSize(
        kMaximumHeldKarpVertices,
        std::vector<std::optional<std::int64_t>>(kMaximumHeldKarpVertices)
    );
    EXPECT_FALSE(solveBoundedTravellingSalesperson(maximumSize).has_value());

    CostMatrix overLimit(
        kMaximumHeldKarpVertices + 1,
        std::vector<std::optional<std::int64_t>>(kMaximumHeldKarpVertices + 1)
    );
    EXPECT_THROW(
        solveBoundedTravellingSalesperson(overLimit),
        std::length_error
    );
}

} // namespace project31::algorithms

namespace project31 {

TEST(AirportBoundedTspTests, PlansInspectionLocationTourThroughAirportFacade) {
    const std::vector<std::vector<std::optional<std::int64_t>>> distances{
        {std::nullopt, 10, 15, 20},
        {5, std::nullopt, 9, 10},
        {6, 13, std::nullopt, 12},
        {8, 8, 9, std::nullopt}
    };
    const auto result = AirportOperations::planAirportInspectionTour(distances);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->totalCost, 35);
    EXPECT_EQ(
        result->tour,
        (std::vector<std::size_t>{0, 1, 3, 2, 0})
    );
}

} // namespace project31
