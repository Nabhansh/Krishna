# Algorithm Examples and Airport Mappings

Build and run the Batch A example:

```sh
cmake --build build --target project31_batch_a_example
./build/project31_batch_a_example
```

The example runs all five Batch A functions. It demonstrates an arrival-volume
deviation series with Maximum Subarray, a capacity scaling calculation with
Fast Exponentiation, a relationship matrix with Celebrity Problem, and the
same matrix product with classical and Strassen multiplication.

| Algorithm | Airport application |
|---|---|
| Maximum Subarray | Find the maximum contiguous interval of positive arrival-count deviations in an analytics window. |
| Fast Exponentiation | Estimate compounded capacity/scaling factors over repeated periods. |
| Celebrity Problem | Validate a directed relation-matrix exercise; it does not determine screening eligibility. |
| Classical Matrix Multiplication | Reference implementation for matrix-based capacity experiments. |
| Strassen Matrix Multiplication | Compare large padded capacity matrices with the classical implementation. |

These are analysis/planning mappings. No passenger may bypass a required
screening procedure based on risk category or algorithm output.

## Greedy airport operations

```sh
cmake --build build --target project31_greedy_example
./build/project31_greedy_example
```

The greedy example selects compatible appointment slots, assigns overlapping
flight screening intervals to lanes, computes nonnegative terminal-route
costs, compares Prim/Kruskal infrastructure forests, and round-trips an event
log with Huffman coding. Infrastructure weights represent planning cost and
route weights represent travel minutes; they are not screening risk scores.

## Dynamic-programming airport planning

```sh
cmake --build build --target project31_dynamic_programming_example
./build/project31_dynamic_programming_example
```

This demonstration chooses airport equipment upgrades under a fixed resource
budget with 0/1 Knapsack and selects the least-cost multiplication order for
an operational analytics matrix pipeline. The selected upgrades are planning
recommendations; they do not affect passenger screening requirements.

## Dynamic-programming batch 2

```sh
cmake --build build --target project31_dynamic_programming_batch2_example
./build/project31_dynamic_programming_batch2_example
```

This example finds a common subsequence between two security event histories
and constructs an optimal lookup tree for frequently accessed operational
record keys. The LCS operates on the input strings' characters. OBST keys
must be supplied in strictly increasing order; successful probabilities have
one value per key and unsuccessful probabilities have one extra boundary
value. Each probability must be finite and in `[0,1]`; callers provide
values whose total is one (within floating-point tolerance).

## Graph shortest-path planning

```sh
cmake --build build --target project31_graph_algorithms_example
./build/project31_graph_algorithms_example
```

The graph demonstration compares Bellman-Ford single-source route planning
with Floyd-Warshall all-pairs planning for airport facilities. Its negative
edge is a net operational cost adjustment, not negative travel time. Both
algorithms return route reconstruction data; Bellman-Ford rejects a negative
cycle reachable from its source, while Floyd-Warshall rejects any negative
cycle in the graph.

## Backtracking airport planning

```sh
cmake --build build --target backtracking_example
./build/backtracking_example
```

The example enumerates non-attacking screening-position layouts on a 4-by-4
board and selects resource-package indices whose capacities total exactly 10.
N-Queens enumerates all solutions and accepts board sizes only from 1 through
12. Subset Sum supports nonnegative package capacities and returns one set of
original input indices, or reports that no exact combination exists.

## Branch-and-bound equipment allocation

```sh
cmake --build build --target branch_bound_example
./build/branch_bound_example
```

The example selects screening equipment under one weight limit with both
FIFO (breadth-first) and LIFO (depth-first) branch-and-bound searches. Each
reports the best value and original package identifiers. Both use a
fractional-knapsack upper bound to prune states and are checked against the
project's dynamic-programming 0/1 Knapsack implementation in tests.

## Least-cost Branch and Bound and bounded inspection tour

```sh
cmake --build build --target least_cost_branch_bound_example bounded_tsp_example
./build/least_cost_branch_bound_example
./build/bounded_tsp_example
```

The equipment example compares FIFO, LIFO, and Least-Cost Branch and Bound on
the same screening equipment and weight limit. Least-Cost uses a priority
queue, expanding the live state with the highest fractional-knapsack upper
bound first; this is the promising direction for the shared maximization
objective. All strategies return maximum value and original selected indices.
Their worst-case search remains exponential, but the queue policies offer
different exploration and memory tradeoffs.

The inspection example uses Held-Karp dynamic programming to find a minimum
cost round trip through every listed airport service location. Its optional
matrix entries denote missing directed routes; zero is a valid route cost.
Asymmetric nonnegative costs are supported. The solver returns no tour for
infeasible route networks and supports at most 18 locations to bound its
`O(n 2^n)` memory and `O(n^2 2^n)` runtime.
