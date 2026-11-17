#include "algorithms/greedy/Kruskal.hpp"

#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace project31::algorithms {
namespace {

bool before(const UndirectedWeightedEdge& left, const UndirectedWeightedEdge& right) {
    if (left.weight != right.weight) {
        return left.weight < right.weight;
    }
    if (left.first != right.first) {
        return left.first < right.first;
    }
    return left.second < right.second;
}

void mergeSort(std::vector<UndirectedWeightedEdge>& edges,
               std::vector<UndirectedWeightedEdge>& buffer,
               std::size_t begin, std::size_t end) {
    if (end - begin < 2) {
        return;
    }
    const std::size_t middle = begin + (end - begin) / 2;
    mergeSort(edges, buffer, begin, middle);
    mergeSort(edges, buffer, middle, end);
    std::size_t left = begin;
    std::size_t right = middle;
    std::size_t out = begin;
    while (left < middle && right < end) {
        buffer[out++] = before(edges[right], edges[left])
            ? edges[right++] : edges[left++];
    }
    while (left < middle) {
        buffer[out++] = edges[left++];
    }
    while (right < end) {
        buffer[out++] = edges[right++];
    }
    for (std::size_t index = begin; index < end; ++index) {
        edges[index] = buffer[index];
    }
}

class DisjointSet {
public:
    explicit DisjointSet(std::size_t size) : parent_(size), rank_(size, 0) {
        for (std::size_t index = 0; index < size; ++index) {
            parent_[index] = index;
        }
    }

    std::size_t find(std::size_t value) {
        if (parent_[value] != value) {
            parent_[value] = find(parent_[value]);
        }
        return parent_[value];
    }

    bool unite(std::size_t left, std::size_t right) {
        left = find(left);
        right = find(right);
        if (left == right) {
            return false;
        }
        if (rank_[left] < rank_[right]) {
            std::swap(left, right);
        }
        parent_[right] = left;
        if (rank_[left] == rank_[right]) {
            ++rank_[left];
        }
        return true;
    }

private:
    std::vector<std::size_t> parent_;
    std::vector<std::size_t> rank_;
};

std::int64_t checkedAdd(std::int64_t left, std::int64_t right) {
    if ((right > 0 && left > std::numeric_limits<std::int64_t>::max() - right) ||
        (right < 0 && left < std::numeric_limits<std::int64_t>::min() - right)) {
        throw std::overflow_error("Spanning forest weight exceeds int64 range.");
    }
    return left + right;
}

} // namespace

SpanningForest kruskalMinimumSpanningForest(
    std::size_t vertexCount,
    std::vector<UndirectedWeightedEdge> edges
) {
    for (const UndirectedWeightedEdge& edge : edges) {
        if (edge.first >= vertexCount || edge.second >= vertexCount) {
            throw std::invalid_argument("Kruskal edge endpoint is outside the graph.");
        }
    }
    if (edges.size() > 1) {
        std::vector<UndirectedWeightedEdge> buffer(edges.size());
        mergeSort(edges, buffer, 0, edges.size());
    }

    DisjointSet sets(vertexCount);
    SpanningForest forest{vertexCount, 0, {}};
    for (const UndirectedWeightedEdge& edge : edges) {
        if (sets.unite(edge.first, edge.second)) {
            forest.edges.push_back(edge);
            forest.totalWeight = checkedAdd(forest.totalWeight, edge.weight);
            --forest.componentCount;
        }
    }
    return forest;
}

} // namespace project31::algorithms
