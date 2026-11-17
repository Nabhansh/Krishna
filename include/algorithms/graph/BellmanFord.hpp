#pragma once

#include "algorithms/greedy/WeightedGraphs.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace project31::algorithms {

struct BellmanFordResult {
    std::size_t source;
    std::vector<std::optional<std::int64_t>> distances;
    std::vector<std::optional<std::size_t>> predecessors;

    [[nodiscard]] std::optional<std::vector<std::size_t>> pathTo(
        std::size_t destination
    ) const;
};

[[nodiscard]] BellmanFordResult bellmanFordShortestPaths(
    std::size_t vertexCount,
    const std::vector<DirectedWeightedEdge>& edges,
    std::size_t source
);

} // namespace project31::algorithms
