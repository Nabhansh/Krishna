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

## Backtracking — complete and verified

Implemented N-Queens enumeration and Subset Sum search, their AirportOperations
entry points, eight focused GoogleTests, `backtracking_example`, and
complexity/recurrence/airport-use documentation.

Verified baseline at commit `913467c`:

- 23 of 34 algorithms implemented and verified.
- All 80 GoogleTests passed.
- The backtracking example ran successfully.

## Branch and bound — verified

Verified FIFO, LIFO, and Least-Cost Branch and Bound for the shared 0/1
Knapsack equipment-allocation problem. The three strategies passed comparison
tests against each other and the existing dynamic-programming Knapsack
implementation. Their AirportOperations integrations and airport equipment
examples were verified.

## Bounded Travelling Salesperson — verified

Verified the bounded Travelling Salesperson implementation using Held-Karp
subset dynamic programming. It reconstructs a minimum-cost airport inspection
tour, supports directed/asymmetric route costs, and explicitly represents
missing routes. Its AirportOperations integration and runnable example were
verified.

## Latest verified development state

- All 34 of 34 mandatory algorithms implemented and verified.
- All 140 GoogleTests passed, including airport integration tests.
- FIFO, LIFO, and Least-Cost Branch and Bound verified.
- Bounded Travelling Salesperson using Held-Karp dynamic programming verified.
- Ford-Fulkerson (Edmonds-Karp) and Maximum Bipartite Matching verified.
- Chinese Remainder Theorem, Naive String Matching, and Rabin-Karp verified.
- Finite Automaton String Matching and Knuth-Morris-Pratt (KMP) verified.
- Clean CMake build passed on the Fedora host.
- Complete CTest suite passed, 140/140.
- CRT scheduling and string-matching examples executed successfully.
- Four-algorithm string-matching benchmark completed successfully; Naive,
  Rabin-Karp, Finite Automaton, and KMP returned identical match positions.
- `git diff --check` passed.
- Current branch: `milestone-3-divide-greedy`.
- Latest verified algorithm commit: `e33d343`.

## Chinese Remainder Theorem and initial string matching — intermediate batch verified

Implemented the Chinese Remainder Theorem utility, Naive String Matching, and
Rabin-Karp String Matching. Added AirportOperations APIs for periodic
maintenance scheduling and flight-code log search, focused unit and
integration tests, runnable examples, and a benchmark comparing small,
medium, large, and adversarial periodic inputs. Both matchers were checked to
return identical positions before timing.

At that batch's successful Fedora host verification:

- Clean CMake build passed.
- Complete CTest suite passed, 133/133.
- CRT scheduling example passed.
- String-matching example passed.
- String-matching benchmark passed with matching result positions.
- `git diff --check` passed.

## Finite Automaton and KMP string matching — complete and verified

Added a 256-byte finite-automaton matcher and Knuth-Morris-Pratt matcher,
reusable prepared-pattern objects, AirportOperations log-search methods,
focused and differential tests, and an airport integration test. Extended the
string-matching example and benchmark to cover all four matchers; the
benchmark separates finite-automaton and KMP preprocessing from scanning and
checks result equality before reporting timings.

Successful Fedora host verification:

- All 34 mandatory algorithms implemented and verified.
- Clean CMake build passed.
- Complete CTest suite passed, 140/140.
- String-matching example passed.
- All four string matchers returned identical match positions.
- Four-algorithm benchmark passed.
- `git diff --check` passed.

Algorithm completion and verification establish the documented algorithm
scope only; they do not by themselves prove that the entire expected airport
simulator product is complete.

## Airport product-level audit — initial implementation; early verification superseded

The product audit is recorded in `reports/product_audit.md`. The initial
product pass added a deterministic scenario simulator, CSV input, CSV/JSON
result export, CLI scenario controls, two-strategy service comparisons,
staffing what-if recommendations, a synthetic large-data scenario, and an
airport workload benchmark. It also added product-level tests and product,
complexity-class, reduction, demo, and release documentation.

Verification during this audit:

- Debug CMake configuration succeeded.
- The pre-change CTest binary passed 140/140 before product changes.
- The post-change build in that earlier run stopped during system-header
  resolution, before project sources could be evaluated.
- The pre-Phase-1 product suite passed 152/152 before additional scheduling
  work; these results do not validate the current source.
- The three airport scenarios, scenario benchmark, and generated CSV/JSON
  outputs could not be executed or validated in this environment.
- No new airport performance values are claimed. Estimated memory is result
  storage, not peak process memory.
- No commit or push was made.

The audit's initial product build attempt did not verify those additions.
Before Phase 1, the product test suite subsequently passed 152/152; that
baseline still does not verify the current scheduling changes. The
deterministic model does not include stochastic arrivals, staff shifts/breaks,
interrupted screening, or production airport integrations. CMake
install/package targets remain out of scope and are listed as a release
decision in `docs/RELEASE_CHECKLIST.md`. The early build limitation above was
superseded by the successful Phase 1-4 Fedora verification below.

## Phase 1 — operational scheduling; Fedora host verified

