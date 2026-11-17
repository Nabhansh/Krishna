# Algorithm and Airport Strategy Comparisons

## Airport assignment strategies

The airport scenario benchmark compares shortest-workload and round-robin
assignment using the same ordered passenger records, staffed lanes, officers,
and outage windows. Before timing output is accepted, the runner verifies that
both strategies serve every passenger exactly once, assign only to staffed
lanes, and schedule no service during an outage.

The generated datasets cover 100, 5,000, and 50,000 passengers, 50,000
synchronized arrivals as a burst/adversarial case, and a small explicit
assignment-divergence case. Each strategy is run three times in a fresh child
process (POSIX `fork`/`exec` on Linux and `CreateProcess` on Windows). The
benchmark source first runs an
unmeasured isolated preflight for each strategy/dataset, validates every
passenger, service interval, assignment, and resource constraint, then starts
the three timed repetitions. Each measured output must match its validated
preflight and repeated schedule before results are reported. Preflight runtime
and RSS are excluded. It compares assignments and service intervals as well
as mean/maximum wait, throughput, and average lane utilization.

Runtime is the median of the simulator's measured microseconds with
min/max-range. On Linux, actual peak RSS is measured per worker with
`getrusage(RUSAGE_SELF).ru_maxrss` and reported in KiB. On Windows,
`GetProcessMemoryInfo` reports `PeakWorkingSetSize` in bytes, converted to
KiB; this is peak resident working set rather than Linux `ru_maxrss`. Each is
a process-lifetime high-water mark that includes generated input and result
storage. The fresh worker prevents one strategy's process history from
contaminating the other. Estimated result-storage bytes remain a separate estimate.
Synthetic records are deterministic and are not real airport traffic.

Latest Fedora verification reports that the Release benchmark correctness
preflight passed for both strategies on all five datasets. Across the tested
workloads, some passenger assignments differed while service intervals and
aggregate wait/throughput matched. Runtime alone does not establish
operational superiority. The benchmark reports three isolated measured
repetitions, median/range runtime, platform-specific per-worker peak resident
memory, and estimated result-storage bytes separately. Numeric timings are intentionally not
reproduced here; they depend on the host and build conditions.

## Existing algorithm benchmarks

The Merge Sort/Quick Sort observations are recorded separately in
`BENCHMARK_REPORT.md`. The four-string-matcher benchmark checks identical
match positions before measuring preprocessing and search. Those
algorithm-level timing results do not measure airport wait or staffing quality
and should not be presented as airport simulation performance.

## Interpretation limits

Shortest-workload and round-robin are deterministic lane-assignment policies,
not security screening algorithms. Both use the same risk-priority dispatcher
and aging policy; risk affects queue order but never skips or shortens
screening. The benchmark uses fixed durations and excludes real-world
arrivals, shifts, breaks, and preemption. Its RSS measurement is the isolated
worker's process-lifetime peak, not memory attributable solely to the
scheduler; it includes generated inputs and runtime overhead.
