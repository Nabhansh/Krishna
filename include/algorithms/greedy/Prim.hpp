#pragma once

#include "algorithms/greedy/WeightedGraphs.hpp"

#include <vector>

namespace project31::algorithms {

[[nodiscard]] SpanningForest primMinimumSpanningForest(
    std::size_t vertexCount,
    const std::vector<UndirectedWeightedEdge>& edges
);

} // namespace project31::algorithms
