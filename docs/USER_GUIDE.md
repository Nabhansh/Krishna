# User Guide

## Build and test

Run these commands from the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

GoogleTest 1.15.2 is discovered as a system package first or fetched by CMake.
No third-party runtime library is required.

### Windows PowerShell with MSVC

Use a Visual Studio 2022 Developer PowerShell from the repository root:

```powershell
cmake -S . -B build
cmake --build build --config Debug --parallel
ctest --test-dir build -C Debug --output-on-failure
```

Replace `Debug` with `Release` for a Release build and test. CMake uses the
Visual Studio generator and MSVC toolchain in this shell. Windows benchmark
workers use `CreateProcess` and report peak working set via
`GetProcessMemoryInfo`; estimated result-storage bytes remain a separate
metric.

## Interactive CLI

Start `./build/project31`. The original manual workflow remains available:

1. Register a passenger with a unique ID, nonblank name, positive screening
   duration, priority, and nonnegative registration timestamp.
2. Record a registered passenger's arrival. The live queue manager assigns
   them to the lane with the least queued screening work and reports that
   workload as the estimated wait.
3. Screen the next passenger in a selected lane.
4. Display lane queue lengths and remaining workload.
5. Exit.
6. Look up a registered passenger.
7. List passengers by priority.
8. Sort a copy of passenger IDs with Merge Sort or Quick Sort.
9. Run a built-in scenario.
10. Load and compare a passenger CSV.
11. Export the last scenario's results.
12. Evaluate staffing against configured service targets.

Live CLI timestamps are integer minutes entered by the operator. They do not
advance a wall clock or automatically process events.

### Built-in scenarios

Menu option 9 offers:

- **Normal operation**: reads `data/normal.csv`, with two staffed lanes.
- **Capacity and staffing shock**: reads `data/capacity_shock.csv`, uses three
  staffed lanes, and makes Lane-1 unavailable from minute 3 to minute 12.
- **Large-data stress**: generates 50,000 deterministic synthetic passengers
  and assigns them to six staffed lanes.

Each run compares shortest-workload and round-robin lane assignment using the
same passenger records, staff, and outages. Both use the same risk-priority
dispatcher. It also runs the existing one-extra-lane what-if. Results show
passenger assignments and waits (a sample for very large runs), risk-category
counts, completions, mean/maximum wait, throughput per hour, lane utilization,
runtime, and estimated result-storage bytes.

### User CSV

Menu option 10 asks for a CSV path, lane count, and officer count. The current
scenario model requires one available officer per active lane. Comparative
runs accept at most 127 baseline lane/officer pairs so the additional-lane
what-if remains within the 128-resource limit. Input must use this header:

```csv
passenger_id,name,arrival_minute,screening_duration_minutes,priority
```

The original five-column schema is retained. An optional sixth column named
`eligible_lanes` can restrict a passenger to IDs such as `Lane-1|Lane-3`; an
omitted or empty value permits every configured lane. CSV fields support
quoted commas and doubled quote characters. Passenger records are stably
ordered by arrival time after loading. Duplicate or blank IDs/names, negative
arrival times, nonpositive screening durations, priority values outside 1-3,
invalid lane IDs, malformed rows, and unsupported headers are rejected.

Priority order is High (1), Elevated (2), then Standard (3). At dispatch,
equal effective priority is resolved by earlier arrival, then stable input
sequence. Each waiting interval of `priorityAgingIntervalMinutes` promotes a
passenger by one category, capped at High; the default is 15 minutes. A
Standard passenger therefore reaches High after 30 minutes waiting at the
default interval. This bounds priority disadvantage even with continuing
high-risk arrivals. Priority never bypasses screening.

The scenario engine uses a timestamped event queue. At a shared timestamp it
processes screening completions, resource re-openings, resource closures,
aging promotions, and passenger arrivals, in that order; dispatch occurs only
after all same-time events have been applied. Lane outages and officer
unavailability use half-open intervals `[start,end)`. A screening service is
never interrupted: it may end exactly when an outage starts, but a service
that would overlap an outage is deferred until the relevant resources are
available. Each active lane requires its paired officer, and an unavailable
officer makes that lane unavailable.

Officer outages and the aging interval are set through
`ScenarioConfiguration`; the legacy CSV rows and built-in scenario inputs
need no new fields. Every accepted passenger must complete screening exactly
once.

### Export

Menu option 11 writes both `<base>.csv` and `<base>.json`; the destination
directory must already exist. Exports contain run identifiers, scenario/input
source, strategy, correctness flag, passenger assignments and waits, lane and
officer data, risk-category counts, throughput, utilization, elapsed runtime,
estimated result-storage bytes, staffing what-if, and lane/officer outage
intervals.
JSON strings are escaped; CSV fields use quote escaping.

### Staffing evaluation

Menu option 12 evaluates the most recently run or loaded scenario. Enter a
minimum and maximum lane/officer pair count (1-128), optional maximum mean and
individual wait targets, an optional minimum throughput target, per-lane and
per-officer costs, and an optional total budget. Target and cost values are
whole numbers at the CLI; the C++ API accepts fractional wait/throughput
targets.

Each candidate count is run through the same event simulator and
shortest-workload lane assignment. The configured resources must be available
within the hard 128-lane/128-officer cap, with one officer per lane. The
default cost is one unit per lane plus one per officer; the API and CLI allow
different nonnegative unit costs. The recommendation is the lowest-cost
candidate satisfying all specified service targets and budget. Ties are
resolved by mean wait, maximum wait, then lane count. If no evaluated
candidate meets the targets, the CLI reports that no feasible staffing
configuration was found instead of promoting an unsuccessful plan.

## Standalone scenario and benchmark commands

Run all three scenarios and export results:

```sh
./build/project31_airport_scenarios build/scenario-results
```

From a Visual Studio Debug build in PowerShell, use:

```powershell
.\build\Debug\project31_airport_scenarios.exe build\scenario-results
```

The command creates `build/scenario-results` if needed. The large workload is
synthetic, not a measured real-world passenger dataset.

Compare both assignment strategies on identical generated datasets:

```sh
./build/project31_airport_scenario_benchmark
```

For a Windows Release build, use:

```powershell
.\build\Release\project31_airport_scenario_benchmark.exe
```

The benchmark covers 100, 5,000, and 50,000 records plus 50,000 synchronized
adversarial arrivals, and a small assignment-divergence workload. Each
strategy runs three times in a fresh worker process after an unmeasured
correctness preflight for both strategies validates the schedules and
resources. Timed outputs are also checked against their preflight schedules
before median/range runtime and strategy differences are reported. On Linux
it reports each worker's
`getrusage(RUSAGE_SELF).ru_maxrss` peak RSS in KiB. This process-lifetime
high-water mark includes generated input and result storage and is host and
allocator dependent. Estimated result-storage bytes are reported separately
and are not RSS. On Windows, each worker reports peak working set from
`GetProcessMemoryInfo` (`PeakWorkingSetSize`, bytes converted to KiB); this is
not the Linux `ru_maxrss` metric. Tested workloads showed different lane
assignments without changed service intervals or aggregate wait/throughput;
runtime alone does not establish operational superiority. Results are
observations, not performance guarantees.

The four-string-matcher and sorting benchmarks are separate algorithm
measurements, not airport-service performance measurements. See
[ALGORITHM_EXAMPLES.md](ALGORITHM_EXAMPLES.md) for all example commands.
