# Algorithm Ownership and Implementation Evidence

This inventory covers the ten required algorithm groups and the project's
additional Chinese Remainder Theorem utility. The 34 implementation entries
count iterative and recursive Binary Search separately; both share one
header/source module. `WeightedGraphs.hpp` and
`KnapsackBranchAndBound.hpp` define shared data types, not additional
standalone algorithms.

No individual student names or allocation records were present in the
repository or supplied project materials. Therefore each owner is explicitly
pending; ownership assignment is an unresolved administrative item and is
not inferred from commit authorship.

| # | Algorithm | Header and source | Test evidence | Complexity reference | Individual student owner |
|---:|---|---|---|---|---|
| 1 | Merge Sort | `include/algorithms/sorting/MergeSort.hpp`; `src/algorithms/MergeSort.cpp` | `tests/unit/sorting_tests.cpp` | [Sorting and searching](../ALGORITHM_ANALYSIS.md#sorting-and-searching) | Pending |
| 2 | Quick Sort | `include/algorithms/sorting/QuickSort.hpp`; `src/algorithms/QuickSort.cpp` | `tests/unit/sorting_tests.cpp` | [Sorting and searching](../ALGORITHM_ANALYSIS.md#sorting-and-searching) | Pending |
| 3 | Iterative Binary Search | `include/algorithms/searching/BinarySearch.hpp`; `src/algorithms/BinarySearch.cpp` | `tests/unit/searching_tests.cpp` | [Sorting and searching](../ALGORITHM_ANALYSIS.md#sorting-and-searching) | Pending |
| 4 | Recursive Binary Search | `include/algorithms/searching/BinarySearch.hpp`; `src/algorithms/BinarySearch.cpp` | `tests/unit/searching_tests.cpp` | [Sorting and searching](../ALGORITHM_ANALYSIS.md#sorting-and-searching) | Pending |
| 5 | Maximum Subarray | `include/algorithms/divide_conquer/MaximumSubarray.hpp`; `src/algorithms/MaximumSubarray.cpp` | `tests/unit/divide_conquer_tests.cpp` | [Divide and conquer](../ALGORITHM_ANALYSIS.md#divide-and-conquer-algorithms) | Pending |
| 6 | Fast Exponentiation | `include/algorithms/divide_conquer/FastExponentiation.hpp`; `src/algorithms/FastExponentiation.cpp` | `tests/unit/divide_conquer_tests.cpp` | [Divide and conquer](../ALGORITHM_ANALYSIS.md#divide-and-conquer-algorithms) | Pending |
| 7 | Celebrity Problem | `include/algorithms/divide_conquer/CelebrityProblem.hpp`; `src/algorithms/CelebrityProblem.cpp` | `tests/unit/divide_conquer_tests.cpp` | [Divide and conquer](../ALGORITHM_ANALYSIS.md#divide-and-conquer-algorithms) | Pending |
| 8 | Classical Matrix Multiplication | `include/algorithms/divide_conquer/ClassicalMatrixMultiplication.hpp`; `src/algorithms/ClassicalMatrixMultiplication.cpp` | `tests/unit/divide_conquer_tests.cpp` | [Divide and conquer](../ALGORITHM_ANALYSIS.md#divide-and-conquer-algorithms) | Pending |
| 9 | Strassen Matrix Multiplication | `include/algorithms/divide_conquer/StrassenMatrixMultiplication.hpp`; `src/algorithms/StrassenMatrixMultiplication.cpp` | `tests/unit/divide_conquer_tests.cpp` | [Divide and conquer](../ALGORITHM_ANALYSIS.md#divide-and-conquer-algorithms) | Pending |
| 10 | Activity Selection | `include/algorithms/greedy/ActivitySelection.hpp`; `src/algorithms/ActivitySelection.cpp` | `tests/unit/greedy_tests.cpp` | [Greedy algorithms](../ALGORITHM_ANALYSIS.md#greedy-algorithms) | Pending |
| 11 | Interval Partitioning | `include/algorithms/greedy/IntervalPartitioning.hpp`; `src/algorithms/IntervalPartitioning.cpp` | `tests/unit/greedy_tests.cpp` | [Greedy algorithms](../ALGORITHM_ANALYSIS.md#greedy-algorithms) | Pending |
| 12 | Huffman Coding | `include/algorithms/greedy/HuffmanCoding.hpp`; `src/algorithms/HuffmanCoding.cpp` | `tests/unit/greedy_tests.cpp` | [Greedy algorithms](../ALGORITHM_ANALYSIS.md#greedy-algorithms) | Pending |
| 13 | Dijkstra | `include/algorithms/greedy/Dijkstra.hpp`; `src/algorithms/Dijkstra.cpp` | `tests/unit/greedy_tests.cpp` | [Greedy algorithms](../ALGORITHM_ANALYSIS.md#greedy-algorithms) | Pending |
| 14 | Prim | `include/algorithms/greedy/WeightedGraphs.hpp`; `src/algorithms/Prim.cpp` | `tests/unit/greedy_tests.cpp` | [Greedy algorithms](../ALGORITHM_ANALYSIS.md#greedy-algorithms) | Pending |
| 15 | Kruskal | `include/algorithms/greedy/WeightedGraphs.hpp`; `src/algorithms/Kruskal.cpp` | `tests/unit/greedy_tests.cpp` | [Greedy algorithms](../ALGORITHM_ANALYSIS.md#greedy-algorithms) | Pending |
| 16 | 0/1 Knapsack (dynamic programming) | `include/algorithms/dynamic_programming/Knapsack.hpp`; `src/algorithms/Knapsack.cpp` | `tests/unit/dynamic_programming_tests.cpp` | [Dynamic programming](../ALGORITHM_ANALYSIS.md#dynamic-programming) | Pending |
| 17 | Matrix Chain Multiplication | `include/algorithms/dynamic_programming/MatrixChainMultiplication.hpp`; `src/algorithms/MatrixChainMultiplication.cpp` | `tests/unit/dynamic_programming_tests.cpp` | [Dynamic programming](../ALGORITHM_ANALYSIS.md#dynamic-programming) | Pending |
| 18 | Longest Common Subsequence | `include/algorithms/dynamic_programming/LongestCommonSubsequence.hpp`; `src/algorithms/LongestCommonSubsequence.cpp` | `tests/unit/dynamic_programming_batch2_tests.cpp` | [Dynamic programming](../ALGORITHM_ANALYSIS.md#dynamic-programming) | Pending |
| 19 | Optimal Binary Search Tree | `include/algorithms/dynamic_programming/OptimalBinarySearchTree.hpp`; `src/algorithms/OptimalBinarySearchTree.cpp` | `tests/unit/dynamic_programming_batch2_tests.cpp` | [Dynamic programming](../ALGORITHM_ANALYSIS.md#dynamic-programming) | Pending |
| 20 | Bounded Travelling Salesperson (Held-Karp) | `include/algorithms/dynamic_programming/BoundedTravellingSalesperson.hpp`; `src/algorithms/BoundedTravellingSalesperson.cpp` | `tests/unit/bounded_tsp_tests.cpp` | [Dynamic programming](../ALGORITHM_ANALYSIS.md#dynamic-programming) | Pending |
| 21 | N-Queens | `include/algorithms/backtracking/NQueens.hpp`; `src/algorithms/NQueens.cpp` | `tests/unit/backtracking_tests.cpp` | [Backtracking](../ALGORITHM_ANALYSIS.md#backtracking) | Pending |
| 22 | Subset Sum | `include/algorithms/backtracking/SubsetSum.hpp`; `src/algorithms/SubsetSum.cpp` | `tests/unit/backtracking_tests.cpp` | [Backtracking](../ALGORITHM_ANALYSIS.md#backtracking) | Pending |
| 23 | FIFO Branch and Bound | `include/algorithms/branch_bound/FifoBranchAndBound.hpp`; `src/algorithms/FifoBranchAndBound.cpp` | `tests/unit/branch_bound_tests.cpp` | [Branch and bound](../ALGORITHM_ANALYSIS.md#branch-and-bound) | Pending |
| 24 | LIFO Branch and Bound | `include/algorithms/branch_bound/LifoBranchAndBound.hpp`; `src/algorithms/LifoBranchAndBound.cpp` | `tests/unit/branch_bound_tests.cpp` | [Branch and bound](../ALGORITHM_ANALYSIS.md#branch-and-bound) | Pending |
| 25 | Least-Cost Branch and Bound | `include/algorithms/branch_bound/LeastCostBranchAndBound.hpp`; `src/algorithms/LeastCostBranchAndBound.cpp` | `tests/unit/least_cost_branch_bound_tests.cpp` | [Branch and bound](../ALGORITHM_ANALYSIS.md#branch-and-bound) | Pending |
| 26 | Bellman-Ford | `include/algorithms/graph/BellmanFord.hpp`; `src/algorithms/BellmanFord.cpp` | `tests/unit/graph_algorithms_tests.cpp` | [Graph shortest paths](../ALGORITHM_ANALYSIS.md#graph-shortest-paths) | Pending |
| 27 | Floyd-Warshall | `include/algorithms/graph/FloydWarshall.hpp`; `src/algorithms/FloydWarshall.cpp` | `tests/unit/graph_algorithms_tests.cpp` | [Graph shortest paths](../ALGORITHM_ANALYSIS.md#graph-shortest-paths) | Pending |
| 28 | Ford-Fulkerson (Edmonds-Karp BFS) | `include/algorithms/flow/FordFulkerson.hpp`; `src/algorithms/FordFulkerson.cpp` | `tests/unit/flow_algorithms_tests.cpp` | [Network flow and matching](../ALGORITHM_ANALYSIS.md#network-flow-and-matching) | Pending |
| 29 | Maximum Bipartite Matching | `include/algorithms/flow/MaximumBipartiteMatching.hpp`; `src/algorithms/MaximumBipartiteMatching.cpp` | `tests/unit/flow_algorithms_tests.cpp` | [Network flow and matching](../ALGORITHM_ANALYSIS.md#network-flow-and-matching) | Pending |
| 30 | Naive String Matching | `include/algorithms/string_matching/NaiveStringMatching.hpp`; `src/algorithms/NaiveStringMatching.cpp` | `tests/unit/string_matching_tests.cpp` | [String matching](../ALGORITHM_ANALYSIS.md#string-matching) | Pending |
| 31 | Rabin-Karp | `include/algorithms/string_matching/RabinKarp.hpp`; `src/algorithms/RabinKarp.cpp` | `tests/unit/string_matching_tests.cpp` | [String matching](../ALGORITHM_ANALYSIS.md#string-matching) | Pending |
| 32 | Finite Automaton String Matching | `include/algorithms/string_matching/FiniteAutomatonStringMatching.hpp`; `src/algorithms/FiniteAutomatonStringMatching.cpp` | `tests/unit/string_matching_batch2_tests.cpp` | [String matching](../ALGORITHM_ANALYSIS.md#string-matching) | Pending |
| 33 | Knuth-Morris-Pratt (KMP) | `include/algorithms/string_matching/KnuthMorrisPratt.hpp`; `src/algorithms/KnuthMorrisPratt.cpp` | `tests/unit/string_matching_batch2_tests.cpp` | [String matching](../ALGORITHM_ANALYSIS.md#string-matching) | Pending |
| 34 | Chinese Remainder Theorem | `include/algorithms/number_theory/ChineseRemainderTheorem.hpp`; `src/algorithms/ChineseRemainderTheorem.cpp` | `tests/unit/chinese_remainder_theorem_tests.cpp` | [Number theory](../ALGORITHM_ANALYSIS.md#number-theory) | Pending |

The required ten groups are sorting, searching, divide-and-conquer, greedy,
dynamic programming, backtracking, branch-and-bound, graph, flow, and string
matching. The CRT utility is an additional implemented number-theory module.
The inventory contains 34 implementation entries, including all three
branch-and-bound variants. Unit-test locations above are registered in
`CMakeLists.txt`; the complete current suite is reported in
`DEVELOPMENT_STATUS.md`.
