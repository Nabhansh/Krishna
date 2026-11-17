#include "algorithms/greedy/Dijkstra.hpp"

#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace project31::algorithms {

std::vector<std::optional<std::int64_t>> dijkstraShortestPaths(
    std::size_t vertexCount,
    const std::vector<DirectedWeightedEdge>& edges,
    std::size_t source
) {
    if (source >= vertexCount) {
        throw std::invalid_argument("Dijkstra source must be a valid vertex.");
    }
    std::vector<std::vector<std::pair<std::size_t, std::int64_t>>> adjacency(vertexCount);
    for (const DirectedWeightedEdge& edge : edges) {
        if (edge.from >= vertexCount || edge.to >= vertexCount) {
            throw std::invalid_argument("Dijkstra edge endpoint is outside the graph.");
        }
        if (edge.weight < 0) {
            throw std::invalid_argument("Dijkstra does not accept negative edge weights.");
        }
        adjacency[edge.from].emplace_back(edge.to, edge.weight);
    }

    std::vector<std::optional<std::int64_t>> distances(vertexCount);
    std::vector<bool> visited(vertexCount, false);
    distances[source] = 0;
    for (std::size_t iteration = 0; iteration < vertexCount; ++iteration) {
        std::size_t current = vertexCount;
        for (std::size_t vertex = 0; vertex < vertexCount; ++vertex) {
            if (!visited[vertex] && distances[vertex] &&
                (current == vertexCount || *distances[vertex] < *distances[current])) {
                current = vertex;
            }
        }
        if (current == vertexCount) {
            break;
        }
        visited[current] = true;
        for (const auto& [neighbor, weight] : adjacency[current]) {
            if (*distances[current] >
                std::numeric_limits<std::int64_t>::max() - weight) {
                throw std::overflow_error("Dijkstra path length exceeds int64 range.");
            }
            const std::int64_t candidate = *distances[current] + weight;
            if (!distances[neighbor] || candidate < *distances[neighbor]) {
                distances[neighbor] = candidate;
            }
        }
    }
    return distances;
}

} // namespace project31::algorithms
