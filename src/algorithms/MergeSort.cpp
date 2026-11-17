#include "algorithms/sorting/MergeSort.hpp"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace project31::algorithms {
namespace {

template <typename Value>
void mergeSortRange(
    std::vector<Value>& values,
    std::vector<Value>& buffer,
    std::size_t begin,
    std::size_t end
) {
    if (end - begin < 2) {
        return;
    }

    const std::size_t middle = begin + (end - begin) / 2;
    mergeSortRange(values, buffer, begin, middle);
    mergeSortRange(values, buffer, middle, end);

    std::size_t left = begin;
    std::size_t right = middle;
    std::size_t output = begin;
    while (left < middle && right < end) {
        if (values[right] < values[left]) {
            buffer[output++] = values[right++];
        } else {
            buffer[output++] = values[left++];
        }
    }
    while (left < middle) {
        buffer[output++] = values[left++];
    }
    while (right < end) {
        buffer[output++] = values[right++];
    }
    for (std::size_t index = begin; index < end; ++index) {
        values[index] = std::move(buffer[index]);
    }
}

template <typename Value>
void mergeSortValues(std::vector<Value>& values) {
    if (values.size() < 2) {
        return;
    }
    std::vector<Value> buffer(values.size());
    mergeSortRange(values, buffer, 0, values.size());
}

} // namespace

void mergeSort(std::vector<int>& values) {
    mergeSortValues(values);
}

void mergeSort(std::vector<std::string>& values) {
    mergeSortValues(values);
}

} // namespace project31::algorithms
