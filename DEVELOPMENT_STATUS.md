# Development Status

## Milestone 1 — complete

Implemented:

- The project directory layout and CMake C++20 build.
- Passenger, security officer, security lane, airport, queue manager, and
  simulation event models in modular headers and source files.
- Passenger field validation, duplicate-ID checks, and guarded status changes.
- Multiple FIFO security lanes and deterministic shortest-workload routing.
- Arrival, queue waiting-time estimates, screening, and an event history.
- A menu-driven CLI with input validation and user-visible operation errors.
- Four CSV data fixtures and focused GoogleTest unit/integration tests.
- README, development status, user guide, design notes, demo script, and
  initial analysis reports.

Verification on Fedora with GCC 16.2.1 and CMake 4.3.0:

- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug` — passed; fetched the pinned
  GoogleTest 1.15.2 because no system package was installed.
- `cmake --build build --parallel 4` — passed without warnings.
- `ctest --test-dir build --output-on-failure` — passed, 8/8 tests.
- Scripted registration, arrival, screening, and queue-display CLI smoke test —
  passed.

After that successful run, the unused empty `src/simulator/SimulationEvent.cpp`
translation unit was removed from the target. A post-cleanup rebuild could not
be started: the shell runner failed before command launch because its required
`slirp4netns` helper is unavailable, and the VS Code task/test runners were not
connected. No application behavior or compiled implementation changed in that
cleanup.

## Milestone 2 — complete and verified

Implemented:

- Hand-written Merge Sort and Quick Sort for integer arrays and passenger IDs.
- Hand-written iterative and recursive Binary Search returning the first
  matching duplicate.
- Sorted passenger ID indexing and priority buckets integrated with Airport
  registration, ID lookup, priority queries, and the CLI.
- A lane workload hash index updated by arrivals and screenings.
- Algorithm/index unit tests, priority-tagged sample data, and a sorting
  benchmark for small, medium, and large passenger-ID datasets.
- Complexity and design documentation for the implemented algorithms and
  indexes.

Verified on Fedora with GCC 16.2.1 and CMake 4.3.0:

- Release CMake configure and GCC build passed.
- All 22 GoogleTests passed.
- The sorting benchmark completed for 100, 5,000, and 50,000 passenger IDs.
- Benchmark measurements are recorded in `BENCHMARK_REPORT.md`.

## Batch A — complete and verified

Implemented five independent divide-and-conquer algorithms with headers,
sources, tests, and an example executable:

- Maximum Subarray, Fast Exponentiation, Celebrity Problem,
  Classical Matrix Multiplication, and Strassen Matrix Multiplication.
- Added `project31_batch_a_example` and airport analytics/capacity mappings in
  `docs/ALGORITHM_EXAMPLES.md`.
- Added ten focused GoogleTests, including empty/invalid inputs, numerical
  boundaries, rectangular products, and Strassen padding.

Verified on the current branch as confirmed for commit `a8811b5`:

- GCC/CMake build passed.
- All 32 GoogleTests passed.
- The Batch A example ran successfully.

## Batch B — implemented; current baseline confirmed

Implemented six independent greedy algorithm modules with separate headers
and sources:

- Activity Selection (hand-written merge sort by finish time).
- Interval Partitioning (hand-written merge sort and min-heap resource index).
- Huffman Coding (frequency heap, prefix-free code generation, encoding and
  decoding, including the single-symbol case).
- Dijkstra (rejects negative weights and reports unreachable vertices).
- Prim and Kruskal (minimum spanning forest for disconnected graphs); Kruskal
  includes its own DSU and hand-written edge merge sort.
- `AirportOperations` facade and executable airport scheduling/routing/
  infrastructure/log-compression demonstration.
- Seventeen Batch B GoogleTests for normal, empty, invalid, disconnected,
  overflow, and operational-facade cases.
- Greedy correctness, complexity, and airport mapping documentation.

Batch B verification:

- The build and all 49 tests passed.
- The greedy demonstration ran successfully.

## Dynamic programming — first two batches complete and verified

The first DP batch implemented:

- Bottom-up 0/1 Knapsack with a full DP table, selected-item reconstruction,
  input checks, and value/table-size overflow guards.
- Bottom-up Matrix Chain Multiplication with a split table, optimal
  parenthesization reconstruction, dimension checks, and checked cost
  arithmetic.
- Bottom-up Longest Common Subsequence with reconstruction of one optimal
  subsequence.
- Optimal Binary Search Tree with successful/unsuccessful probabilities,
  expected-cost calculation, and optimal tree reconstruction.
- Airport operations APIs, runnable examples, focused unit tests, and
  complexity/recurrence documentation for both batches.

Verified baseline at commit `ddd1d1b`:

- 19 of 34 algorithms implemented and verified.
- All 62 GoogleTests passed.
- Both dynamic-programming examples ran successfully.

## Graph shortest paths — complete and verified

Implemented Bellman-Ford and Floyd-Warshall with negative-weight support,
negative-cycle detection, checked distance arithmetic, path reconstruction,
airport route-planning APIs, cross-check tests, and an airport example.

Verified baseline at commit `f707b69`:

- 21 of 34 algorithms implemented and verified.
- All 72 GoogleTests passed.

## Backtracking — implementation added; verification blocked

Added N-Queens enumeration and Subset Sum search, their AirportOperations
entry points, eight focused GoogleTests, `backtracking_example`, and
complexity/recurrence/airport-use documentation.

Verification attempt:

- Debug CMake configuration succeeded.
- The clean build failed before project sources compiled because GCC 16's
  configured system headers reference unavailable `bits/wordsize.h`.
- CTest could not run because the clean build did not produce the test binary;
  the example executable is likewise unavailable.
- The 21/34 algorithms and 72/72 passing tests above remain the latest
  verified baseline. Backtracking implementations and eight new tests are
  authored but unverified; no commit or push was made.
