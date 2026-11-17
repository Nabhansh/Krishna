# Sorting Benchmark Report

## Results

The following recorded run used passenger-ID string datasets. Values are
elapsed sorting time in milliseconds:

| Records | Merge Sort (ms) | Quick Sort (ms) |
|---:|---:|---:|
| 100 | 0.083 | 0.057 |
| 5,000 | 7.897 | 6.242 |
| 50,000 | 93.125 | 77.558 |

These are the supplied measured results for this run, not newly generated
measurements. Runtime depends on compiler, build mode, hardware, and system
load. The benchmark executable is `project31_sorting_benchmark`.

## Scope and method

The runner constructs deterministic passenger IDs before timing, copies the
input outside the measured interval, times each sorting implementation with
`std::chrono`, and validates ordering after timing. The dataset sizes are
small (100), medium (5,000), and large (50,000). These measurements compare
sorting time only; they do not imply a general performance winner.

Additional algorithm benchmarks will be reported only after their runner has
produced real results.
