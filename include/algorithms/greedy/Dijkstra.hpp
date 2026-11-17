#pragma once

#include "algorithms/greedy/WeightedGraphs.hpp"

#include <optional>
#include <vector>

namespace project31::algorithms {

[[nodiscard]] std::vector<std::optional<std::int64_t>> dijkstraShortestPaths(
    std::size_t vertexCount,
    const std::vector<DirectedWeightedEdge>& edges,
    std::size_t source
);

} // namespace project31::algorithms
