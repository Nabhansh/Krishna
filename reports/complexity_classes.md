# Complexity-Class Analysis

This note classifies the implemented problem-solving approaches by their
documented worst-case resource bounds. It is not a proof of implementation
correctness; see the algorithm tests and per-algorithm analysis for that.

## Polynomial-time implementations

The following implementations have polynomial worst-case bounds in their
explicit input sizes:

- Merge Sort, Binary Search, and the graph shortest-path and spanning-forest
  algorithms.
- Ford-Fulkerson using BFS augmenting paths (Edmonds-Karp), with
  `O(V E^2)` time.
- Maximum Bipartite Matching using DFS augmenting paths, with `O(L E)` time.
- Naive, Rabin-Karp, finite-automaton, and KMP string matching. Naive and
  Rabin-Karp have `O(nm)` worst-case time; the DFA and KMP have linear
  scanning time after preprocessing.
- Activity Selection, Interval Partitioning, Huffman Coding, and the
  divide-and-conquer matrix/subarray routines.
- Bellman-Ford, Floyd-Warshall, Dijkstra, Prim, Kruskal, and the CRT utility.

Quick Sort has `O(n log n)` average time but `O(n^2)` worst-case time; it is
not a deterministic polynomial-time bound of `O(n log n)`.

## Pseudo-polynomial and exponential implementations

- Bottom-up 0/1 Knapsack takes `O(nW)` time and space. This is
  pseudo-polynomial in the numeric capacity `W`, whose binary representation
  has only `O(log W)` bits. The optimization problem is NP-hard.
- FIFO, LIFO, and Least-Cost Branch and Bound solve the same Knapsack
  optimization problem and retain exponential worst-case time; pruning does
  not change the worst-case class.
- Subset Sum uses include/exclude backtracking and has `O(2^n)` worst-case
  time. The decision version is NP-complete.
- Bounded Travelling Salesperson uses Held-Karp subset dynamic programming in
  `O(n^2 2^n)` time and `O(n 2^n)` space. The general TSP optimization problem
  is NP-hard; the implementation has a documented size cap.
- N-Queens enumerates solutions with factorial worst-case search and output
  storage proportional to the returned solutions. It is an enumeration
  problem, not an implementation of a polynomial decision solver.

## Interpretation

For NP-hard optimization problems, an exact algorithm can still be useful on
small airport planning instances while having exponential worst-case cost.
Branch-and-bound ordering and capacity caps are practical controls, not
polynomial-time guarantees. The airport scenario comparison itself is
polynomial in the passenger count and staffed lane count for a fixed small
number of outages; its detailed bound is in `complexity_report.md`.

Problem-class labels describe the mathematical problem, not the domain
application. Airport examples are planning/analysis demonstrations and do not
authorize a passenger to bypass required screening.
