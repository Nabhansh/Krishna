#include "algorithms/flow/FordFulkerson.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace project31::algorithms {
namespace {

struct ResidualEdge {
    std::size_t from;
    std::size_t to;
    std::size_t reverseIndex;
    std::int64_t residualCapacity;
    std::size_t originalEdgeIndex;
    bool forward;
};

} // namespace

FordFulkersonResult fordFulkersonMaxFlow(
    std::size_t vertexCount,
    const std::vector<CapacityEdge>& edges,
    std::size_t source,
    std::size_t sink
) {
    if (vertexCount == 0) {
        throw std::invalid_argument("Maximum flow requires at least one vertex.");
    }
    if (source >= vertexCount || sink >= vertexCount) {
        throw std::invalid_argument("Maximum-flow source or sink is outside the graph.");
    }
    if (vertexCount > std::vector<std::vector<std::size_t>>().max_size() ||
        edges.size() > std::vector<ResidualEdge>().max_size() / 2 ||
        edges.size() > std::vector<FlowEdgeResult>().max_size()) {
        throw std::length_error("Maximum-flow graph exceeds container capacity.");
    }
    for (const CapacityEdge& edge : edges) {
        if (edge.from >= vertexCount || edge.to >= vertexCount) {
            throw std::invalid_argument("Maximum-flow edge endpoint is outside the graph.");
        }
        if (edge.capacity < 0) {
            throw std::invalid_argument("Maximum-flow capacities must not be negative.");
        }
    }

    if (source == sink) {
        std::vector<FlowEdgeResult> edgeFlows;
        edgeFlows.reserve(edges.size());
        for (const CapacityEdge& edge : edges) {
            edgeFlows.push_back({edge.from, edge.to, edge.capacity, 0});
        }
        return {0, std::move(edgeFlows)};
    }

    std::vector<ResidualEdge> residualEdges;
    residualEdges.reserve(edges.size() * 2);
    std::vector<std::vector<std::size_t>> adjacency(vertexCount);
    for (std::size_t index = 0; index < edges.size(); ++index) {
        const CapacityEdge& edge = edges[index];
        const std::size_t forwardIndex = residualEdges.size();
        const std::size_t reverseIndex = forwardIndex + 1;
        residualEdges.push_back(
            {edge.from, edge.to, reverseIndex, edge.capacity, index, true}
        );
        residualEdges.push_back(
            {edge.to, edge.from, forwardIndex, 0, index, false}
        );
        adjacency[edge.from].push_back(forwardIndex);
        adjacency[edge.to].push_back(reverseIndex);
    }

    std::int64_t maximumFlow = 0;
    const std::size_t noEdge = std::numeric_limits<std::size_t>::max();
    std::vector<std::size_t> parentEdge(vertexCount);
    std::vector<bool> visited(vertexCount);

    while (true) {
        std::fill(visited.begin(), visited.end(), false);
        std::fill(parentEdge.begin(), parentEdge.end(), noEdge);
        std::queue<std::size_t> pending;
        visited[source] = true;
        pending.push(source);

        while (!pending.empty() && !visited[sink]) {
            const std::size_t current = pending.front();
            pending.pop();
            for (std::size_t edgeIndex : adjacency[current]) {
                const ResidualEdge& edge = residualEdges[edgeIndex];
                if (edge.residualCapacity <= 0 || visited[edge.to]) {
                    continue;
                }
                visited[edge.to] = true;
                parentEdge[edge.to] = edgeIndex;
                pending.push(edge.to);
                if (edge.to == sink) {
                    break;
                }
            }
        }
        if (!visited[sink]) {
            break;
        }

        std::int64_t augmentation = std::numeric_limits<std::int64_t>::max();
        for (std::size_t vertex = sink; vertex != source;) {
            const std::size_t edgeIndex = parentEdge[vertex];
            augmentation = std::min(
                augmentation,
                residualEdges[edgeIndex].residualCapacity
            );
            vertex = residualEdges[edgeIndex].from;
        }
        if (maximumFlow > std::numeric_limits<std::int64_t>::max() - augmentation) {
            throw std::overflow_error("Maximum flow exceeds int64 range.");
        }

        for (std::size_t vertex = sink; vertex != source;) {
            const std::size_t edgeIndex = parentEdge[vertex];
            ResidualEdge& edge = residualEdges[edgeIndex];
            ResidualEdge& reverse = residualEdges[edge.reverseIndex];
            if (reverse.residualCapacity >
                std::numeric_limits<std::int64_t>::max() - augmentation) {
                throw std::overflow_error("Residual capacity exceeds int64 range.");
            }
            edge.residualCapacity -= augmentation;
            reverse.residualCapacity += augmentation;
            vertex = edge.from;
        }
        maximumFlow += augmentation;
    }

    std::vector<std::int64_t> flows(edges.size(), 0);
    for (const ResidualEdge& edge : residualEdges) {
        if (edge.forward) {
            flows[edge.originalEdgeIndex] =
                edges[edge.originalEdgeIndex].capacity - edge.residualCapacity;
        }
    }

    std::vector<FlowEdgeResult> edgeFlows;
    edgeFlows.reserve(edges.size());
    for (std::size_t index = 0; index < edges.size(); ++index) {
        edgeFlows.push_back({
            edges[index].from,
            edges[index].to,
            edges[index].capacity,
            flows[index]
        });
    }
    return {maximumFlow, std::move(edgeFlows)};
}

} // namespace project31::algorithms
