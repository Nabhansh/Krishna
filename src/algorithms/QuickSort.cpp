#include "algorithms/sorting/QuickSort.hpp"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace project31::algorithms {
namespace {

template <typename Value>
Value medianOfThree(const Value& first, const Value& middle, const Value& last) {
    if (first < middle) {
        if (middle < last) {
            return middle;
        }
        return first < last ? last : first;
    }
    if (first < last) {
        return first;
    }
    return middle < last ? last : middle;
}

template <typename Value>
void quickSortRange(std::vector<Value>& values, std::size_t begin, std::size_t end) {
    while (end - begin > 1) {
        const std::size_t middle = begin + (end - begin) / 2;
        const Value pivot = medianOfThree(values[begin], values[middle], values[end - 1]);

        std::size_t lower = begin;
        std::size_t current = begin;
        std::size_t upper = end;
        while (current < upper) {
            if (values[current] < pivot) {
                std::swap(values[lower++], values[current++]);
            } else if (pivot < values[current]) {
                std::swap(values[current], values[--upper]);
            } else {
                ++current;
            }
        }

        const std::size_t leftSize = lower - begin;
        const std::size_t rightSize = end - upper;
        if (leftSize < rightSize) {
            quickSortRange(values, begin, lower);
            begin = upper;
        } else {
            quickSortRange(values, upper, end);
            end = lower;
        }
    }
}

template <typename Value>
void quickSortValues(std::vector<Value>& values) {
    if (values.size() > 1) {
        quickSortRange(values, 0, values.size());
    }
}

} // namespace

void quickSort(std::vector<int>& values) {
    quickSortValues(values);
}

void quickSort(std::vector<std::string>& values) {
    quickSortValues(values);
}

} // namespace project31::algorithms
