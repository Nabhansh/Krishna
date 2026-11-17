#include "algorithms/greedy/Prim.hpp"

#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace project31::algorithms {
namespace {

std::int64_t checkedAdd(std::int64_t left, std::int64_t right) {
    if ((right > 0 && left > std::numeric_limits<std::int64_t>::max() - right) ||
        (right < 0 && left < std::numeric_limits<std::int64_t>::min() - right)) {
        throw std::overflow_error("Spanning forest weight exceeds int64 range.");
    }
    return left + right;
}

} // namespace

SpanningForest primMinimumSpanningForest(
    std::size_t vertexCount,
    const std::vector<UndirectedWeightedEdge>& edges
) {
    std::vector<std::vector<std::pair<std::size_t, std::int64_t>>> adjacency(vertexCount);
    for (const UndirectedWeightedEdge& edge : edges) {
        if (edge.first >= vertexCount || edge.second >= vertexCount) {
            throw std::invalid_argument("Prim edge endpoint is outside the graph.");
        }
        adjacency[edge.first].emplace_back(edge.second, edge.weight);
        adjacency[edge.second].emplace_back(edge.first, edge.weight);
    }

    SpanningForest forest{0, 0, {}};
    std::vector<bool> visited(vertexCount, false);
    std::vector<std::optional<std::int64_t>> best(vertexCount);
    std::vector<std::size_t> parent(vertexCount, vertexCount);
    for (std::size_t root = 0; root < vertexCount; ++root) {
        if (visited[root]) {
            continue;
        }
        ++forest.componentCount;
        best[root] = 0;
        while (true) {
            std::size_t current = vertexCount;
            for (std::size_t vertex = 0; vertex < vertexCount; ++vertex) {
                if (!visited[vertex] && best[vertex] &&
                    (current == vertexCount || *best[vertex] < *best[current])) {
                    current = vertex;
                }
            }
            if (current == vertexCount) {
                break;
            }
            visited[current] = true;
            if (parent[current] != vertexCount) {
                forest.edges.push_back({parent[current], current, *best[current]});
                forest.totalWeight = checkedAdd(forest.totalWeight, *best[current]);
            }
            for (const auto& [neighbor, weight] : adjacency[current]) {
                if (!visited[neighbor] && (!best[neighbor] || weight < *best[neighbor])) {
                    best[neighbor] = weight;
                    parent[neighbor] = current;
                }
            }
        }
    }
    return forest;
}

} // namespace project31::algorithms
