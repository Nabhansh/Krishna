# Milestone 1 Demo Script

1. Build the project and run `./build/project31`.
2. Register `P-1`, name `Ada Lovelace`, screening duration `5`, registration
   time `0`.
3. Register `P-2`, name `Grace Hopper`, screening duration `3`, registration
   time `0`.
4. Record arrivals for both passengers at minute `1`. Confirm the first uses
   `Lane-1` and the second uses `Lane-2`.
5. Register `P-3`, duration `2`, then record arrival at minute `2`. Confirm
   the lane choice is based on current workload and the displayed wait is
   the workload before P-3 joins.
6. Display queues, screen a passenger from a nonempty lane, and display queues
   again to confirm the workload decreased.
7. Attempt to register a passenger with a blank ID or a zero duration and
   confirm the CLI reports the validation error.
