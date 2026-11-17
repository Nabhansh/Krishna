# Milestone 1-2 Complexity Notes

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
| Classical Matrix Multiplication | O(r c k) | O(r k) result |
| Strassen Matrix Multiplication | O(n^log2(7)) padded square | O(n^2) |

`R` denotes the number of priority buckets. Sorted ID insertion shifts O(P)
elements; the insertion-point search itself is O(log P). Benchmark timings
are observations, not asymptotic proofs.
