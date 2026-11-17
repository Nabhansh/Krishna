#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace project31::algorithms {

struct DirectedWeightedEdge {
    std::size_t from;
    std::size_t to;
    std::int64_t weight;
};

struct UndirectedWeightedEdge {
    std::size_t first;
    std::size_t second;
    std::int64_t weight;
};

struct SpanningForest {
    std::size_t componentCount;
    std::int64_t totalWeight;
    std::vector<UndirectedWeightedEdge> edges;
};

} // namespace project31::algorithms
