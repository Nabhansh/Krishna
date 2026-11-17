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

## Batch A — implemented; verification blocked

Implemented five independent divide-and-conquer algorithms with headers,
sources, tests, and an example executable:

- Maximum Subarray, Fast Exponentiation, Celebrity Problem,
  Classical Matrix Multiplication, and Strassen Matrix Multiplication.
- Added `project31_batch_a_example` and airport analytics/capacity mappings in
  `docs/ALGORITHM_EXAMPLES.md`.
- Added ten focused GoogleTests, including empty/invalid inputs, numerical
  boundaries, rectangular products, and Strassen padding.

CMake configuration passed. The first GCC build attempt could not compile
either project or GoogleTest because GCC's standard-library include
directories were unavailable to the compiler in the current environment
(`fatal error: string/cstddef: No such file or directory`). This is an
environment/toolchain-access failure, so Batch A is not verified. Batch B has
not been started; the requested build/test gate must pass first.

## Batch B — not started

The six greedy algorithms have not yet been implemented.

Dynamic programming and later categories remain deferred until Batches A and B
are implemented and verified.
