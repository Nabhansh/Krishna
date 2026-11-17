#include "algorithms/branch_bound/LeastCostBranchAndBound.hpp"

#include "BranchAndBoundKnapsackInternal.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace project31::algorithms {
namespace {

struct LiveNode {
    branch_bound_detail::Node state;
    long double upperBound;
    std::size_t sequence;
};

struct LessPromisingNode {
    bool operator()(const LiveNode& left, const LiveNode& right) const {
        if (left.upperBound != right.upperBound) {
            return left.upperBound < right.upperBound;
        }
        return left.sequence > right.sequence;
    }
};

} // namespace

BranchBoundKnapsackResult solveKnapsackLeastCostBranchAndBound(
    const std::vector<BranchBoundItem>& items,
    std::int64_t capacity
) {
    using namespace branch_bound_detail;
    const Problem problem = prepareProblem(items, capacity);
    std::priority_queue<LiveNode, std::vector<LiveNode>, LessPromisingNode> liveNodes;
    std::size_t nextSequence = 0;
    liveNodes.push({{0, 0, 0, {}}, upperBound(problem, {0, 0, 0, {}}), nextSequence++});
    std::int64_t bestValue = 0;
    std::vector<std::size_t> bestIndices;

    while (!liveNodes.empty()) {
        LiveNode current = liveNodes.top();
        liveNodes.pop();
        consider(current.state, bestValue, bestIndices);
        if (current.state.level == problem.order.size() ||
            current.upperBound <= static_cast<long double>(bestValue)) {
            continue;
        }

        const std::size_t itemIndex = problem.order[current.state.level];
        if (items[itemIndex].weight <= problem.capacity - current.state.weight) {
            Node included = includedChild(problem, current.state);
            const long double bound = upperBound(problem, included);
            if (bound > static_cast<long double>(bestValue)) {
                if (nextSequence == std::numeric_limits<std::size_t>::max()) {
                    throw std::length_error("Least-cost branch-and-bound queue is too large.");
                }
                liveNodes.push({std::move(included), bound, nextSequence++});
            }
        }

        Node excluded = excludedChild(current.state);
        const long double bound = upperBound(problem, excluded);
        if (bound > static_cast<long double>(bestValue)) {
            if (nextSequence == std::numeric_limits<std::size_t>::max()) {
                throw std::length_error("Least-cost branch-and-bound queue is too large.");
            }
            liveNodes.push({std::move(excluded), bound, nextSequence++});
        }
    }

    return {bestValue, std::move(bestIndices)};
}

} // namespace project31::algorithms
