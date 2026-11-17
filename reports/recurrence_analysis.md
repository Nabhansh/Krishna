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
