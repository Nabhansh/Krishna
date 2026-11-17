#include "algorithms/searching/BinarySearch.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace project31::algorithms {
namespace {

template <typename Value>
std::size_t lowerBoundIterative(
    const std::vector<Value>& values,
    const Value& target
) {
    std::size_t begin = 0;
    std::size_t end = values.size();
    while (begin < end) {
        const std::size_t middle = begin + (end - begin) / 2;
        if (values[middle] < target) {
            begin = middle + 1;
        } else {
            end = middle;
        }
    }
    return begin;
}

template <typename Value>
std::size_t lowerBoundRecursive(
    const std::vector<Value>& values,
    const Value& target,
    std::size_t begin,
    std::size_t end
) {
    if (begin == end) {
        return begin;
    }
    const std::size_t middle = begin + (end - begin) / 2;
    if (values[middle] < target) {
        return lowerBoundRecursive(values, target, middle + 1, end);
    }
    return lowerBoundRecursive(values, target, begin, middle);
}

template <typename Value>
std::optional<std::size_t> iterativeSearch(
    const std::vector<Value>& values,
    const Value& target
) {
    const std::size_t index = lowerBoundIterative(values, target);
    if (index < values.size() && values[index] == target) {
        return index;
    }
    return std::nullopt;
}

template <typename Value>
std::optional<std::size_t> recursiveSearch(
    const std::vector<Value>& values,
    const Value& target
) {
    const std::size_t index = lowerBoundRecursive(values, target, 0, values.size());
    if (index < values.size() && values[index] == target) {
        return index;
    }
    return std::nullopt;
}

} // namespace

std::optional<std::size_t> iterativeBinarySearch(
    const std::vector<int>& values,
    int target
) {
    return iterativeSearch(values, target);
}

std::optional<std::size_t> recursiveBinarySearch(
    const std::vector<int>& values,
    int target
) {
    return recursiveSearch(values, target);
}

std::optional<std::size_t> iterativeBinarySearch(
    const std::vector<std::string>& values,
    const std::string& target
) {
    return iterativeSearch(values, target);
}

std::optional<std::size_t> recursiveBinarySearch(
    const std::vector<std::string>& values,
    const std::string& target
) {
    return recursiveSearch(values, target);
}

std::size_t lowerBound(
    const std::vector<std::string>& values,
    const std::string& target
) {
    return lowerBoundIterative(values, target);
}

} // namespace project31::algorithms