Implemented against the existing scenario and passenger models:

- High, Elevated, and Standard scenario dispatch priority, with stable
  arrival/input-sequence tie breaking and a configurable aging interval
  (default 15 minutes). Aging promotes one category per interval and is capped
  at High; every passenger still receives mandatory screening.
- A timestamped priority-queue simulation for arrivals, completions, aging,
  and lane/officer outage boundaries. Same-time ordering is completion,
  resource reopening, resource closure, aging, arrival, then dispatch.
- Optional passenger lane eligibility, officer outages, whole-service outage
  deferral, and checks for exact-once completion and non-overlapping service.
- A staffing evaluation API and CLI flow that run multiple lane/officer
  configurations, check service targets and a configurable resource budget,
  and select the lowest-cost feasible candidate. The existing one-extra-pair
  comparison remains available.
- Focused scheduling, starvation, outage, eligibility, staffing, and
  regression tests, plus updated design, user, algorithm, complexity, demo,
  and audit documentation.

Host verification reported for Phase 1:

- Debug build succeeded on Fedora with GCC 16.2.1.
- CTest passed, 171/171.
- Normal, capacity-shock, and 50,000-passenger stress scenarios completed.
- The scenario benchmark completed for small, medium, large, and
  adversarial-burst workloads.
- Three CSV and three JSON exports parsed and matched their run summaries.

## Phase 2 — independent QA and benchmarking; host baseline verified

Implemented:

- Added the public `AirportScenarioRunner::runStrategy` entry point so isolated
  benchmark workers run one strategy through the same validated simulator.
- Added a minute-step reference simulator in integration tests. It uses
  independent vectors and direct time progression to compare deterministic
  generated small cases against both production strategies, including
  eligibility, aging, lane outages, and officer outages.
- Added CSV tests for empty/header-only files, missing/reordered columns,
  invalid values, duplicate IDs, large integers, and overflow boundaries;
  added JSON control-character escaping, exported-row/result consistency, and
  regression coverage for preserving a trailing empty CSV field.
- Reworked the airport benchmark to use three fresh `exec` workers per
  strategy/dataset, report median/range simulation time and Linux
  `getrusage(RUSAGE_SELF).ru_maxrss` in KiB, and keep estimated
  result-storage bytes separate.
- Added an assignment-divergence workload and comparisons of per-passenger
  lane assignments and service intervals. Benchmark documentation now
  describes isolation, repetitions, RSS limitations, and reproducibility.
- Corrected the benchmark worker failure diagnostic to use the existing
  `toString(strategy)` formatter instead of an undefined local name.
- Reworked the test CSV row splitter to parse delimiters while respecting
  quoted and escaped fields, and to retain the final empty field explicitly.

The supplied Fedora host baseline reports a successful full build, 179/179
CTest tests, benchmark execution with three isolated repetitions and actual
peak RSS, and successful normal, capacity-shock, and 50,000-passenger scenario
exports. It also reports consistent 41-column CSV rows and parseable JSON.

## Phases 3-4 — final pre-release verification; Fedora verified

Latest Fedora verification:

- Fresh Debug and Release configurations and full builds passed.
- CTest passed 179/179 in both Debug and Release.
- The Release benchmark correctness preflight passed for both strategies on
  all five datasets. The benchmark uses three isolated measured repetitions
  per strategy/dataset, reports median and min/max runtime, per-worker Linux
  peak RSS, and estimated result-storage bytes separately.
- Different passenger-to-lane assignments occurred in some workloads, but
  service-interval differences were zero and aggregate wait/throughput
  matched. Runtime alone does not establish operational superiority.
- Normal, capacity-shock, and 50,000-passenger scenarios completed; their CSV
  rows had consistent 41-column widths and their JSON exports parsed.
- Documented CLI scenario, CSV loading, export, and invalid-input workflows
  completed successfully. `git diff --check` passed.

The deterministic fixed-duration model still excludes stochastic arrivals,
staff shifts/breaks, interrupted service, and live airport integrations.
CMake install/package targets are not present and remain a separate release
decision. No commit, tag, or push is claimed by this status entry.

## Final delivery and cross-platform verification — in progress

- Added the 34-entry algorithm ownership inventory and the requirements
  traceability matrix. Student owner names remain pending because none were
  supplied or recorded in the repository.
- Added Windows MSVC CI and a Windows benchmark worker using
  `CreateProcess` and `GetProcessMemoryInfo`; Linux retains isolated POSIX
  workers and `getrusage(RUSAGE_SELF).ru_maxrss`.
- Added a reusable CSV/JSON scenario-export validator and documented exact
  Linux and PowerShell commands for the normal, capacity-shock, and
  50,000-passenger demonstrations.
- Fresh out-of-tree Debug and Release CMake configuration succeeded locally.
  Both builds stopped before compiling project code because the sandbox host
  is missing `/usr/include/features-time64.h`. No system headers or packages
  were modified. This environment has not yet verified the current source.
- The new Ubuntu and Windows MSVC CI workflow must pass before this delivery
  can be described as cross-platform verified or tagged for release.
