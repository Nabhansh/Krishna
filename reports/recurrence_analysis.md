# Recurrence Analysis

## 0/1 Knapsack

Let `K(i, c)` be the maximum value obtainable from the first `i` items with
capacity `c`. The bottom-up implementation evaluates:

```text
K(0, c) = 0
K(i, c) = K(i - 1, c)                                      if w_i > c
K(i, c) = max(K(i - 1, c), v_i + K(i - 1, c - w_i))        otherwise
```

The base case represents selecting no items. Every transition either excludes
item `i` or includes it and consults only row `i - 1`, enforcing 0/1 use.
There are `(n + 1)(W + 1)` states and constant work per state, giving
`O(nW)` time and `O(nW)` space including reconstruction.

## Matrix Chain Multiplication

Let `M[i,j]` be the minimum scalar multiplication count for matrices `Ai` to
`Aj`, and let `p` be the dimension vector where `Ai` has dimensions
`p[i-1] × p[i]`.

```text
M[i,i] = 0
M[i,j] = min over i <= k < j of
         M[i,k] + M[k+1,j] + p[i-1] * p[k] * p[j]
```

Each possible final split is evaluated once per interval. Increasing interval
length ensures both subproblems have already been computed. The split table
reconstructs an optimal parenthesization. The cubic loops give `O(n^3)` time;
cost and split tables use `O(n^2)` space.

## Longest Common Subsequence

Let `L[i,j]` be the LCS length of the first `i` characters of one string and
the first `j` characters of the other:

```text
L[0,j] = L[i,0] = 0
L[i,j] = L[i-1,j-1] + 1                         if x[i-1] == y[j-1]
L[i,j] = max(L[i-1,j], L[i,j-1])                otherwise
```

The table is filled by increasing prefixes. Backtracking from `(m,n)` follows
diagonal cells for matches and a maximum-valued neighboring cell otherwise,
reconstructing one valid LCS. The table takes `O(mn)` time and `O(mn)` space.

## Optimal Binary Search Tree

Use half-open key intervals `[i,j)`. Let `C[i,j]` be the minimum expected
search cost and `W[i,j]` the total probability mass in that interval,
including its unsuccessful-search boundaries:

```text
C[i,i] = W[i,i] = q[i]
W[i,j] = W[i,j-1] + p[j-1] + q[j]
C[i,j] = W[i,j] + min over i <= r < j of
         (C[i,r] + C[r+1,j])
```

The empty-interval base case charges its dummy-key probability at the current
depth. For every nonempty interval, each key is tried as root; saved split
indices reconstruct the minimizing tree. There are `O(n^2)` intervals, each
examining up to `n` roots, so time is `O(n^3)` and cost, weight, and root tables
use `O(n^2)` space. The expected cost is measured in comparisons, with root
depth one; input probabilities are validated to sum to one within a small
floating-point tolerance.

## Bellman-Ford

Let `D_k(v)` be the best distance to `v` after considering paths with at most
`k` edges. With source `s`:

```text
D_0(s) = 0
D_0(v) = infinity                                for v != s
D_k(v) = min(D_(k-1)(v),
             min over edges (u,v) of D_(k-1)(u) + w(u,v))
```

An optimal simple path uses at most `V-1` edges. The implementation relaxes
all edges for at most `V-1` passes, stopping when no distance changes, then
checks for a further reachable improvement to detect a reachable negative
cycle. This takes `O(VE)` time and `O(V)` auxiliary space, excluding the input
edge list.

## Floyd-Warshall

Let `D_k[i,j]` be the shortest path from `i` to `j` whose intermediate
vertices are drawn from the first `k` vertices:

```text
D_0[i,j] = minimum direct edge weight, or 0 for i == j, or infinity
D_k[i,j] = min(D_(k-1)[i,j],
               D_(k-1)[i,k] + D_(k-1)[k,j])
```

The intermediate-vertex loop computes the all-pairs table in `O(V^3)` time
and `O(V^2)` space. A negative diagonal entry after the iterations identifies
a negative cycle. The parallel next-hop matrix supports route reconstruction.
Both shortest-path implementations guard signed distance addition against
overflow.

## N-Queens

Let `Q(r, C, D1, D2)` enumerate valid placements from row `r` given occupied
columns and diagonals. At each row, each column not in the occupied sets is
tried; the recursive call advances to `r+1`, and the placement is undone on
return. A solution is emitted at `r=N`. The search tree has at most
`N * (N-1) * ... * 1 = N!` leaves and uses `O(N)` working space, exclusive of
the returned solutions. The implementation caps `N` at 12 because it
enumerates and retains every board.

## Subset Sum

For position `i` and remaining target `t`, backtracking tries:

```text
S(i, t) = S(i+1, t - a[i]) or S(i+1, t), if a[i] <= t
S(i, 0) = true
S(n, t) = false, for t > 0
```

Nonnegative values permit skipping a value larger than the remaining target.
The search stops on the first solution and carries original indices along the
include branch. In the worst case, inclusion/exclusion visits `O(2^n)` states
with `O(n)` recursive stack and selected-index storage. A preliminary sum
check is capped at the target, so the feasibility check itself cannot
overflow.

