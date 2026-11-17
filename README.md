# Project 31: Airport Security Queue and Resource Allocation Simulator

Project 31 is a portable C++20 airport operations project. It includes a
manual passenger registration/queue CLI, a deterministic discrete-event
scenario simulator, CSV input, CSV/JSON result export, and a reusable library
of 34 implemented algorithm modules. Scenario dispatch orders passengers by
risk category with configurable priority aging, while requiring every
passenger to complete screening.

## Build and test

Requirements: CMake 3.20+, a C++20 compiler, and GoogleTest 1.15.2 for tests.
CMake uses an installed GoogleTest package or downloads the pinned version
during configuration. Production code has no third-party runtime dependency.

Run from the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

For Fedora/Linux, use the commands above. On Windows, open a Visual Studio
2022 Developer PowerShell (MSVC) at the repository root:

```powershell
cmake -S . -B build
cmake --build build --config Debug --parallel
ctest --test-dir build -C Debug --output-on-failure
```

For a Release verification, replace `Debug` with `Release` in the build and
CTest commands. CMake selects the Visual Studio generator and MSVC toolchain
in the Developer PowerShell environment. The GitHub Actions Windows job also
builds and tests with MSVC; MinGW is not used as a substitute.

## Interactive application

Start `./build/project31` on Linux or `.\build\Debug\project31.exe` on
Windows. The menu supports manual registration, passenger arrival, screening,
queue display, ID lookup, priority listing, and Merge/Quick Sort of passenger
IDs. It also provides:

- **Run built-in scenario**: normal operation, capacity/staffing shock, or a
  50,000-passenger deterministic synthetic stress workload.
- **Load and compare a passenger CSV**: choose a path and the number of lanes
  and officers. The model requires exactly one available officer per active
  lane. Comparisons accept 1-127 baseline pairs so the additional-staffing
  what-if stays within the 128-resource limit.
- **Export last scenario**: enter an output base path; `.csv` and `.json`
  files are written alongside it. The parent directory must already exist.
- **Evaluate staffing against service targets**: evaluate actual simulation
  runs for a lane-count range and choose the least-cost plan satisfying the
  selected wait, throughput, and budget limits.

The application accepts this CSV schema:

```csv
passenger_id,name,arrival_minute,screening_duration_minutes,priority
P001,Ada Lovelace,0,4,2
```

An optional sixth column, `eligible_lanes`, restricts a passenger to listed
IDs such as `Lane-1|Lane-3`; an omitted or empty value means every configured
lane is eligible. The original five-column schema remains supported. Fields
may be quoted using standard doubled-quote escaping. IDs must be unique,
arrival times nonnegative, durations positive, and priority 1, 2, or 3.
Priority 1 is High, 2 Elevated, and 3 Standard.

The scenario dispatcher always chooses the best effective risk priority that
can use an available eligible lane. Equal effective priorities are ordered
by arrival time, then stable input sequence. Every configured aging interval
raises a waiting passenger by one category, capped at High; the default is 15
minutes. This prevents indefinite starvation under continuing higher-risk
arrivals without bypassing screening.

The deterministic event engine processes completions, resource re-openings,
resource closures, aging promotions, and arrivals in that order at each
timestamp, then dispatches. A screening cannot overlap a lane or its assigned
officer's outage; service is delayed until the whole screening fits, not
paused or skipped. The engine reports per-passenger wait and assignment,
mean/maximum wait, throughput, lane utilization, runtime, and estimated
result-storage size. Arrivals and screening durations are fixed, not a
stochastic forecast.

The one-extra-lane/officer comparison remains a sensitivity result.
Configurable staffing evaluation separately simulates each candidate
lane/officer count, applies optional wait/throughput targets and a resource
budget, and recommends the lowest-cost candidate satisfying every target.
The default resource-cost assumption is one cost unit per lane plus one per
officer; both unit costs and the total budget are configurable.

## Executable scenarios and benchmarks

Run the three airport scenarios and write actual result files (the runner
creates the output directory):

```sh
./build/project31_airport_scenarios build/scenario-results
```

Windows PowerShell (from the repository root, after a Debug build):

```powershell
.\build\Debug\project31_airport_scenarios.exe build\scenario-results
```

This runs the five-record normal sample, the bursty capacity-shock sample
with Lane-1 unavailable from minute 3 through minute 12, and a generated
50,000-passenger large-data workload using six staffed lanes. The synthetic
input is deterministic and is identified as synthetic in both exports.

Compare both assignment strategies on identical synthetic small, medium,
large, synchronized-burst, and assignment-divergence inputs:

```sh
./build/project31_airport_scenario_benchmark
```

Windows PowerShell:

```powershell
.\build\Release\project31_airport_scenario_benchmark.exe
```

Before measurement, an unmeasured isolated correctness preflight validates
both strategies for each dataset; each of the three measured worker results is
also checked against its preflight. Measurements report median/range runtime,
service quality, and assignment/interval differences. On Linux, peak RSS is
each worker's `getrusage(RUSAGE_SELF).ru_maxrss` high-water mark in KiB. On
Windows, the isolated worker reports `PeakWorkingSetSize` from
`GetProcessMemoryInfo`, converted from bytes to KiB; this is peak resident
working set, not a Linux `ru_maxrss` value. Estimated result-storage bytes
are a separate estimate, not measured memory. Results are observational, not
pass/fail performance thresholds.

In the tested workloads, strategies sometimes assigned passengers to
different lanes without changing service intervals or aggregate wait and
throughput. Runtime differences compare implementation cost only and do not
establish that either assignment strategy is operationally superior.

Additional algorithm demonstrations and the four-string-matcher benchmark
are listed in [docs/ALGORITHM_EXAMPLES.md](docs/ALGORITHM_EXAMPLES.md). The
algorithm library includes sorting/searching and indexing, divide-and-conquer,
greedy, dynamic programming, graph, flow/matching, backtracking,
branch-and-bound, number theory, and string matching implementations.

## Data, architecture, and status

`data/normal.csv`, `data/capacity_shock.csv`, `data/large_dataset.csv`, and
`data/adversarial.csv` provide sample inputs. The adversarial fixture is
expected to fail validation and is used by tests.

- `include/core`, `src/core`: passenger, lane, officer, airport, and index
  models.
- `include/simulator`, `src/simulator`: manual queue workflow, scenario
  simulation, CSV parsing/export, and command-line interface.
- `include/algorithms`, `src/algorithms`: independently implemented algorithms.
- `tests/unit`, `tests/integration`, `tests/regression`: algorithm, workflow,
  input-validation, export, and scenario tests.
- `benchmarks`, `examples`: reproducible algorithm and airport workload runs.
- `reports`, `docs`: complexity, recurrence, comparison, design, user, demo,
  product audit, and release material.

See [docs/DESIGN.md](docs/DESIGN.md), [docs/USER_GUIDE.md](docs/USER_GUIDE.md),
[docs/DEMO_SCRIPT.md](docs/DEMO_SCRIPT.md),
[docs/REQUIREMENTS_TRACEABILITY.md](docs/REQUIREMENTS_TRACEABILITY.md),
[docs/ALGORITHM_OWNERSHIP.md](docs/ALGORITHM_OWNERSHIP.md), and
[DEVELOPMENT_STATUS.md](DEVELOPMENT_STATUS.md). Algorithm completion does
not by itself establish that every operational requirement of a production
airport system is met; model scope and remaining verification are recorded in
the product audit and development status.
