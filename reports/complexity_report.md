# Core, Index, and Scenario Complexity Notes

Let `L` be the number of screening lanes, `P` the number of registered
passengers, `R` the number of distinct priority values, and `k` the number of
passengers in one priority bucket.

| Operation | Time | Extra space |
|---|---:|---:|
| Passenger field validation | O(1) | O(1) |
| Add a lane (including duplicate-ID check) | O(L) | O(1) |
| Route and enqueue one arrival | O(L) | O(1), excluding queue storage |
| Estimate one lane's indexed workload | Expected O(1) | O(1) |
| Screen the next passenger in a lane | O(1) | O(1) |
| Build all lane status rows | O(L) | O(L) |
| Register passenger in the sorted ID index | O(P) | O(1) extra (O(P) total index) |
| Find passenger ID (binary search + hash lookup) | O(log P) expected | O(1) |
| Read IDs for one priority bucket | O(log R + k log k) | O(k) |
| Merge Sort | O(n log n), worst case | O(n) |
| Quick Sort | O(n log n) average; O(n^2) worst case | O(log n) stack |
| Iterative Binary Search | O(log n) | O(1) |
| Recursive Binary Search | O(log n) | O(log n) call stack |
| Maximum Subarray (divide and conquer) | O(n log n) | O(log n) call stack |
| Fast Exponentiation | O(log |e|) | O(1) |
| Celebrity Problem | O(n) | O(1) |
| Classical Matrix Multiplication | O(r c k) | O(r c) result |
| Strassen Matrix Multiplication | O(n^log2(7)) padded square | O(n^2) |

`R` denotes the number of priority buckets. Sorted ID insertion shifts O(P)
elements; the insertion-point search itself is O(log P). Benchmark timings
are observations, not asymptotic proofs.

## Airport scenario runner

Let `N` be passenger count, `L` staffed lane/officer pairs, `K` resource
outage intervals, and `E <= NL` passenger-lane eligibility pairs. Each
passenger can be promoted at most twice. The timestamped event queue therefore
contains `O(N+K)` events. Dispatch scans the priority-ordered waiting
references for feasible passengers, and outage feasibility can rescan the
configured interval lists.

| Operation | Time | Storage |
|---|---:|---:|
| Stable arrival ordering | O(N log N) | O(N) temporary storage |
| Validate IDs and non-overlapping per-resource outages | Expected O(N + K^2) | O(N) ID set |
| Event creation/processing | O((N + K) log(N + K)) | O(N + K) |
| Waiting eligibility-set updates | O(E log N) | O(E) references |
| One scenario run, conservative bound | O(N^2 L K^2); O(N^2 L) with no outages | O(NL + N + K) |
| Compare two strategies and one extra-lane what-if | Three scenario runs | Three result sets retained |
| Staffing evaluation of C candidate configurations | C actual scenario runs | O(C(N + L + K)) retained results |
| Export CSV or JSON | O(output bytes) | O(1) streaming state beyond result records |

The conservative run bound accounts for searching up to `N` waiting entries
per lane for each of `N` starts and repeated feasibility checks across outage
intervals. It is an implementation upper bound; it is not a typical-case
performance claim. The simulator caps staffing at 128 lane/officer pairs, one
officer per active lane. Reported memory estimates cover returned result
objects and their string capacities, not peak process memory. Detailed
complexity for the 34 algorithms is in `ALGORITHM_ANALYSIS.md`.
