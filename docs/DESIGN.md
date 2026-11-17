# Design Decisions

## Scope boundary

Milestones 1 and 2 implement the passenger-to-screening workflow, sorting and
searching algorithms, and the passenger and queue indexes described here.
Other algorithm directories remain extension points. CSV files are provided
as fixtures; importing them remains deferred so the simulator does not imply
a parser that is not implemented.

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

## Queue policy and waiting time

Every lane is FIFO. On arrival, the queue manager assigns the passenger to the
lane with the smallest sum of queued screening durations. A tie is resolved by
lane insertion order, which makes the policy deterministic. The reported wait
is the selected lane's workload before this passenger is added. This is a
simple estimate: the simulator has no parallel service timing, preemption, or
stochastic screening durations.

Screening removes exactly one passenger from the chosen lane. The caller
supplies event time; the screening operation does not invent elapsed time or
mutate a global clock.

## Build and dependencies

CMake enforces standard C++20 without compiler-specific language extensions.
GoogleTest is discovered as a system package first; otherwise a fixed upstream
release is fetched by CMake. Production code has no third-party runtime
dependency.

## Test boundaries

Unit tests cover passenger validation and legal status transitions, sorting
and searching edge cases, duplicate IDs, index updates, and workload
accounting. Integration tests exercise registration, lane selection,
waiting-time calculation, screening, and invalid registration/routing. Future
algorithms should add focused tests in the matching test area and keep
cross-module workflow tests in `tests/integration`.
