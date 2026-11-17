#include "algorithms/graph/BellmanFord.hpp"

#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace project31::algorithms {
namespace {

std::int64_t checkedPathSum(std::int64_t distance, std::int64_t edgeWeight) {
    if ((edgeWeight > 0 &&
         distance > std::numeric_limits<std::int64_t>::max() - edgeWeight) ||
        (edgeWeight < 0 &&
         distance < std::numeric_limits<std::int64_t>::min() - edgeWeight)) {
        throw std::overflow_error("Bellman-Ford path distance exceeds int64 range.");
    }
    return distance + edgeWeight;
}

} // namespace

std::optional<std::vector<std::size_t>> BellmanFordResult::pathTo(
    std::size_t destination
) const {
    if (destination >= distances.size()) {
        throw std::out_of_range("Bellman-Ford destination is outside the graph.");
    }
    if (!distances[destination]) {
        return std::nullopt;
    }

    std::vector<std::size_t> reversedPath;
    std::size_t current = destination;
    for (std::size_t count = 0; count < distances.size(); ++count) {
        reversedPath.push_back(current);
        if (current == source) {
            for (std::size_t left = 0, right = reversedPath.size();
                 left < right / 2;
                 ++left) {
                std::swap(reversedPath[left], reversedPath[right - left - 1]);
            }
            return reversedPath;
        }
        if (current >= predecessors.size() || !predecessors[current]) {
            throw std::logic_error("Bellman-Ford predecessor chain is incomplete.");
        }
        current = *predecessors[current];
        if (current >= distances.size()) {
            throw std::logic_error("Bellman-Ford predecessor is outside the graph.");
        }
    }
    throw std::logic_error("Bellman-Ford predecessor chain contains a cycle.");
}

BellmanFordResult bellmanFordShortestPaths(
    std::size_t vertexCount,
    const std::vector<DirectedWeightedEdge>& edges,
    std::size_t source
) {
    if (vertexCount == 0) {
        throw std::invalid_argument("Bellman-Ford requires at least one vertex.");
    }
    if (source >= vertexCount) {
        throw std::invalid_argument("Bellman-Ford source is outside the graph.");
    }
    for (const DirectedWeightedEdge& edge : edges) {
        if (edge.from >= vertexCount || edge.to >= vertexCount) {
            throw std::invalid_argument("Bellman-Ford edge endpoint is outside the graph.");
        }
    }
    if (vertexCount > std::vector<std::optional<std::int64_t>>().max_size() ||
        vertexCount > std::vector<std::optional<std::size_t>>().max_size()) {
        throw std::length_error("Bellman-Ford graph exceeds vector capacity.");
    }

    std::vector<std::optional<std::int64_t>> distances(vertexCount);
    std::vector<std::optional<std::size_t>> predecessors(vertexCount);
    distances[source] = 0;

    for (std::size_t iteration = 1; iteration < vertexCount; ++iteration) {
        bool changed = false;
        for (const DirectedWeightedEdge& edge : edges) {
            if (!distances[edge.from]) {
                continue;
            }
            const std::int64_t candidate =
                checkedPathSum(*distances[edge.from], edge.weight);
            if (!distances[edge.to] || candidate < *distances[edge.to]) {
                distances[edge.to] = candidate;
                predecessors[edge.to] = edge.from;
                changed = true;
            }
        }
        if (!changed) {
            break;
        }
    }

    for (const DirectedWeightedEdge& edge : edges) {
        if (!distances[edge.from]) {
            continue;
        }
        const std::int64_t candidate =
            checkedPathSum(*distances[edge.from], edge.weight);
        if (!distances[edge.to] || candidate < *distances[edge.to]) {
            throw std::domain_error("Bellman-Ford found a reachable negative-weight cycle.");
        }
    }

    return {source, std::move(distances), std::move(predecessors)};
}

} // namespace project31::algorithms
