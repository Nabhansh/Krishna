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

## Checkpoint throughput and officer-to-lane assignment

```sh
cmake --build build --target ford_fulkerson_example bipartite_matching_example
./build/ford_fulkerson_example
./build/bipartite_matching_example
```

The flow example models directed capacity between four screening checkpoints.
It reports a maximum throughput of 12 units and the flow on each connection.
Ford-Fulkerson with BFS finds augmenting paths in the residual network;
per-edge output includes the original capacity and its final assigned flow.
This is a capacity-planning result, not a shortest route, a minimum spanning
tree, or a change to screening rules.

Example flow output:

```text
Maximum checkpoint throughput: 12
Connection flows:
  0 -> 1: 7/8
  0 -> 2: 5/5
  1 -> 2: 2/3
  1 -> 3: 5/5
  2 -> 3: 7/7
```

The matching example gives three officers their compatible lanes. It reports
three assignments, including a rerouting along an augmenting path to avoid
leaving an officer unmatched. Matching only represents compatibility and
does not relax officer qualifications, lane requirements, or passenger
screening policies.

Example matching output:

```text
Officer-to-lane assignments (3):
  Officer 0 -> Lane 1
  Officer 1 -> Lane 0
  Officer 2 -> Lane 2
```

## Maintenance-cycle scheduling and operational-log search

```sh
cmake --build build --target crt_scheduling_example string_matching_example
./build/crt_scheduling_example
./build/string_matching_example
```

The scheduling example combines inspection cycles at day offsets 3 modulo 7
and 5 modulo 9. The next shared time is day 59, and the complete schedule
repeats every 63 days:

```text
Inspection and calibration cycles next align at day 59 (combined cycle 63 days).
```

CRT accepts positive pairwise-coprime periods and signed offsets; the returned
solution is normalized into the combined period. It rejects non-coprime
periods and any combined modulus that cannot fit in `int64_t`. An empty
schedule system has identity result `0 mod 1`.

The log example searches byte-oriented records for flight code `FLT-217`.
Naive matching and Rabin-Karp return the same zero-based offsets:

```text
Naive match offsets: 8 39
Rabin-Karp match offsets: 8 39
```

Both matchers include overlaps and treat an empty pattern as matching every
boundary. They search the exact bytes in `std::string`; they do not decode
Unicode. Rabin-Karp verifies equality after hash matches to rule out collision
false positives.

The benchmark compares results before timing:

```sh
cmake --build build --target project31_string_matching_benchmark
./build/project31_string_matching_benchmark
```

Datasets cover small, medium, large, and adversarial periodic text. A row is
reported only if both algorithms produce identical match positions.
