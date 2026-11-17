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
