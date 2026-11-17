#pragma once

#include "algorithms/greedy/WeightedGraphs.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace project31::algorithms {

struct FloydWarshallResult {
    std::vector<std::vector<std::optional<std::int64_t>>> distances;
    std::vector<std::vector<std::optional<std::size_t>>> nextHops;

    [[nodiscard]] std::optional<std::vector<std::size_t>> path(
        std::size_t from,
        std::size_t to
    ) const;
};

[[nodiscard]] FloydWarshallResult floydWarshallShortestPaths(
    std::size_t vertexCount,
    const std::vector<DirectedWeightedEdge>& edges
);

} // namespace project31::algorithms
