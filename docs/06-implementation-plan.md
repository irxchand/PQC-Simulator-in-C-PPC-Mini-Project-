---
doc_id: "DOC-06"
title: "Implementation Plan and Technical Tasks"
project_id: "PQC-GRAPH-C"
version: "0.1"
date: "2026-10-22"
status: "READY_FOR_FINAL_REVIEW"
parents: ["DOC-04 v0.1", "DOC-05 v0.2"]
baseline_commit: "eff9baaea6a08c22cd317449a18b89894ef020dd"
implementation_status: "NOT_AUTHORIZED_OR_STARTED"
companion: "DOC-07 — Test Plan and Test Cases"
---

# DOC-06 — Implementation Plan and Technical Tasks

## 1. Execution rule

This is the final planning packet with amended DOC-05 and DOC-07. It authorizes no code changes. After final review and explicit implementation authorization, Codex works incrementally **in the existing repository**, using the selected DOC-05 files/interfaces. No additional architecture or planning document is required before coding.

Preserve DOC-01's rubric/report requirements, DOC-02's narrowed scope, DOC-03's product requirements, all 22 SRS IDs, and DES-05-01–14. Use the pinned baseline above and immutable `baseline-evidence.zip`; expected source SHA-256 is `cbf9846fdb646a1b850023b62b0086e92cca379a97298183b0bb236ae7386f87`. Keep the approved documents/evidence available in `docs/` as the implementation input packet; copy them without regeneration if not already there.

One task at a time. Implement its production behavior and corresponding tests together, run the task's checks, review the diff, then report evidence. Advance only when required checks pass or a documented environment limitation has been reviewed. Do not modify unrelated user changes, silently retarget the baseline, skip failures, or expand scope.

## 2. Staged build/test conventions

All commands below are **future acceptance commands**, not executed results. Run from the repository root. DOC-07 defines each test case. Use the C11 flags and production/test separation already selected in DOC-05.

| Command | Introduced | Meaning |
|---|---|---|
| `make all` | T06-01 | Build the currently integrated application; it must remain runnable. |
| `make test-unit` | T06-01 | Build/run the C runner for all module tests introduced so far, with PQC_TESTING. |
| `make test-analysis-only` | T06-04 | Build/run the analysis-only executable without sim/qml linkage. |
| `make test-reference` | T06-05 | Run the production reference probe plus independent Python checks. |
| `python3 tests/test_cli.py --stage menu` | T06-05 | Verify the migrated existing menu modes before batch analysis is wired. |
| `python3 tests/test_cli.py --stage all` | T06-06 | Verify all menu/batch/help/status/report behavior. |
| `make test` | T06-06 | Run all current unit, analysis-only, reference, and CLI checks. |
| `make sanitize` | T06-07 | Run applicable checks with the separate sanitizer objects selected in DOC-05. |

The extra task-level make targets only expose the existing test executables; they are not new production architecture. Add tests incrementally; do not create placeholders that pass unimplemented behavior. Before T06-05, `a.c` continues to provide the unchanged legacy menus. New core modules are independently tested but do not yet replace that path. **T06-05 performs the engine/trainer/menu migration together**, so no accepted intermediate build drops Train or retains two production gate engines/trainers. After migration, all eight production C files use the DOC-05 build command.

## 3. Dependency-ordered tasks

Dependencies are sequential: T06-01 → 02 → 03 → 04 → 05 → 06 → 07 → 08. Stable interfaces come from DOC-05; local implementation details must not change those contracts.

### T06-01 — Establish the starting point and shared foundations

- **Precondition:** final implementation authorization; existing working tree inspected. Verify the pinned source/hash, or explain any source drift before touching it. Documentation-only commits may follow the baseline; do not reset the repository to hide them.
- **Files:** `common.h/.c`, `circuit.h/.c`, `Makefile`, `.gitignore`, `tests/test_main.c`, `tests/test_common.c`, `tests/test_circuit.c`. Keep `a.c` behavior unchanged.
- **Work:** replay the immutable baseline in an isolated pinned checkout; implement selected statuses, diagnostics, checked arithmetic, allocation wrappers/accounting, Circuit ownership/growth and gate validation. Establish the cumulative C runner and ignored build outputs.
- **Links:** SRS-04-001, 003, 012, 013; DES-05-02–04, 14; VT-04-01, 04, 12; TC-07-01–03.
- **Commands:** DOC-07 §2 baseline replay; `make all test-unit`; `git diff --check`.
- **Accept/evidence:** baseline replay log and environment, current source diff/hash, passing foundation tests including realloc(NULL,n), failed growth and retained bytes, unchanged valid legacy menu smoke behavior. No Graph/simulator rewrite or production test controls.

