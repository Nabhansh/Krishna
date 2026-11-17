# Requirements Traceability

This matrix is based on the Project31 specification and requirements supplied
with the repository work, cross-checked against the current source, CMake
targets, tests, and demonstration commands. No separate authoritative
requirements document was found in the repository; if a course rubric has
additional wording, reconcile it before final grading. “Complete” means
implemented and supported by a named test and/or runnable evidence, not that
the simulator is production-certified.

| Requirement | Implementation evidence | Test evidence | Documentation / demonstration evidence | Status |
|---|---|---|---|---|
| Sorting | `src/algorithms/MergeSort.cpp`, `QuickSort.cpp` | `tests/unit/sorting_tests.cpp` | `ALGORITHM_ANALYSIS.md`; `project31_greedy_example` and sorting benchmark | Complete |
| Searching | `src/algorithms/BinarySearch.cpp`; `src/core/PassengerIndex.cpp` | `tests/unit/searching_tests.cpp`, `tests/unit/index_tests.cpp` | `docs/DESIGN.md`; interactive passenger-ID lookup | Complete |
| Divide and conquer | Five modules under `src/algorithms/` (Maximum Subarray, Fast Exponentiation, Celebrity, Classical Matrix Multiplication, Strassen) | `tests/unit/divide_conquer_tests.cpp` | `docs/ALGORITHM_EXAMPLES.md`; Batch A example | Complete |
| Greedy algorithms | Activity Selection, Interval Partitioning, Huffman, Dijkstra, Prim, Kruskal sources | `tests/unit/greedy_tests.cpp` | `ALGORITHM_ANALYSIS.md`; `project31_greedy_example` | Complete |
| Dynamic programming | Knapsack, Matrix Chain, LCS, OBST, bounded TSP modules | `tests/unit/dynamic_programming_tests.cpp`, `dynamic_programming_batch2_tests.cpp`, `bounded_tsp_tests.cpp` | `ALGORITHM_ANALYSIS.md`; DP and TSP examples | Complete |
| Backtracking | `src/algorithms/NQueens.cpp`, `SubsetSum.cpp` | `tests/unit/backtracking_tests.cpp` | `docs/ALGORITHM_EXAMPLES.md`; `backtracking_example` | Complete |
| Branch and bound | FIFO, LIFO, and Least-Cost 0/1 Knapsack solvers | `tests/unit/branch_bound_tests.cpp`, `least_cost_branch_bound_tests.cpp` | `ALGORITHM_ANALYSIS.md`; branch-bound examples | Complete |
| Graph algorithms | `src/algorithms/BellmanFord.cpp`, `FloydWarshall.cpp` | `tests/unit/graph_algorithms_tests.cpp` | `docs/ALGORITHM_EXAMPLES.md`; graph example | Complete |
| Flow and matching | `src/algorithms/FordFulkerson.cpp`, `MaximumBipartiteMatching.cpp` | `tests/unit/flow_algorithms_tests.cpp`, `tests/integration/flow_operations_tests.cpp` | `docs/ALGORITHM_EXAMPLES.md`; flow and matching examples | Complete |
| String matching | Naive, Rabin-Karp, DFA, and KMP modules | `tests/unit/string_matching_tests.cpp`, `string_matching_batch2_tests.cpp`, `tests/integration/number_theory_string_matching_tests.cpp` | `docs/ALGORITHM_EXAMPLES.md`; four-matcher benchmark | Complete |
| Passenger ID and priority indexes; queue workload index | `src/core/PassengerIndex.cpp`, `src/simulator/QueueWorkloadIndex.cpp` | `tests/unit/index_tests.cpp`, `tests/unit/airport_scheduling_tests.cpp` | `docs/DESIGN.md`; CLI lookup, priority, and queue commands | Complete |
| Deterministic airport simulator | `src/simulator/AirportScenario.cpp`; `include/simulator/AirportScenario.hpp` | `tests/integration/airport_scenario_tests.cpp`, `airport_scheduling_tests.cpp`, `tests/regression/airport_scenario_regression_tests.cpp` | `docs/USER_GUIDE.md`; normal, shock, and large demonstrations | Complete for the documented fixed-duration simulation model |
| Priority scheduling, aging, lane/officer eligibility and outages | Event ordering and dispatch in `AirportScenarioRunner` | `AirportPrioritySchedulingTests`, scenario oracle and outage tests in the integration/regression files above | `docs/DESIGN.md`, `docs/USER_GUIDE.md` | Complete for documented constraints |
| Staffing recommendations | `AirportScenarioRunner::evaluateStaffing`; `AirportOperations` facade and CLI | `AirportStaffingEvaluationTests` in `tests/integration/airport_scheduling_tests.cpp` | User guide staffing evaluation section; CLI option 12 | Complete |
| Reusable C++ library | CMake `project31_core` target with public headers in `include/` | Library is linked by tests and every example/CLI target | `docs/DESIGN.md`; in-tree examples | Complete as an in-tree CMake library; install/package rules are not provided |
| CLI | `src/simulator/CommandLineInterface.cpp`, `src/main.cpp` | `AirportScenarioCliTests` and airport integration tests | `docs/USER_GUIDE.md`; interactive section of `docs/DEMO_SCRIPT.md` | Complete |
| CSV input and CSV/JSON export | `AirportScenarioRunner::loadCsv*`, `writeCsv`, `writeJson` in `src/simulator/AirportScenario.cpp` | CSV robustness, JSON escaping, export consistency, and CLI tests in `tests/integration/airport_scenario_tests.cpp` | User guide CSV/export sections; demonstration exports | Complete for the documented schema |
| Benchmarking and memory reporting | `benchmarks/airport_scenario_benchmark.cpp` | Correctness preflight and schedule/resource checks execute before timed reporting; invoked in CI | `BENCHMARK_REPORT.md`, `reports/algorithm_comparison.md`; `docs/DEMO_SCRIPT.md` | Complete after Linux and Windows CI verification; metrics are host-specific |
| Test coverage | GoogleTest target `project31_tests` in `CMakeLists.txt` | Unit, integration, regression, generated/reference-oracle tests; Fedora baseline 179/179 | `DEVELOPMENT_STATUS.md`; CI runs CTest on both platforms/configurations | Complete for committed test suite; external CI result is a release gate |
| Documentation and demonstration scenarios | Root reports and `docs/` guides/examples | CLI, scenario, export, and regression tests provide executable checks | README, user guide, design, demo, algorithm examples, complexity and release docs | Complete for current repository scope |
| Individual student ownership attribution | `docs/ALGORITHM_OWNERSHIP.md` inventories all 34 implementation entries | Each entry points to its unit test | Ownership document | Partial: names/assignments were not supplied; every owner is pending |

The simulator models deterministic arrivals and fixed screening durations. It
does not model stochastic demand, staff shifts/breaks, interrupted service,
or live airport systems. Those limits are product scope, not hidden
completions. Current benchmark evidence shows some lane-assignment changes
without service-interval or aggregate wait/throughput changes on the tested
workloads; runtime alone is not evidence that a strategy is operationally
better.
