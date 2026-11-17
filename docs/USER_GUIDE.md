# User Guide

## Build

From the project root:

```sh
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Use the simulator

Start `./build/project31`. The application presents a numbered menu:

1. Register a passenger with a unique ID, name, positive screening duration,
   numeric priority, and nonnegative registration timestamp. Lower priority
   numbers are more urgent.
2. Record the arrival of a registered passenger. The queue manager assigns
   them to the lane with the least queued screening work and reports the
   pre-arrival workload as the estimated wait.
3. Screen the next passenger in a lane by its displayed lane ID (`Lane-1` or
   `Lane-2` in the default setup).
4. Display each lane's queue length and remaining queued screening workload.
5. Exit.
6. Look up a registered passenger by ID.
7. List passengers in a selected priority bucket.
8. Display registered passenger IDs sorted with Merge Sort or Quick Sort.

Times are integer minutes provided by the operator. They are event timestamps,
not a wall-clock or automatically advancing simulation clock.

Run `./build/project31_sorting_benchmark` to time Merge Sort and Quick Sort on
small (100), medium (5,000), and large (50,000) deterministic passenger-ID
datasets. Timings are informational, not pass/fail thresholds.
