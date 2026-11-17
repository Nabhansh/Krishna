#include "simulator/AirportOperations.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace {

void printTree(const project31::algorithms::OptimalBinarySearchTreeNode* node) {
    if (node == nullptr) {
        return;
    }
    std::cout << node->key << '\n';
    printTree(node->left.get());
    printTree(node->right.get());
}

} // namespace

int main() {
    const auto commonHistory = project31::AirportOperations::compareOperationalEventHistories(
        "ARRIVE SCREEN QUEUE BOARD",
        "ARRIVE QUEUE SCREEN BOARD"
    );
    std::cout << "Common event-history subsequence (length "
              << commonHistory.length << "): "
              << commonHistory.subsequence << '\n';

    const auto lookup = project31::AirportOperations::buildOperationalLookupTree(
        {"event-arrival", "event-board", "event-screen"},
        {0.3, 0.2, 0.3},
        {0.05, 0.05, 0.05, 0.05}
    );
    std::cout << "Expected operational lookup cost: "
              << lookup.expectedCost << '\n'
              << "Lookup tree in preorder:\n";
    printTree(lookup.root.get());
    return 0;
}
