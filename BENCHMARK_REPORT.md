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

## Airport scenario benchmark

`project31_airport_scenario_benchmark` compares shortest-workload and
round-robin assignment on identical, deterministic synthetic inputs of 100,
5,000, and 50,000 passengers, 50,000 synchronized burst arrivals, and a
small workload designed to expose assignment-policy differences. Each
strategy/dataset pair runs three times in fresh `exec`-isolated worker
processes. The benchmark source performs one separate, unmeasured
process-isolated correctness preflight per strategy and dataset. The parent
validates passenger coverage, service intervals, assignments, resource
constraints, and the explicit divergence case before launching the three
timed repetitions. Each measured result is then validated and compared with
its preflight schedule before any timing results are printed. Preflight
runtime and RSS are excluded from the reported measurements.

The explicit divergence workload gives one lane 10 minutes of assigned work
and the other 9 minutes when both become free at minute 10. Round-robin next
selects the first lane, while shortest-workload selects the second. Both still
start the passenger at minute 10, so total wait and throughput can be
identical even though lane assignment and per-lane utilization differ. This
explains why aggregate-only comparisons can conceal strategy differences.

Runtime is reported as the median and min/max of the simulator's measured
microseconds. On Linux, peak RSS is collected in each worker with
`getrusage(RUSAGE_SELF).ru_maxrss`; Linux reports this value in KiB. On
Windows, the worker uses `GetProcessMemoryInfo` and reports
`PeakWorkingSetSize` converted from bytes to KiB. The Windows value is peak
resident working set, not Linux `ru_maxrss`. Both are process-lifetime
high-water measurements that include generated input and simulation/result
storage and are affected by the operating system, allocator, compiler, and
host load. Isolated worker processes prevent one strategy's high-water mark
from contaminating another. The simulator's
`estimated_memory_bytes` remains an estimated result-storage metric and is
reported separately; it is not RSS.

Latest Fedora verification reports that the Release benchmark correctness
preflight passed for both strategies on all five datasets. It reported
different passenger-to-lane assignments in some workloads, but zero
service-interval differences and matching aggregate wait/throughput metrics.
These results do not establish operational superiority from runtime. The
benchmark source reports three isolated measured repetitions, median/min-max
runtime, platform-specific measured peak resident memory, and estimated
result-storage bytes separately.
No numeric Release timing or RSS values are recorded in this report; consult
the benchmark output for host-specific measurements.