### T06-02 — Implement physical Graph and BFS

- **Precondition:** T06-01 accepted.
- **Files:** `graph.h/.c`, `tests/test_graph.c`, runner registration and Makefile source lists.
- **Work:** selected sorted linked adjacency lists, atomic two-node edge insertion, duplicate detection, deterministic BFS/path reconstruction, explicit unreachable results, and idempotent destruction.
- **Links:** SRS-04-005, 007, 013, 018; DES-05-05, 06, 14; VT-04-05–07, 12, 13; TC-07-04, 05.
- **Commands:** `make all test-unit`; `git diff --check`.
- **Accept/evidence:** exact line/diamond paths, insertion-order invariance, isolated/unreachable/self-path cases, both edge-node allocation failures leave Graph unchanged and leak no owned blocks. No matrix, DFS, caching, weights, or routing execution.

### T06-03 — Implement shared validated input

- **Precondition:** T06-02 accepted; Circuit and Graph interfaces available.
- **Files:** `input.h/.c`, `tests/test_input.c`, runner/Makefile registration.
- **Work:** the selected bounded reader, strict shared gate parser, complete Circuit/topology loaders, physical source lines, duplicate warnings, terminal-line handling, and test-only read-error seam. No gate executes while loading.
- **Links:** SRS-04-002, 003, 005, 012, 013; DES-05-04, 07, 14; VT-04-02–05, 12; TC-07-06–08.
- **Commands:** `make all test-unit`; `git diff --check`.
- **Accept/evidence:** unchanged `circuit.txt` accepted with CRLF/final unterminated line; bad late records return empty outputs; limits and read/allocation failures clean up. Legacy menu parsing is replaced at T06-05, not by creating a second user input route now.

### T06-04 — Implement CX analysis and the cost model

- **Precondition:** T06-03 accepted.
- **Files:** `analysis.h/.c`, `tests/test_analysis.c`, `tests/analysis_only_test.c`, runner/Makefile registration.
- **Work:** extract actual ordered CX records from Circuit, validate identity mapping, compute paths and tagged RESTORE-3CX-v1 costs, preserve repetitions, and build complete/incomplete summaries transactionally.
- **Links:** SRS-04-004, 006, 008–010, 013, 018, 020; DES-05-06, 08, 12, 14; VT-04-03, 08–10, 12; TC-07-09, 10.
- **Commands:** `make all test-unit test-analysis-only`; `git diff --check`.
- **Accept/evidence:** exact costs/counts, NA availability semantics, zero-CX and blocked cases, actual source provenance, and 30-qubit analysis linked without sim/qml. No manually duplicated CX list or emitted SWAP circuit.

### T06-05 — Migrate the existing simulator, QML, and menus atomically

- **Precondition:** T06-04 accepted and the original baseline remains available outside the candidate source path.
- **Files:** `sim.h/.c`, `qml.h/.c`, `a.c`, `tests/test_sim.c`, `tests/test_qml.c`, `tests/reference_probe.c`, `tests/test_reference.py`, `tests/test_cli.py`, runner/Makefile registration.
- **Work:** move the existing gate formulas and one trainer into their selected modules; add explicit status plumbing, Kahan accumulation, numerical guards, QML observations, and the existing four menu adapters. Wire file mode to the complete shared loader and interactive mode to the same gate parser. Remove superseded engines, parser, and training loop from `a.c` in this task. Batch analysis is wired next.
- **Links:** SRS-04-001–003, 010–016, 019; DES-05-01, 02, 07, 09–11, 13, 14; VT-04-02, 04, 11, 14, 15; TC-07-11–13, menu subset of TC-07-15.
- **Commands:** `make all test-unit test-reference`; `python3 tests/test_cli.py --stage menu`; `git diff --check`.
- **Accept/evidence:** preserved valid gate/QML behavior and menu numbering, encoding-only/initial/trained/zero-parameter observations, dense/skewed 20-qubit checks, no partial invalid-file execution, explicit failures instead of fake zero probabilities. Source review confirms only one engine/trainer. No new optimizer, seed CLI, or Graph-dependent arithmetic.

