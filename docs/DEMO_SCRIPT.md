# Project 31 Airport Operations Demonstration

Run commands from a clean checkout's repository root. The scenario executable
deliberately runs all three demonstrations in one invocation; it has no
per-scenario CLI flags. It compares shortest-workload and round-robin on the
same input and configuration for every scenario, validates coverage/resource
constraints, and writes both formats.

## 1. Configure, build, and test

### Fedora/Linux

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 4
ctest --test-dir build --output-on-failure
```

### Windows PowerShell with MSVC

Use Visual Studio 2022 Developer PowerShell:

```powershell
cmake -S . -B build
cmake --build build --config Debug --parallel 4
ctest --test-dir build -C Debug --output-on-failure
```

## 2. Run the required scenarios

### Fedora/Linux

```sh
./build/project31_airport_scenarios build/scenario-results
```

### Windows PowerShell (Debug build)

```powershell
.\build\Debug\project31_airport_scenarios.exe build\scenario-results
```

This one command runs each required scenario and produces
`normal.csv`/`.json`, `capacity_shock.csv`/`.json`, and
`large_stress.csv`/`.json` under `build/scenario-results/`.

| Demonstration | Exact input and generation | Passengers / resources | Expected evidence |
|---|---|---|---|
| Normal operation | `data/normal.csv` | 5 passengers; 2 lanes and 2 officers | Both strategies complete the same records; no outage is configured. |
| Capacity/constraint shock | `data/capacity_shock.csv`; Lane-1 outage `[3,12)` | 12 passengers; 3 lanes and 3 officers | Both strategies complete all records; service intervals do not overlap the closed lane/officer window. |
| Large-data stress | Deterministic synthetic generator in `AirportScenarioRunner::makeSyntheticScenario` | 50,000 passengers; 6 lanes and 6 officers | Both strategies complete all generated records; data is synthetic, not an airport passenger sample. |

Each JSON comparison has `correctness_verified: true` and three runs: both
strategies plus the one-extra-lane/officer sensitivity run. Each run records
every passenger's arrival, service start, completion, wait, lane, and officer.
The runner checks exact-once coverage, assignment eligibility, service
durations, lane/officer non-overlap, outage boundaries, and summary metrics
before exporting. The CSV keeps a consistent 41-column schema for summary,
passenger, lane, and outage records. The results are reproducible model
outputs, not measured airport operations.

The same input does not imply the same lane assignment. Existing benchmark
evidence found some assignment differences but zero service-interval
differences and matching aggregate wait/throughput on the tested workloads.
Neither policy is claimed to be operationally superior based only on runtime.

Validate all six exports on either platform with Python 3:

```text
python tools/validate_scenario_exports.py build/scenario-results
```

The validator parses every JSON and CSV, checks expected passenger counts,
exact-once passenger coverage in each run, start/completion/wait consistency,
lane/officer assignment consistency, resource interval non-overlap, and
consistent 41-column CSV rows.

## 3. Run the Release benchmark

### Fedora/Linux

```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel 4
./build-release/project31_airport_scenario_benchmark
```

### Windows PowerShell with MSVC

```powershell
cmake -S . -B build-release
cmake --build build-release --config Release --parallel 4
.\build-release\Release\project31_airport_scenario_benchmark.exe
```

For each of five deterministic datasets and both strategies, the benchmark
performs an isolated correctness preflight followed by three isolated
measured repetitions. It checks full schedules and resource constraints
before reporting median/min/max runtime, assignment and service-interval
differences, service metrics, measured peak memory, and estimated result
storage separately. Linux uses worker `getrusage(RUSAGE_SELF).ru_maxrss` in
KiB; Windows uses `GetProcessMemoryInfo` `PeakWorkingSetSize` converted from
bytes to KiB. Both are process-lifetime high-water measurements and include
input, simulation, and runtime overhead; Windows working set and Linux RSS
are platform-specific and should not be directly compared. Runtime alone is
not evidence of better service quality.

## 4. Interactive CLI

Linux: `./build/project31`. Windows Debug: `.\build\Debug\project31.exe`.

- Options 1-4 provide manual registration, queueing, screening, and queue
  status. Option 7 looks up priority categories; option 8 selects Merge Sort
  or Quick Sort.
- Option 9 selects a built-in scenario.
- Option 10 loads a CSV and prompts for lanes and officers. Try
  `data/adversarial.csv` to see invalid input rejected.
- Option 11 exports the last scenario. Use an existing base directory, for
  example `build/scenario-results/manual`, without an extension.
- Option 12 evaluates lane/officer ranges against optional wait, throughput,
  and cost targets.

Priority values 1 (High), 2 (Elevated), and 3 (Standard) affect dispatch;
waiting passengers age one category per configured interval, capped at High.
Stable arrival/input order breaks equal effective-priority ties. Scheduling
does not bypass mandatory screening. Manual queue options remain a separate
operator-controlled FIFO workflow.
