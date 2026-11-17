# Algorithm Analysis

All implementations are independent C++20 code; no STL sort/search routines
are used as substitutes. Graph algorithms use standard containers for
representation, while Kruskal uses a separately implemented DSU and hand-coded
merge sort for edge ordering.

## Greedy algorithms

| Algorithm | Airport use | Time | Auxiliary space |
|---|---|---:|---:|
| Activity Selection | Select the largest set of nonoverlapping screening appointments | O(n log n) | O(n) |
| Interval Partitioning | Assign overlapping screening intervals to the fewest lanes | O(n log n) | O(n) |
| Huffman Coding | Encode/decode event log text by observed symbol frequencies | Build O(k log k); encode O(mℓ); decode O(b) | O(kℓ) tree/codes |
| Dijkstra | Route planning with nonnegative route costs | O(V^2 + E) | O(V + E) |
| Prim | Minimum-cost airport infrastructure spanning forest | O(V^2 + E) | O(V + E) |
| Kruskal | Independent infrastructure spanning forest using DSU | O(E log E + E alpha(V)) | O(V + E) |

`n` is the schedule size, `k` the number of unique symbols, `m` the log
length, and `V`/`E` the graph vertex/edge counts. Dijkstra is an adjacency-list
implementation with a linear minimum-vertex selection, hence O(V^2+E).
Prim also uses linear selection and returns a minimum spanning forest for
disconnected graphs. Kruskal sorts edges with an independently implemented
merge sort and returns the same forest contract.

`ℓ` is the maximum codeword length and `b` is the encoded bit count. The
returned encoded/decoded string requires output space proportional to its
length in addition to the O(kℓ) stored codewords and O(k) tree nodes.

### Correctness notes

- Activity Selection sorts by finishing time and greedily accepts the next
  interval whose start is at least the last accepted finish. The earliest
  finishing compatible activity leaves maximal remaining time.
- Interval Partitioning processes intervals in start order and reuses the
  resource with earliest finish whenever it is available. The number of
  simultaneously active intervals is a lower bound on resources; the heap
  policy uses exactly that number.
- Huffman repeatedly combines the two least-frequency nodes. Deterministic
  symbol tie-breaking makes code generation reproducible. Tree paths produce
  prefix-free codes; the single-symbol alphabet uses code `0`.
- Dijkstra finalizes the nearest unvisited reachable vertex. Its nonnegative
  weight validation is required for this greedy invariant.
- Prim grows a minimum tree per connected component by the lightest crossing
  edge. Kruskal accepts exactly edges joining distinct DSU components.

## Divide and conquer algorithms

| Algorithm | Time | Auxiliary space |
|---|---:|---:|
| Maximum Subarray | O(n log n) | O(log n) call stack |
| Fast Exponentiation | O(log |e|) | O(1) |
| Celebrity Problem | O(n) | O(1) |
| Classical Matrix Multiplication | O(rck) | O(rk) output |
| Strassen Matrix Multiplication | O(n^log2(7)) padded square | O(n^2) |

Maximum Subarray combines optimal left, right, and crossing ranges. Fast
Exponentiation squares the base and halves the exponent. Celebrity elimination
removes at least one candidate per comparison and verifies the survivor.
Strassen pads rectangular compatible matrices to a power-of-two square and
uses classical multiplication for small subproblems.

## Airport application and safety

`AirportOperations` is the simulator-facing planning/compression facade.
Its examples show appointment selection, lane allocation, route planning,
infrastructure planning by both MST methods, and event-log compression.
Planning priority and route cost do not bypass screening: screening policy is
outside these algorithms and remains mandatory for every passenger.

See `docs/ALGORITHM_EXAMPLES.md` for executable examples and commands. The
greedy demonstration can be run with `./build/project31_greedy_example`.
