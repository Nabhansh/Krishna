# Design Decisions

## Product scope and boundaries

The project contains the live passenger-to-screening workflow, a deterministic
scenario simulator with CSV input and CSV/JSON export, and the 34 mandatory
algorithm implementations. The algorithm modules remain independently
callable; `AirportOperations` connects the airport-planning uses that have
facade methods. Product completion is distinct from algorithm verification.

The scenario simulator is deterministic and uses fixed screening durations.
It models staffing as one officer per active lane, passenger-to-lane
eligibility, lane/officer outages, and risk-priority dispatch with aging.
Priority changes scheduling order only; every accepted passenger still
completes mandatory screening. It does not model stochastic arrivals, shifts,
breaks, interrupted screening, or real-time sensor feeds.

## Modules and ownership

- `core` types hold airport domain state and enforce local invariants.
- `QueueManager` owns lanes and selects a lane on arrival.
- `PassengerIndex` maintains the sorted ID and priority lookup structures;
  Airport's ID-to-passenger hash map remains the authoritative record store.
- `QueueWorkloadIndex` maintains lane workloads incrementally rather than
  rescanning each lane's passenger queue for every wait-time query.
- `Airport` is the application-facing coordinator. It owns registered
  passengers and event history, while its queue manager owns queued copies.
- The CLI is implemented separately from `main.cpp`; `main.cpp` only wires
  standard input/output into the CLI.
- `AirportScenarioRunner` validates input, creates ordered events, dispatches
  passengers, calculates metrics from completion/start events, compares lane
  assignment strategies, evaluates staffing candidates, and serializes results.
  `AirportOperations` exposes these operations to the CLI.
- Each scenario lane is paired with exactly one `SecurityOfficer`. Arriving
  passengers transition through the existing `Passenger` states; the
  `SecurityLane` queue holds the passenger currently being screened. Waiting
  passengers are maintained in priority-ordered lane-eligibility sets so
  priority and eligibility can be considered globally at dispatch.
- Public interfaces live under `include/`; implementations live under `src/`.
  This keeps domain rules testable without driving the terminal.

The queue stores passenger snapshots rather than references into the airport's
registry. This avoids dangling references if the registry's hash table grows.
The airport uses passenger IDs to update the authoritative status after
screening.

## Validation and state transitions

Passenger ID and name must contain a non-whitespace character, screening
duration must be positive, and IDs are unique per airport. Event times must be
nonnegative. Passenger status progresses only from registered to queued to
screened; invalid transitions are reported as exceptions and are not silently
accepted. The CLI catches operation errors at the interaction boundary and
prints them to the user.

## Sorting, searching, and indexing

Passenger IDs are kept in a sorted vector. Registration finds the insertion
point with the hand-written iterative lower-bound search, inserts the ID,
and rejects duplicates using hand-written iterative Binary Search. Airport
lookup uses that binary search before retrieving the authoritative record from
the hash map. Priority groups map integer priority values to passenger IDs;
lower numbers indicate greater urgency. Priority query results are ordered by
ID using the hand-written Merge Sort.

The CLI can sort a copy of registered IDs with either Merge Sort or Quick
Sort. Merge Sort is stable with O(n log n) time and O(n) auxiliary space.
Quick Sort uses median-of-three, three-way partitioning, with O(n log n)
average and O(n^2) worst-case time. It recursively processes the smaller
partition and iterates over the larger, limiting call-stack space to O(log n)
even for a poor partition sequence.

Both Binary Search variants return the first matching position for duplicates.
They run in O(log n) time; the iterative implementation uses O(1) space and
the recursive implementation uses O(log n) call-stack space. Inputs must
already be sorted.

Queue workloads are maintained in an unordered map keyed by lane ID. An
arrival increments the cached sum and screening decrements it; workload
lookups are expected O(1). The least-workloaded lane is still selected by a
linear pass over lanes so ties remain deterministic by lane insertion order.
The workload accumulator checks for 64-bit overflow.

## Divide-and-conquer algorithms (Batch A)

- Maximum Subarray returns a half-open range and sum using recursive split,
  cross-boundary scan, and combine; it handles empty input and reports
  unrepresentable signed sums.
- Fast Exponentiation uses exponentiation by squaring, accepts signed 64-bit
  exponents, and rejects non-finite inputs, zero to a negative exponent, and
  floating-point overflow.
- Celebrity Problem uses candidate elimination followed by verification;
  relation matrices must be square and contain only zero/one values.
- Classical matrix multiplication validates rectangular shape compatibility
  and finite entries. It supports rectangular products.
- Strassen matrix multiplication pads compatible rectangular inputs to a
  power-of-two square, recursively evaluates seven products, and falls back
  to classical multiplication at small subproblems.

The maximum-subarray routine is intended for arrival-volume deviation series
in a scenario/analytics layer. Fast exponentiation can model compounded
capacity growth. Matrix operations and the celebrity relation solver are
independent algorithm exercises and are not used to make passenger screening
decisions.

