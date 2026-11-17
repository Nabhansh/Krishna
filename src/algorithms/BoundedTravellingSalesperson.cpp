#include "algorithms/dynamic_programming/BoundedTravellingSalesperson.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace project31::algorithms {
namespace {

std::int64_t checkedCostSum(std::int64_t left, std::int64_t right) {
    if (left > std::numeric_limits<std::int64_t>::max() - right) {
        throw std::overflow_error("Travelling Salesperson tour cost exceeds int64 range.");
    }
    return left + right;
}

} // namespace

std::optional<BoundedTravellingSalespersonResult>
solveBoundedTravellingSalesperson(
    const std::vector<std::vector<std::optional<std::int64_t>>>& costs,
    std::size_t startVertex
) {
    const std::size_t vertexCount = costs.size();
    if (vertexCount == 0) {
        throw std::invalid_argument("Travelling Salesperson requires at least one vertex.");
    }
    if (vertexCount > kMaximumHeldKarpVertices) {
        throw std::length_error(
            "Held-Karp Travelling Salesperson is limited to 18 vertices."
        );
    }
    if (startVertex >= vertexCount) {
        throw std::invalid_argument("Travelling Salesperson start vertex is invalid.");
    }
    for (const auto& row : costs) {
        if (row.size() != vertexCount) {
            throw std::invalid_argument(
                "Travelling Salesperson cost matrix must be square."
            );
        }
        for (const auto& edgeCost : row) {
            if (edgeCost && *edgeCost < 0) {
                throw std::invalid_argument(
                    "Travelling Salesperson edge costs must be nonnegative."
                );
            }
        }
    }

    if (vertexCount == 1) {
        return BoundedTravellingSalespersonResult{0, {startVertex, startVertex}};
    }

    const std::size_t subsetCount = std::size_t{1} << vertexCount;
    if (subsetCount > std::numeric_limits<std::size_t>::max() / vertexCount) {
        throw std::length_error("Held-Karp table dimensions overflow.");
    }
    const std::size_t cellCount = subsetCount * vertexCount;
    std::vector<std::optional<std::int64_t>> bestCost(cellCount);
    std::vector<std::int8_t> parent(cellCount, static_cast<std::int8_t>(-1));
    const auto cell = [vertexCount](std::size_t subset, std::size_t endpoint) {
        return subset * vertexCount + endpoint;
    };

    const std::size_t startBit = std::size_t{1} << startVertex;
    bestCost[cell(startBit, startVertex)] = 0;
    for (std::size_t subset = 0; subset < subsetCount; ++subset) {
        if ((subset & startBit) == 0) {
            continue;
        }
        for (std::size_t endpoint = 0; endpoint < vertexCount; ++endpoint) {
            const auto currentCost = bestCost[cell(subset, endpoint)];
            if (!currentCost) {
                continue;
            }
            for (std::size_t next = 0; next < vertexCount; ++next) {
                const std::size_t nextBit = std::size_t{1} << next;
                if ((subset & nextBit) != 0 || !costs[endpoint][next]) {
                    continue;
                }
                const std::size_t nextSubset = subset | nextBit;
                const std::int64_t candidate =
                    checkedCostSum(*currentCost, *costs[endpoint][next]);
                auto& knownCost = bestCost[cell(nextSubset, next)];
                if (!knownCost || candidate < *knownCost) {
                    knownCost = candidate;
                    parent[cell(nextSubset, next)] = static_cast<std::int8_t>(endpoint);
                }
            }
        }
    }

    const std::size_t fullSet = subsetCount - 1;
    std::optional<std::int64_t> optimalTourCost;
    std::size_t finalEndpoint = vertexCount;
    for (std::size_t endpoint = 0; endpoint < vertexCount; ++endpoint) {
        if (endpoint == startVertex || !bestCost[cell(fullSet, endpoint)] ||
            !costs[endpoint][startVertex]) {
            continue;
        }
        const std::int64_t candidate = checkedCostSum(
            *bestCost[cell(fullSet, endpoint)],
            *costs[endpoint][startVertex]
        );
        if (!optimalTourCost || candidate < *optimalTourCost) {
            optimalTourCost = candidate;
            finalEndpoint = endpoint;
        }
    }
    if (!optimalTourCost) {
        return std::nullopt;
    }

    std::vector<std::size_t> reversePath;
    std::size_t subset = fullSet;
    std::size_t endpoint = finalEndpoint;
    while (endpoint != startVertex) {
        reversePath.push_back(endpoint);
        const std::int8_t previous = parent[cell(subset, endpoint)];
        if (previous < 0) {
            throw std::logic_error("Held-Karp parent chain is incomplete.");
        }
        subset ^= std::size_t{1} << endpoint;
        endpoint = static_cast<std::size_t>(previous);
    }

    std::vector<std::size_t> tour;
    tour.reserve(vertexCount + 1);
    tour.push_back(startVertex);
    for (auto position = reversePath.rbegin(); position != reversePath.rend(); ++position) {
        tour.push_back(*position);
    }
    tour.push_back(startVertex);
    return BoundedTravellingSalespersonResult{*optimalTourCost, std::move(tour)};
}

} // namespace project31::algorithms
