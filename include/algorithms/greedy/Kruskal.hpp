#pragma once

#include "algorithms/greedy/WeightedGraphs.hpp"

#include <vector>

namespace project31::algorithms {

[[nodiscard]] SpanningForest kruskalMinimumSpanningForest(
    std::size_t vertexCount,
    std::vector<UndirectedWeightedEdge> edges
);

} // namespace project31::algorithms
