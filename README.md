# Project31 Airport Security Simulator

Project31 is a C++20 airport passenger registration and security-queue simulator.
Milestone 1 provides the core simulator. Milestone 2 adds hand-written Merge
Sort, Quick Sort, iterative and recursive Binary Search, and passenger/queue
indexes integrated with the Airport APIs and CLI.

## Requirements

- CMake 3.20 or newer
- GCC 11 or newer (or another C++20 compiler); verified on Fedora with GCC
- GoogleTest 1.15.2 (CMake uses an installed package when available, otherwise
  it downloads the pinned version during configuration)

On Fedora, a system GoogleTest can be installed with `sudo dnf install
gtest-devel`; this avoids needing CMake to download it.

## Build and test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Run the interactive application with:

```sh
./build/project31
```

The CLI can look up passengers by ID, list passengers in a priority bucket,
and display IDs sorted with either implemented sorting algorithm. Lower
numeric priority values indicate more urgent passengers.

Run sorting benchmarks for 100, 5,000, and 50,000 realistic passenger IDs:

```sh
./build/project31_sorting_benchmark
```

Set `-DPROJECT31_BUILD_BENCHMARKS=OFF` to omit the benchmark executable.
Benchmark timings are observational and are not pass/fail thresholds.

The CLI starts with two staffed lanes. Register a passenger, record their
arrival, then screen the next passenger in a selected lane. Queue estimates
are the sum of screening durations already waiting in that lane at the time
the new passenger arrives.

## Data files

`data/normal.csv` is a small nominal workload, `capacity_shock.csv` contains
bursty arrivals, `large_dataset.csv` is a larger deterministic sample, and
`adversarial.csv` includes records intended to exercise validation. These are
input fixtures for subsequent simulator/data-import work; the Milestone 1 CLI
does not yet import CSV files.

## Project map

- `include/core`, `src/core`: passenger, officer, lane, and airport models
- `include/simulator`, `src/simulator`: queue manager, events, and CLI
- `include/algorithms`, `src/algorithms`: hand-written sorting and searching
  algorithms, with other algorithm topics reserved for later milestones
- `include/core/PassengerIndex.hpp`: sorted passenger ID and priority indexes
- `include/simulator/QueueWorkloadIndex.hpp`: per-lane workload index
- `tests`: unit, integration, and regression test areas
- `docs`, `reports`: user, design, demo, and analysis documentation
- `DEVELOPMENT_STATUS.md`: milestone scope and verification record

See [docs/DESIGN.md](docs/DESIGN.md) for architectural choices and
[docs/USER_GUIDE.md](docs/USER_GUIDE.md) for the CLI workflow.
