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

## Graph shortest paths

| Algorithm | Airport use | Time | Auxiliary space |
|---|---|---:|---:|
| Bellman-Ford | Single-source facility route costs with negative adjustments | O(VE) | O(V) |
| Floyd-Warshall | All-pairs route costs among airport facilities | O(V^3) | O(V^2) |

Both use directed weighted edges and checked `int64_t` distance arithmetic.
Bellman-Ford stores predecessor vertices and rejects a negative cycle only
when it is reachable from the selected source. Floyd-Warshall stores a
next-hop matrix and rejects any negative cycle. Unreachable pairs are
represented by empty optionals; path accessors return no path for those pairs.

## Network flow and matching

| Algorithm | Airport use | Time | Auxiliary space |
|---|---|---:|---:|
| Ford-Fulkerson (Edmonds-Karp BFS) | Maximize passenger/baggage throughput through checkpoint capacities | O(VE^2) | O(V + E) |
| Maximum Bipartite Matching | Assign officers to compatible screening lanes | O(L E) | O(L + R + E) |

Ford-Fulkerson stores a residual graph with a distinct reverse edge for each
input capacity edge, so parallel and antiparallel connections remain
independent. Each BFS finds a shortest augmenting path by edge count; the
bottleneck capacity is pushed along it, and reverse residual capacity permits
later paths to cancel and reroute earlier flow. When BFS cannot reach the
sink, the vertices reachable from the source define a cut whose capacity
equals the accumulated flow, establishing maximality. Per-input edge flows
are returned in input order. Zero capacities are permitted, negative
capacities are rejected, and an equal source/sink is explicitly defined to
produce zero flow. The BFS-based bound is O(VE^2); checked accumulation
rejects a maximum flow or residual capacity that cannot fit in `int64_t`.

Maximum Bipartite Matching uses DFS augmenting paths (the Kuhn algorithm).
For each officer, DFS searches allowed lanes and recursively reroutes any
currently matched officer to another lane. If no augmenting path remains,
the augmenting-path theorem guarantees the matching has maximum cardinality.
Each officer and lane appears at most once in the returned pairs. Duplicate
allowed edges are harmless and preserve deterministic input-order traversal.
The implementation takes O(L E) time and O(L + R + E) storage, where `L` and
`R` are partition sizes and `E` is the number of allowed assignment edges.

## Number theory

| Algorithm | Airport use | Time | Auxiliary space |
|---|---|---:|---:|
| Chinese Remainder Theorem | Reconcile periodic inspection and maintenance cycles | O(k log^2 M) | O(1) |

For `k` congruences and combined modulus bounded by `M`, CRT combines one
congruence at a time. It requires positive, pairwise-coprime moduli and
returns the normalized solution in `[0, M)`. Negative residues are normalized
with signed remainder. Each merge uses the extended Euclidean algorithm to
find the inverse of the accumulated modulus modulo the next modulus. Modular
multiplication uses add-and-double arithmetic, avoiding wide-integer compiler
extensions and overflow-prone products. Combined-modulus overflow raises an
overflow error. The empty system returns the identity solution `0 mod 1`.

## String matching

| Algorithm | Airport use | Worst-case time | Auxiliary space |
|---|---|---:|---:|
| Naive String Matching | Find flight codes or event markers in operational logs | O(nm) | O(1), excluding output |
| Rabin-Karp | Search log records with a rolling hash and collision verification | O(nm); expected O(n+m) | O(1), excluding output |

Both functions accept text then pattern, return every zero-based start index,
preserve overlaps, and compare `std::string` bytes (not Unicode code points).
An empty pattern matches all `n+1` boundaries, while a longer-than-text
pattern returns no matches. Naive matching compares each candidate window
directly. Rabin-Karp rolls a fixed-base modular hash from one window to the
next and verifies byte equality at every hash match, so collisions cannot
produce false matches. The deterministic hash operations use bounded
64-bit intermediates. Rabin-Karp is expected linear for typical inputs; with
many collisions/verified candidates its worst case is O(nm).

## Backtracking

| Algorithm | Airport use | Worst-case time | Auxiliary space |
|---|---|---:|---:|
| N-Queens | Enumerate non-attacking screening-position layouts | O(N!) | O(N) per search path, plus all returned solutions |
| Subset Sum | Select resource packages totaling an exact capacity | O(2^n) | O(n) recursion/result path |

N-Queens places one queen per row and backtracks over unoccupied columns and
diagonals. It returns every board, so output memory is necessarily
proportional to the number and size of solutions. Enumeration is capped at
N=12 to bound runtime and memory; N=0 is invalid. Subset Sum explores inclusion
and exclusion choices, pruning any selected value larger than the remaining
target. Nonnegative inputs enable this pruning and the capped total check
avoids overflow when quickly rejecting unreachable targets.

## Branch and bound

