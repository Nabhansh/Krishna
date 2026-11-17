#include "algorithms/flow/MaximumBipartiteMatching.hpp"

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace project31::algorithms {
namespace {

bool augmentMatching(
    std::size_t left,
    const std::vector<std::vector<std::size_t>>& adjacency,
    std::vector<std::optional<std::size_t>>& matchedRight,
    std::vector<std::optional<std::size_t>>& matchedLeft,
    std::vector<bool>& visitedRight,
    std::vector<std::size_t>& touchedRight
) {
    for (std::size_t right : adjacency[left]) {
        if (visitedRight[right]) {
            continue;
        }
        visitedRight[right] = true;
        touchedRight.push_back(right);
        if (!matchedRight[right] ||
            augmentMatching(
                *matchedRight[right],
                adjacency,
                matchedRight,
                matchedLeft,
                visitedRight,
                touchedRight
            )) {
            matchedRight[right] = left;
            matchedLeft[left] = right;
            return true;
        }
    }
    return false;
}

} // namespace

MaximumBipartiteMatchingResult maximumBipartiteMatching(
    std::size_t leftPartitionSize,
    std::size_t rightPartitionSize,
    const std::vector<BipartiteEdge>& allowedEdges
) {
    if (leftPartitionSize >
            std::vector<std::vector<std::size_t>>().max_size() ||
        rightPartitionSize > std::vector<std::optional<std::size_t>>().max_size() ||
        leftPartitionSize > std::vector<std::optional<std::size_t>>().max_size() ||
        allowedEdges.size() > std::vector<std::size_t>().max_size()) {
        throw std::length_error("Bipartite graph exceeds container capacity.");
    }
    for (const BipartiteEdge& edge : allowedEdges) {
        if (edge.left >= leftPartitionSize || edge.right >= rightPartitionSize) {
            throw std::invalid_argument("Bipartite edge endpoint is outside its partition.");
        }
    }

    std::vector<std::vector<std::size_t>> adjacency(leftPartitionSize);
    for (const BipartiteEdge& edge : allowedEdges) {
        adjacency[edge.left].push_back(edge.right);
    }

    std::vector<std::optional<std::size_t>> matchedRight(rightPartitionSize);
    std::vector<std::optional<std::size_t>> matchedLeft(leftPartitionSize);
    std::vector<bool> visitedRight(rightPartitionSize, false);
    std::vector<std::size_t> touchedRight;
    touchedRight.reserve(rightPartitionSize);
    std::size_t cardinality = 0;

    for (std::size_t left = 0; left < leftPartitionSize; ++left) {
        if (augmentMatching(
                left,
                adjacency,
                matchedRight,
                matchedLeft,
                visitedRight,
                touchedRight
            )) {
            ++cardinality;
        }
        for (std::size_t right : touchedRight) {
            visitedRight[right] = false;
        }
        touchedRight.clear();
    }

    std::vector<std::pair<std::size_t, std::size_t>> matchedPairs;
    matchedPairs.reserve(cardinality);
    for (std::size_t left = 0; left < leftPartitionSize; ++left) {
        if (matchedLeft[left]) {
            matchedPairs.emplace_back(left, *matchedLeft[left]);
        }
    }
    return {cardinality, std::move(matchedPairs)};
}

} // namespace project31::algorithms