### T06-06 — Wire batch analysis and reproduce the topology comparison

- **Precondition:** T06-05 accepted.
- **Files:** `a.c`, the four `examples/` files named in DOC-05, `tests/test_cli.py`, Makefile.
- **Work:** add only the specified help/batch entry points, report layout, quoting, stable fields/statuses, warning/error streams, and the same-workload three-topology experiment. Freeze example bytes from DOC-04 §9.
- **Links:** SRS-04-004, 009–012, 017, 020; DES-05-08, 11–13; VT-04-03, 09–11; TC-07-15, 16.
- **Commands:** `make all test`; `python3 tests/test_cli.py --stage all`; `git diff --check`. The explicit CLI command is an alternative to its inclusion in `make test`, not a required duplicate run.
- **Accept/evidence:** line totals 4/12/15; shortcut totals 0/0/3; disconnected full costs NA; identical circuit hash and logical simulation; valid disconnected analysis exits 0. Incorrect input produces no successful report. No frontend or hardware execution.

### T06-07 — Complete boundary, failure, and sanitizer validation

- **Precondition:** T06-06 accepted; all features available.
- **Files:** existing test/probe files and Makefile; production files only for narrowly justified defects found by these tests.
- **Work:** finish DOC-07 failure sweeps, realloc preservation, independent bounds, 20-qubit numerical cases, fixed-initialization training repeats, stdout/read errors, and isolated sanitizer builds. Keep earlier checks cumulative.
- **Links:** SRS-04-003, 012–016, 018, 019; DES-05-03, 07, 09, 10, 13, 14; VT-04-04, 12–15; TC-07-02, 03, 08, 11–14, 17.
- **Commands:** `make test`; `make sanitize`; `git diff --check`. If the observed hosted LeakSanitizer restriction recurs, use the documented `ASAN_OPTIONS=detect_leaks=0 make sanitize` fallback and record leak checking as unavailable.
- **Accept/evidence:** no unexplained failures/regressions, cleanup returns owned-block counts to their starting values, accurate numerical checks, and sanitizer findings resolved or environment limitations explicitly bounded. Do not change scope/thresholds to make failing cases disappear.

### T06-08 — Close the core handoff with actual evidence

- **Precondition:** T06-07 accepted.
- **Files:** existing `README.md`; generated evidence under ignored `build/evidence/`. No new planning document is required.
- **Work:** document actual build/run commands, formats, limits, assumptions, corrected legacy behaviors, topology results and QML controls. Capture real CLI screenshots/output and link the core evidence to DOC-01's nine report sections. Reuse the last passing full-suite run if code is unchanged; rerun affected checks only if a fix is made.
- **Links:** SRS-04-001, 017, 020–022; DES-05-12, 13 and §13; VT-04-01, 10, 16; TC-07-18.
- **Accept/evidence:** reviewer can reproduce the core and understand its limits. The mandatory final report and Vite + React presentation remain required post-core academic deliverables; complete their content from actual results after core review, never invent results or build the frontend ahead of the C core. TC-07-18 remains pending for any undelivered report/presentation component; core success is not whole-project completion.

## 4. Evidence and stop conditions

Each task response contains only: task ID and relevant starting/ending commit; changed files and behavior; checks actually run with exit/result and log paths; unresolved issues/environment limits; and whether its acceptance criteria passed. Local commits may mark accepted increments, but do not push, merge, deploy, or overwrite unrelated work without authorization.

Stop the affected task and explain the smallest correction if source drift changes assumptions, approved interfaces conflict, a required test fails unexplained, an output/model claim exceeds evidence, or a fix needs scope/architecture changes. Routine local coding choices within the specified interfaces do not require a new planning round. Never describe an unrun check as passing. Preserve the baseline archive and old-document IDs.

**Final-review gate:** check DOC-05 v0.2, this plan, and DOC-07 together once. After implementation is explicitly authorized, the next action is **T06-01 in the existing repository**, then incremental tasks/tests. This planning session has authorized and executed none of those production tasks.