| Algorithm | Airport use | Worst-case time | Auxiliary space |
|---|---|---:|---:|
| FIFO Branch and Bound | Select valuable screening equipment with breadth-first live-node expansion | O(n 2^n) | O(n 2^n) |
| LIFO Branch and Bound | Select valuable screening equipment with depth-first live-node expansion | O(n 2^n) | O(n^2) |
| Least-Cost Branch and Bound | Select valuable screening equipment by best-first upper-bound priority | O(n 2^n) | O(n 2^n) |

All three solvers use 0/1 Knapsack decisions and a fractional-knapsack
relaxation as an admissible upper bound. Items are ordered deterministically
by exact value-to-weight ratio and identifier; original indices are preserved
in results. Each explored node computes an O(n) bound and carries an O(n)
choice path. FIFO expands the oldest live node from a queue and may retain
exponentially many such paths. LIFO expands the newest live node from a stack;
its live frontier has at most O(n) nodes, each carrying an O(n) path.
Least-Cost uses a priority queue ordered by descending upper bound, since this
is a maximization problem; insertion order breaks equal-bound ties
deterministically. Its best-first live frontier may also grow exponentially.
All three have exponential worst-case search and reject unrepresentable
feasible value sums. FIFO, LIFO, and Least-Cost can be compared on the same
equipment records through `AirportOperations`; they return optimal values
while their exploration order may produce different valid optimal selections.

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

## Dynamic programming

| Algorithm | Airport use | Time | Space |
|---|---|---:|---:|
| 0/1 Knapsack | Choose operational equipment upgrades within a fixed budget | O(nW) | O(nW) |
| Matrix Chain Multiplication | Minimize scalar operations in an analytics matrix pipeline | O(n^3) | O(n^2) |
| Longest Common Subsequence | Compare event and passenger-processing histories | O(mn) | O(mn) |
| Optimal Binary Search Tree | Arrange operational record lookups by access probability | O(n^3) | O(n^2) |
| Bounded Travelling Salesperson (Held-Karp) | Minimize a round trip through airport inspection locations | O(n^2 2^n) | O(n 2^n) |

For Knapsack, `n` is the item count and `W` is the integer capacity. The DP
table stores the best value using the first `i` items at capacity `c`; a
separate previous-item row ensures each item is used at most once.
Reconstruction walks the table backward and returns original zero-based item
indices. The implementation rejects negative input values and capacities,
mismatched arrays, table-size overflow, and an unrepresentable result.

For Matrix Chain Multiplication, `n` is the number of matrices. Each DP cell
stores the minimum cost for a contiguous subchain and a split table records
the selected parenthesization. Checked arithmetic rejects chains whose costs
cannot be represented by `int64_t`. Empty dimension input is an empty chain;
one matrix has zero multiplication cost.

Bounded TSP uses Held-Karp subset DP. A state records the set of visited
locations and the last location; the solver stores the least cost for that
state and a predecessor for route reconstruction. Optional matrix entries
represent missing directed edges, while zero is a valid edge cost. The
implementation supports asymmetric nonnegative costs, detects cost overflow,
and returns no tour when the required round trip is infeasible. It is capped
at 18 vertices: this bounds the exponential table to about 4.7 million
subset/endpoint states, limiting both runtime and memory to an explicit,
documented range. A one-location tour has cost zero and repeats that location
as its return.

The airport facade exposes the Knapsack choice as budgeted upgrade planning
and Matrix Chain as analytics-pipeline ordering. These outputs inform resource
planning only and do not alter mandatory passenger screening.

The LCS implementation stores a full prefix table and reconstructs one
longest common sequence; tied choices may yield any valid optimum. OBST uses
successful and unsuccessful search probabilities to minimize expected
comparison depth, stores chosen roots, and reconstructs a key-bearing tree.
Its keys must be strictly increasing, probability arrays must have `n` and
`n+1` entries, and all supplied probabilities must be finite values in `[0,1]`.
The supplied probabilities must sum to one within a small floating-point
tolerance.

## Airport application and safety

`AirportOperations` is the simulator-facing planning/compression facade.
Its examples show appointment selection, lane allocation, route planning,
infrastructure planning by both MST methods, event-log compression, budgeted
upgrade selection, analytics matrix-chain ordering, history comparison,
frequency-weighted record lookup, equipment selection with all three Branch
and Bound strategies, an inspection tour through service locations, checkpoint
throughput planning, officer-to-lane assignment, recurring maintenance
reconciliation, and flight-code/event-marker search in operational logs.
Planning priority and route cost do not bypass screening: screening policy is
outside these algorithms and remains mandatory for every passenger.

See `docs/ALGORITHM_EXAMPLES.md` for executable examples and commands. The
greedy demonstration can be run with `./build/project31_greedy_example`; the
DP demonstrations can be run with `./build/project31_dynamic_programming_example`,
`./build/project31_dynamic_programming_batch2_example`, and
`./build/bounded_tsp_example`. The Branch and Bound strategies are demonstrated
by `./build/branch_bound_example` and `./build/least_cost_branch_bound_example`.
