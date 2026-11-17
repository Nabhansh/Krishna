#include "algorithms/searching/BinarySearch.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace project31::algorithms {

TEST(BinarySearchTests, IterativeSearchHandlesEmptyAndSortedIntegerInputs) {
    const std::vector<int> empty;
    const std::vector<int> sorted{-8, -2, 0, 3, 7, 12};

    EXPECT_FALSE(iterativeBinarySearch(empty, 1).has_value());
    EXPECT_EQ(iterativeBinarySearch(sorted, -8), 0U);
    EXPECT_EQ(iterativeBinarySearch(sorted, 12), 5U);
    EXPECT_EQ(iterativeBinarySearch(sorted, 3), 3U);
    EXPECT_FALSE(iterativeBinarySearch(sorted, 4).has_value());
}

TEST(BinarySearchTests, RecursiveSearchHandlesEmptyAndDuplicateKeys) {
    const std::vector<int> empty;
    const std::vector<int> sorted{1, 2, 2, 2, 4, 8};

    EXPECT_FALSE(recursiveBinarySearch(empty, 2).has_value());
    EXPECT_EQ(recursiveBinarySearch(sorted, 2), 1U);
    EXPECT_EQ(recursiveBinarySearch(sorted, 8), 5U);
    EXPECT_FALSE(recursiveBinarySearch(sorted, 9).has_value());
}

TEST(BinarySearchTests, BothSearchesFindPassengerIdsAndReturnFirstDuplicate) {
    const std::vector<std::string> ids{
        "P-001", "P-002", "P-002", "P-019", "P-104"
    };

    EXPECT_EQ(iterativeBinarySearch(ids, "P-002"), 1U);
    EXPECT_EQ(recursiveBinarySearch(ids, "P-002"), 1U);
    EXPECT_EQ(iterativeBinarySearch(ids, "P-019"), 3U);
    EXPECT_EQ(recursiveBinarySearch(ids, "P-000"), std::nullopt);
}

} // namespace project31::algorithms
