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
