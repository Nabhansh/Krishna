# Reduction Mapping

These are mathematical mappings between decision/optimization formulations
used to explain complexity and cross-check compatible airport APIs. They are
not additional algorithm implementations, and no security policy is reduced
to a score or bypass.

## Subset Sum to 0/1 Knapsack decision

Given nonnegative Subset Sum values `a[i]` and target `T`, construct a
Knapsack instance with weight `a[i]`, value `a[i]`, capacity `T`, and ask
whether the maximum value is at least `T`. Any feasible selection has total
weight at most `T`; value equals weight, so reaching value `T` is equivalent
to a subset summing exactly to `T`. This standard reduction explains why
exact Subset Sum and Knapsack optimization have exponential general-case
algorithms despite their use in small equipment/resource examples.

## Hamiltonian Cycle to Travelling Salesperson feasibility

Given an undirected graph with `n` vertices, create an `n`-by-`n` route matrix
with cost 1 for each original edge and no route for each non-edge. A
Hamiltonian cycle exists exactly when the bounded TSP solver can construct a
tour visiting all vertices and returning to its start. The mapping is
polynomial in the graph size. It demonstrates the general TSP hardness; the
project's Held-Karp implementation is capped and is not a general-scale
polynomial solver.

## Bipartite Matching to unit-capacity Maximum Flow

For officer partition `L` and lane partition `R`, create a source, a sink,
one node per officer, and one node per lane. Add source-to-officer and
lane-to-sink edges with capacity 1, plus capacity-1 edges for allowed
officer-lane assignments. Integral maximum flow has value equal to maximum
matching cardinality; each unit-flow path identifies one assignment. This
connects the two implemented airport resource-allocation APIs and provides a
small-instance cross-check formulation.

## Limits

These reductions concern abstract computational problems and are not security
screening rules. The scenario runner's risk labels and resource recommendations
remain scheduling/planning inputs: risk priority with aging affects dispatch,
and staffing recommendations are scenario estimates. Neither changes the
mandatory screening requirement; every passenger must still be screened.
