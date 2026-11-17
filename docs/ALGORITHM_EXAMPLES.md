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
