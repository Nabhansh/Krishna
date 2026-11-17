# Release Preparation Checklist

This is a release-readiness checklist, not a claim that a release has been
produced. The current repository does not define CMake install/package rules.

## Required verification

- [x] Configure and perform fresh Debug and Release builds with a working
      C++20 toolchain.
- [x] Run the complete CTest suite; retain all 171 Phase 1 tests and confirm
      all eight Phase 2 QA tests are included (179 tests in each build).
- [x] Run the three-scenario executable and compare the normal, shock, and
      generated 50,000-passenger results with their exported CSV/JSON.
- [x] Parse every generated JSON with a standard JSON parser and every CSV
      with a CSV parser; verify counts, assignments, waits, strategy names,
      run IDs, and metrics against the scenario results.
- [x] Run the airport scenario benchmark and verify its preflight, repetition,
      median/range, platform-specific measured peak-memory, and estimated-storage reporting. Document the
      qualitative comparison and RSS limitations; do not infer superiority.
      Numeric host-specific timing/RSS values are not copied into this report.
- [x] State explicitly whether assignment differences changed service
      intervals or aggregate service metrics; do not infer operational
      superiority from runtime alone.
- [x] Run the existing sorting and four-matcher examples/benchmarks.
- [x] Run `git diff --check` and inspect generated output for private or
      non-synthetic passenger data before distribution.
- [x] Re-read README, user guide, design guide, demo script, product audit,
      complexity, recurrence, reduction, and algorithm reports.
- [ ] Test from a clean checkout and confirm commands are run from the project
      root when relying on the relative `data/` directory.

## Product and packaging limitations to resolve before a production release

- [ ] Decide whether CMake install/package targets are required and add them
      before publishing binary packages.
- [ ] Review the deterministic fixed-duration model with airport operations
      stakeholders. It does not simulate stochastic arrivals, shifts/breaks,
      interrupted service, or live airport systems.
- [ ] Confirm risk-priority scheduling, aging, and one-officer-per-lane
      constraints meet the intended educational/demo requirements; they are
      not an operational security policy.
- [ ] Obtain successful fresh Debug and Release builds/tests for the final
      source state on Linux and MSVC Windows; the preceding Fedora baseline
      passed 179/179 before the portability changes.

Algorithm verification alone does not satisfy this release checklist or
establish that a complete production airport product is ready.
