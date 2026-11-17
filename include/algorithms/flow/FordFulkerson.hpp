#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace project31::algorithms {

struct CapacityEdge {
    std::size_t from;
    std::size_t to;
    std::int64_t capacity;
};

struct FlowEdgeResult {
    std::size_t from;
    std::size_t to;
    std::int64_t capacity;
    std::int64_t flow;
};

struct FordFulkersonResult {
    std::int64_t maximumFlow;
    std::vector<FlowEdgeResult> edgeFlows;
};

[[nodiscard]] FordFulkersonResult fordFulkersonMaxFlow(
    std::size_t vertexCount,
    const std::vector<CapacityEdge>& edges,
    std::size_t source,
    std::size_t sink
);

} // namespace project31::algorithms
