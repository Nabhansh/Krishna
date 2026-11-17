#include "algorithms/graph/FloydWarshall.hpp"

#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace project31::algorithms {
namespace {

using Distance = std::optional<std::int64_t>;
using NextHop = std::optional<std::size_t>;

std::int64_t checkedPathSum(std::int64_t left, std::int64_t right) {
    if ((right > 0 && left > std::numeric_limits<std::int64_t>::max() - right) ||
        (right < 0 && left < std::numeric_limits<std::int64_t>::min() - right)) {
        throw std::overflow_error("Floyd-Warshall path distance exceeds int64 range.");
    }
    return left + right;
}

} // namespace

std::optional<std::vector<std::size_t>> FloydWarshallResult::path(
    std::size_t from,
    std::size_t to
) const {
    const std::size_t vertexCount = distances.size();
    if (from >= vertexCount || to >= vertexCount) {
        throw std::out_of_range("Floyd-Warshall path endpoint is outside the graph.");
    }
    if (!distances[from][to]) {
        return std::nullopt;
    }

    std::vector<std::size_t> result{from};
    std::size_t current = from;
    for (std::size_t steps = 0; current != to && steps < vertexCount; ++steps) {
        if (current >= nextHops.size() || !nextHops[current][to]) {
            throw std::logic_error("Floyd-Warshall next-hop chain is incomplete.");
        }
        current = *nextHops[current][to];
        if (current >= vertexCount) {
            throw std::logic_error("Floyd-Warshall next hop is outside the graph.");
        }
        result.push_back(current);
    }
    if (current != to) {
        throw std::logic_error("Floyd-Warshall next-hop chain contains a cycle.");
    }
    return result;
}

FloydWarshallResult floydWarshallShortestPaths(
    std::size_t vertexCount,
    const std::vector<DirectedWeightedEdge>& edges
) {
    if (vertexCount > std::vector<std::vector<Distance>>().max_size() ||
        vertexCount > std::vector<std::vector<NextHop>>().max_size() ||
        vertexCount > std::vector<Distance>().max_size() ||
        vertexCount > std::vector<NextHop>().max_size() ||
        (vertexCount != 0 &&
         vertexCount > std::numeric_limits<std::size_t>::max() / vertexCount)) {
        throw std::length_error("Floyd-Warshall graph dimensions exceed vector capacity.");
    }
    for (const DirectedWeightedEdge& edge : edges) {
        if (edge.from >= vertexCount || edge.to >= vertexCount) {
            throw std::invalid_argument("Floyd-Warshall edge endpoint is outside the graph.");
        }
    }

    std::vector<std::vector<Distance>> distances(
        vertexCount,
        std::vector<Distance>(vertexCount)
    );
    std::vector<std::vector<NextHop>> nextHops(
        vertexCount,
        std::vector<NextHop>(vertexCount)
    );
    for (std::size_t vertex = 0; vertex < vertexCount; ++vertex) {
        distances[vertex][vertex] = 0;
        nextHops[vertex][vertex] = vertex;
    }
    for (const DirectedWeightedEdge& edge : edges) {
        if (!distances[edge.from][edge.to] ||
            edge.weight < *distances[edge.from][edge.to]) {
            distances[edge.from][edge.to] = edge.weight;
            nextHops[edge.from][edge.to] = edge.to;
        }
    }

    for (std::size_t intermediate = 0; intermediate < vertexCount; ++intermediate) {
        for (std::size_t from = 0; from < vertexCount; ++from) {
            if (!distances[from][intermediate]) {
                continue;
            }
            for (std::size_t to = 0; to < vertexCount; ++to) {
                if (!distances[intermediate][to]) {
                    continue;
                }
                const std::int64_t candidate = checkedPathSum(
                    *distances[from][intermediate],
                    *distances[intermediate][to]
                );
                if (!distances[from][to] || candidate < *distances[from][to]) {
                    distances[from][to] = candidate;
                    nextHops[from][to] = nextHops[from][intermediate];
                }
            }
        }
    }

    for (std::size_t vertex = 0; vertex < vertexCount; ++vertex) {
        if (distances[vertex][vertex] && *distances[vertex][vertex] < 0) {
            throw std::domain_error("Floyd-Warshall found a negative-weight cycle.");
        }
    }

    return {std::move(distances), std::move(nextHops)};
}

} // namespace project31::algorithms
