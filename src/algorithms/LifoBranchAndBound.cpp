#include "algorithms/branch_bound/LifoBranchAndBound.hpp"

#include "BranchAndBoundKnapsackInternal.hpp"

#include <cstdint>
#include <vector>

namespace project31::algorithms {

BranchBoundKnapsackResult solveKnapsackLifoBranchAndBound(
    const std::vector<BranchBoundItem>& items,
    std::int64_t capacity
) {
    using namespace branch_bound_detail;
    const Problem problem = prepareProblem(items, capacity);
    std::vector<Node> liveNodes;
    liveNodes.push_back({0, 0, 0, {}});
    std::int64_t bestValue = 0;
    std::vector<std::size_t> bestIndices;

    while (!liveNodes.empty()) {
        Node current = std::move(liveNodes.back());
        liveNodes.pop_back();
        consider(current, bestValue, bestIndices);
        if (current.level == problem.order.size() ||
            !canImprove(problem, current, bestValue)) {
            continue;
        }

        const std::size_t nextItem = problem.order[current.level];
        liveNodes.push_back(excludedChild(current));
        if (items[nextItem].weight <= problem.capacity - current.weight) {
            liveNodes.push_back(includedChild(problem, current));
        }
    }

    return {bestValue, std::move(bestIndices)};
}

} // namespace project31::algorithms
