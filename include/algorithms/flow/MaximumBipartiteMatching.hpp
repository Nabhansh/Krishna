#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace project31::algorithms {

struct BipartiteEdge {
    std::size_t left;
    std::size_t right;
};

struct MaximumBipartiteMatchingResult {
    std::size_t cardinality;
    std::vector<std::pair<std::size_t, std::size_t>> matchedPairs;
};

[[nodiscard]] MaximumBipartiteMatchingResult maximumBipartiteMatching(
    std::size_t leftPartitionSize,
    std::size_t rightPartitionSize,
    const std::vector<BipartiteEdge>& allowedEdges
);

} // namespace project31::algorithms