## Queue policy and waiting time

The original manual CLI queue manager is a separate workflow: it assigns an
arrival to the lane with the least queued screening work, breaks ties by lane
insertion order, and screens the lane head when the operator requests it.
Manual timestamps do not advance a simulation clock.

The scenario simulator uses global dispatch priority: High (CSV priority 1),
Elevated (2), then Standard (3). At each dispatch it considers passengers
whose eligible lanes and paired officers are available and selects the
highest effective priority. Ties use earlier arrival followed by stable input
sequence. Each full waiting interval of
`ScenarioConfiguration::priorityAgingIntervalMinutes` promotes the passenger
one level, capped at High; the default interval is 15 minutes. A Standard
passenger reaches High after two intervals. Aging is only applied while
waiting. Dispatch priority never bypasses screening.

For the same priority order, ShortestWorkload selects the currently available
eligible lane with the least cumulative assigned screening work, breaking
ties by lane index. RoundRobin selects the next available eligible lane in
stable lane order. These are lane-assignment policies; both use the same
priority/aging dispatcher.

## Scenario event simulation

The scenario runner is separate from the manual live CLI workflow. The
five-column CSV remains valid; an optional `eligible_lanes` column takes
pipe-separated lane IDs (`Lane-1|Lane-3`). An absent or empty value means all
configured lanes. Scenario configuration supplies the positive priority
aging interval and optional officer outage intervals. Lane and officer
outages are half-open `[start,end)` intervals.

The event queue orders records by timestamp, event precedence, then insertion
sequence. At equal timestamps, it processes screening completions first,
resource re-openings second, resource closures third, passenger aging fourth,
and passenger arrivals fifth. Dispatch runs only after every event at that
timestamp has been processed. Therefore a completion at a lane-outage start
finishes before the lane closes, an outage end makes the resource available
at that minute, and simultaneous arrivals are all present before priority is
evaluated.

The dispatcher starts a service only when its full fixed duration fits
outside every future lane and paired-officer outage. A service may end exactly
at an outage start but may not overlap the outage; it resumes only after the
resource becomes available and is never preempted. At each start the
passenger is enqueued on the existing `SecurityLane`; at its completion the
queue head transitions to Screened. A passenger cannot start before arrival,
a lane/officer cannot run concurrent services, and each accepted passenger
must complete exactly once. Finite outages and at least one configured
eligible lane guarantee eventual service.

Mean wait is total queue wait divided by passenger count. Throughput is
completed passengers per elapsed simulated hour, measured from the first
arrival through the last completion. Lane utilization is busy screening
minutes divided by staffed, non-outage minutes in that horizon. The benchmark
runs both strategies on the same input. An unmeasured isolated correctness
preflight validates passenger coverage, assignments, service intervals,
staffing, and outage constraints before the three measured repetitions.
Measured schedules are checked against the preflight before results are
reported. Runtime is measured with `steady_clock`; the benchmark reports the
median and range over three repetitions per strategy. Linux workers use
`getrusage(RUSAGE_SELF).ru_maxrss` for process-lifetime peak RSS in KiB.
Windows workers use `GetProcessMemoryInfo` and report
`PeakWorkingSetSize` converted from bytes to KiB; that is peak resident
working set, not the Linux metric. Worker processes are isolated on each
platform. These high-water measurements include generated input and
simulation/runtime storage and depend on host conditions. The run result's
estimated result-storage bytes are a separate estimate, not measured RSS.

The existing one-extra-lane/officer comparison is retained as a sensitivity
result. The separate staffing evaluator simulates every candidate pair count
in the configured range, up to 128 lanes and 128 officers, using identical
passenger inputs and the same dispatch policy. One officer per lane is a hard
constraint. It checks optional maximum mean wait, maximum individual wait,
minimum throughput, and a resource budget. The recommendation minimizes
total staffing cost among candidates meeting all targets. Default cost is
one unit per lane plus one per officer; callers may set either unit cost and
the total budget. Equal-cost candidates are ordered by mean wait, maximum
wait, then lane count. If none meets the targets, the result has no
recommendation rather than labeling the best failed candidate feasible.

## Build and dependencies

CMake enforces standard C++20 without compiler-specific language extensions.
GoogleTest is discovered as a system package first; otherwise a fixed upstream
release is fetched by CMake. Production code has no third-party runtime
dependency.

## Test boundaries

Unit tests cover passenger validation and legal status transitions, sorting
and searching edge cases, duplicate IDs, index updates, and workload
accounting. Integration and regression tests also cover scenario CSV parsing,
arrival ordering, risk categories, lane outages, a small brute-force wait
oracle, CLI scenario execution, and CSV/JSON serialization. The full scenario
stress benchmark is a standalone runner rather than a timing-sensitive test.
