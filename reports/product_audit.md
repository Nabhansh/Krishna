# Project 31 Product Audit

Initial audit against the requested airport operations product. The first
table records the state before product-level changes. The final table records
the current implementation and verification state.

## Initial audit

| Requirement | Existing implementation / evidence | Status | Minimal action |
|---|---|---|---|
| Portable C++20 application and CMake build | `CMakeLists.txt` enforces C++20 without extensions and builds `project31`; baseline CTest passes 140/140. | Complete | Keep the existing standard/build; register the scenario runner and tests. |
| Discrete-event passenger simulation | `Airport`, `QueueManager`, and `SimulationEvent` register, enqueue, and screen manually; timestamps are supplied by the CLI and no simulation clock processes a workload. | Partial | Add a deterministic event-driven scenario runner with completion times. |
| Passenger, lane, officer, and risk-category records | `Passenger`, `SecurityLane`, and `SecurityOfficer` validate core fields; priority is an integer, with no named risk category or scenario staffing/closure model. | Partial | Preserve these models; map documented priority categories in scenario records and model staffed lanes and outages. |
| Service metrics and staffing/resource recommendations | Existing queue APIs report queued work and estimated wait; planning algorithms have airport examples, but no scenario throughput, utilization, or measured resource recommendation. | Partial | Report wait, throughput, utilization, and a simulated additional-staffed-lane what-if. |
| Reusable 34-algorithm library and airport connections | Algorithms have separate headers/sources, CMake registration, verification tests, examples, and multiple `AirportOperations` adapters. | Complete | Preserve every verified implementation/API and document operational use/limitations. |
| Menu-driven data loading, operational strategy selection, scenarios, and results | `CommandLineInterface` offers manual passenger registration, arrival, screening, display, lookup, priority listing, and sorting; it has no data loader or scenario workflow. | Partial | Add menu entries to run built-in or user-provided CSV scenarios, compare strategies, and display metrics. |
| Runnable normal, capacity-shock, and large-data scenarios | `data/normal.csv`, `data/capacity_shock.csv`, and `data/large_dataset.csv` are fixtures; `README.md` explicitly says CSV import is not implemented. | Missing | Add validated CSV loading, outage-aware shock simulation, and a deterministic generated large workload. |
| Airport scenario benchmark comparing valid strategies | Existing sorting and string-matching benchmarks time algorithms only; they do not simulate airport queues or compare service outcomes. | Partial | Add a runner comparing two assignment strategies on identical scenario records, validating results before measurements. |
| Memory, time, solution quality, and scalability comparison | Existing benchmarks report timings and correctness for their algorithm-specific tasks, not airport memory/quality metrics. | Partial | Report elapsed time, estimated result/workload storage, waits, throughput, and utilization by dataset size. |
| CSV and JSON operational export | The CLI and application have no export code or export tests. | Missing | Serialize actual run, passenger, lane/officer, metric, strategy, and benchmark data; validate output. |
| Unit, integration, edge, oracle, and regression coverage | CMake runs 140 tests across unit/integration sources; `tests/regression` is empty and there are no CLI, scenario-import, or export tests. | Partial | Add scenario, invalid-input, small-instance oracle, CLI, and serialization tests without weakening existing tests. |
| README, guides, reports, demonstration, and release preparation | Several documents describe only Milestones 1-2; `DEMO_SCRIPT.md` is a manual smoke test. Existing recurrence/complexity reports cover algorithms; no product release checklist, complexity-class analysis, or reduction mapping was found. | Partial | Update product workflow docs, add executable scenario/demo instructions, and provide the missing analysis/release notes. |

## Final audit status

| Requirement | Implementation / evidence | Status | Remaining action |
|---|---|---|---|
| Portable C++20 application | Fresh Fedora Debug and Release builds previously passed 179/179; Windows MSVC CI and a new Ubuntu CI run are being added for the current release source. | Pending current CI | CMake install/package targets are not provided; decide separately whether packages are required. |
| Discrete-event operations and service metrics | `AirportScenarioRunner` uses timestamped events, Passenger transitions, SecurityLane screening, paired officers, lane/officer outages, and event-derived wait, throughput, makespan, and utilization metrics. | Complete for the documented model | Fixed durations are used; stochastic arrivals, shifts/breaks, preemption, and live airport integrations are out of scope. |
| Risk-priority scheduling and starvation control | Scenario dispatch orders High/Elevated/Standard with configurable aging, arrival/FIFO tie breaking, and optional lane eligibility. Unit/integration/regression tests cover priority, aging, and exact-once screening. | Complete for the documented model | Priority affects scheduling only and is not an operational security policy. |
| 34-algorithm library and airport facade | All 34 previously verified algorithms remain unchanged; `AirportOperations` exposes scenario comparison and staffing evaluation. | Complete | Preserve the verified APIs and algorithms. |
| Menu-driven loading, scenarios, strategies, staffing, and export | CLI options 9-12 run built-ins, load CSV, compare shortest-workload/round-robin, export CSV/JSON, and evaluate staffing targets; documented flows and controlled invalid-input behavior passed. | Complete | Commands that use relative data paths must be run from the project root. |
| Normal, capacity-shock, and large scenarios | `examples/airport_scenarios.cpp` loads normal/shock CSVs and generates 50,000 deterministic synthetic passengers; outputs were generated and parsed. | Complete | Synthetic passengers are test data, not real airport observations. |
| Multi-strategy benchmark | Linux measurements were previously verified; current source retains Linux `ru_maxrss` and adds Windows `GetProcessMemoryInfo` working-set measurement plus isolated `CreateProcess` workers. | Pending Windows MSVC CI | Assignments differ in some workloads without changing service intervals or aggregate wait/throughput; runtime is not operational superiority. |
| Configurable staffing recommendations | `evaluateStaffing` runs candidate lane/officer counts through the simulator, filters invalid eligibility/resource candidates, applies targets and budget, and recommends the least-cost feasible result or none. | Complete | One officer per lane and the 128-resource cap are model constraints. |
| CSV/JSON export | `writeCsv/writeJson` and file exporters include run, passenger, lane/officer, metric, strategy, and outage data; generated CSV/JSON passed parsing and consistency checks. | Complete | CSV retains the documented schema; JSON/CSV are generated result records, not a durable database format. |
| Test coverage | The current suite contains 179 tests, including the independent minute-step oracle, malformed/edge CSV, numeric boundaries, escaping, export consistency, and regression cases. | Complete | Re-run both build configurations when source changes. |
| Documentation, demo, and release preparation | README, guides, demo, algorithm/complexity reports, reduction mapping, release checklist, and this audit describe current behavior and limitations. | Complete for this pre-release audit | Production packaging and operational-stakeholder review remain separate decisions. |

### Verification boundary

The preceding Fedora baseline completed fresh Debug and Release builds and
reported 179/179 CTest passes in each configuration, before the current
Windows portability and CI changes. The Release benchmark
correctness preflight passed for both strategies on all five datasets. It
reports three isolated measured repetitions, runtime median/range, actual
per-worker peak RSS, and estimated result-storage bytes separately. In some
workloads, lane assignments differed while service intervals and aggregate
wait/throughput remained the same. No strategy is described as operationally
superior based on runtime.

The normal, capacity-shock, and 50,000-passenger scenarios, CSV/JSON parsing,
documented CLI workflows, and controlled invalid-input behavior were also
verified at that baseline. Current Linux/Windows CI must pass before this
audit can mark cross-platform delivery verified. The current model remains
deterministic and fixed-duration; it is not production airport screening
software. CMake install/package targets are not provided.