## FIFO Branch and Bound (0/1 Knapsack)

Each state records the next item position, accumulated weight/value, and
selected original indices. It creates include/exclude children when feasible.
The admissible bound adds the maximum value of a fractional relaxation of the
remaining items, considered in nonincreasing value-to-weight ratio:

```text
U(node) = node.value
         + value of the best fractional fill of (capacity - node.weight)
best = max(best, node.value)
expand node only when U(node) > best
```

FIFO stores live states in a queue and pops from the front, yielding
breadth-first state-space exploration. Up to `O(2^n)` states can be retained;
bound computation and state path copying cost `O(n)` per state, so worst-case
time and space are `O(n 2^n)`.

## LIFO Branch and Bound (0/1 Knapsack)

The state definition, include/exclude transitions, incumbent, and admissible
fractional bound are the same as FIFO Branch and Bound. LIFO stores live
states in a stack, pushes the exclude child before the include child, and pops
from the back so the include branch is explored immediately. It explores the
same exponential search tree depth-first. Each of at most `O(n)` live stack
states carries an `O(n)` selected-index path; bound calculation across all
states is `O(n 2^n)` time, while live-state storage is `O(n^2)`.

## Least-Cost Branch and Bound (0/1 Knapsack)

The state, include/exclude transitions, incumbent, and fractional-knapsack
upper bound are the same as for FIFO and LIFO. The live state is selected by
best-first search:

```text
next = live node with maximum upper bound
if upperBound(next) <= best: discard it
otherwise evaluate and expand its feasible include/exclude children
```

Because this is a maximization problem, the priority queue removes the largest
upper bound first. A monotonically increasing insertion sequence resolves
equal bounds reproducibly. The bound is admissible: relaxing the remaining
0/1 choices to permit a fractional final item cannot underestimate any
integral completion, so pruning a state whose bound cannot exceed the
incumbent preserves an optimal solution. In the worst case, up to `O(2^n)`
states are generated and each bound/path operation is `O(n)`, giving
`O(n 2^n)` time and `O(n 2^n)` live-state storage. Best-first ordering can
find strong incumbents early but does not improve the exponential worst case.

## Bounded Travelling Salesperson (Held-Karp)

For a fixed start vertex `s`, let `D[S,j]` be the minimum cost of a path
starting at `s`, visiting exactly the vertex subset `S`, and ending at `j`.
The start is in `S`; absent directed edges have no transition:

```text
D[{s}, s] = 0
D[S union {j}, j] = min over i in S, edge(i,j) exists of
                    D[S,i] + cost(i,j)
tourCost = min over j != s, edge(j,s) exists of
           D[allVertices,j] + cost(j,s)
```

The implementation stores the predecessor attaining each minimum and walks
those entries backward to reconstruct a closed tour. This subset-state
recurrence explores every visit order compactly instead of enumerating `n!`
permutations. There are `O(n 2^n)` subset/endpoint states and up to `O(n)`
predecessors per state, so time is `O(n^2 2^n)` and space is `O(n 2^n)`.
The solver accepts directed/asymmetric nonnegative costs, represents missing
edges with empty optionals (not zero), checks additions for overflow, and
returns no result if no Hamiltonian cycle through the specified start exists.
The explicit 18-vertex cap limits the DP to about 4.7 million states. For one
vertex, the tour is `[s,s]` with zero cost.

## Ford-Fulkerson Maximum Flow (Edmonds-Karp)

For each BFS augmenting path `P` in the residual graph, let
`delta = min residualCapacity(e)` over edges on `P`. Update every forward
residual edge by `-delta` and its paired reverse edge by `+delta`:

```text
residual(e)       = residual(e) - delta
residual(reverse) = residual(reverse) + delta
maximumFlow       = maximumFlow + delta
```

The BFS path is shortest in number of edges. Reverse residual edges allow the
search to undo a previous assignment and use a different route, preserving
optimality. Edmonds-Karp performs O(VE) augmentations; each BFS and path
update costs O(E), so the worst-case time is O(VE^2) and residual storage is
O(V+E). Each original directed edge has its own residual pair, including
parallel edges. Per-edge flow is original capacity minus its forward residual
capacity. The implementation validates terminals/endpoints and nonnegative
capacities and checks flow accumulation for `int64_t` overflow. Equal source
and sink is defined to return zero flow.

## Maximum Bipartite Matching (DFS augmenting paths)

Let `M[r]` be the officer currently matched to lane `r`, if any. For each
unmatched officer, DFS explores allowed officer-to-lane edges; if a lane is
occupied, it recursively searches for an alternate lane for its current
officer:

```text
augment(officer):
    for each allowed lane not already visited in this search:
        mark lane visited
        if lane is free or augment(currentOfficer(lane)):
            match lane to officer
            return success
    return failure
```

Every successful DFS adds one pair and updates the alternating path. When no
augmenting path exists, Berge's augmenting-path theorem says the matching is
maximum. With `L` officers and `E` allowed edges, at most `L` searches each
take O(E), for O(L E) time. Adjacency, matching, and search state use
O(L+R+E) storage, plus O(L) recursive stack depth. Duplicate edges do not
change the result; stable input traversal makes tie outcomes deterministic.
