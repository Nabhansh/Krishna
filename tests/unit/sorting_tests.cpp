#include "algorithms/sorting/MergeSort.hpp"
#include "algorithms/sorting/QuickSort.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace project31::algorithms {

TEST(SortingTests, MergeSortHandlesEmptySingleAndDuplicateValues) {
    std::vector<int> empty;
    std::vector<int> single{42};
    std::vector<int> duplicates{4, 1, 4, 2, 1, 4};

    mergeSort(empty);
    mergeSort(single);
    mergeSort(duplicates);

    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(single, (std::vector<int>{42}));
    EXPECT_EQ(duplicates, (std::vector<int>{1, 1, 2, 4, 4, 4}));
}

TEST(SortingTests, MergeSortOrdersPassengerIdsAndAlreadySortedInput) {
    std::vector<std::string> ids{"P-104", "P-002", "P-019", "P-002"};
    mergeSort(ids);
    EXPECT_EQ(ids, (std::vector<std::string>{"P-002", "P-002", "P-019", "P-104"}));

    mergeSort(ids);
    EXPECT_EQ(ids, (std::vector<std::string>{"P-002", "P-002", "P-019", "P-104"}));
}

TEST(SortingTests, MergeSortHandlesReverseOrderedInput) {
    std::vector<int> reversed(20000);
    for (std::size_t index = 0; index < reversed.size(); ++index) {
        reversed[index] = static_cast<int>(reversed.size() - index);
    }

    mergeSort(reversed);

    for (std::size_t index = 0; index < reversed.size(); ++index) {
        EXPECT_EQ(reversed[index], static_cast<int>(index + 1));
    }
}

TEST(SortingTests, QuickSortHandlesEmptySingleDuplicateAndOrderedInputs) {
    std::vector<int> empty;
    std::vector<int> single{7};
    std::vector<int> duplicates{5, 5, 1, 5, 1, 3};
    std::vector<int> ordered(30000);
    for (std::size_t index = 0; index < ordered.size(); ++index) {
        ordered[index] = static_cast<int>(index);
    }

    quickSort(empty);
    quickSort(single);
    quickSort(duplicates);
    quickSort(ordered);

    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(single, (std::vector<int>{7}));
    EXPECT_EQ(duplicates, (std::vector<int>{1, 1, 3, 5, 5, 5}));
    for (std::size_t index = 0; index < ordered.size(); ++index) {
        EXPECT_EQ(ordered[index], static_cast<int>(index));
    }
}

TEST(SortingTests, QuickSortHandlesReverseOrderedAndAllEqualInputs) {
    std::vector<int> reversed(30000);
    for (std::size_t index = 0; index < reversed.size(); ++index) {
        reversed[index] = static_cast<int>(reversed.size() - index);
    }
    std::vector<int> equalValues(30000, 9);

    quickSort(reversed);
    quickSort(equalValues);

    for (std::size_t index = 1; index < reversed.size(); ++index) {
        EXPECT_LE(reversed[index - 1], reversed[index]);
    }
    for (int value : equalValues) {
        EXPECT_EQ(value, 9);
    }
}

} // namespace project31::algorithms
