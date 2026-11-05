---
doc_id: "DOC-07"
title: "Test Plan and Test Cases"
project_id: "PQC-GRAPH-C"
version: "0.1"
date: "2026-10-22"
status: "READY_FOR_FINAL_REVIEW"
parents: ["DOC-04 v0.1", "DOC-05 v0.2", "DOC-06 v0.1"]
baseline_commit: "eff9baaea6a08c22cd317449a18b89894ef020dd"
implementation_status: "NOT_AUTHORIZED_OR_STARTED"
test_execution_status: "PLANNED_NOT_EXECUTED_AGAINST_EXTENSION"
---

# DOC-07 — Test Plan and Test Cases

## 1. Purpose and practical execution

Implement these tests alongside DOC-06's tasks, after coding is authorized. This plan adds no production interface or architecture. Reuse the exact DOC-05 test files and actual production functions. All cases below are **planned acceptance checks**, not reported passes. Previously observed baseline results remain only those in DOC-04 and `baseline-evidence.zip`.

Use standard C assertions/a small C runner, Python standard-library references/subprocess checks, and the compiler sanitizers already selected. No test framework dependency is required. Keep production objects separate from PQC_TESTING objects. `make test-unit` runs all currently introduced C cases; `make test-reference` runs the production reference probe and independent references; `make test-analysis-only` excludes sim/qml linkage. `make test` combines these and `python3 tests/test_cli.py --stage all` without requiring a duplicate manual CLI run. The CLI script's `--stage menu` option runs only existing-mode checks during T06-05; `all` is its default after T06-06. Test scripts read `PQC_TEST_APP` (default `./qmlc`) and `PQC_TEST_PROBE` (default `build/reference_probe`). The sanitize target selects `build/sanitize/qmlc` and `build/sanitize/reference_probe` through those variables and runs its sanitized C executables too, rather than accidentally checking ordinary binaries. These variables select verification executables only; they add no production runtime controls.

`tests/reference_probe.c` emits named observations with `%.17g` through the public sim/qml APIs; it has no second trainer. Python compares actual amplitudes/forward outputs with independent mathematical operators using the same little-endian convention. Unit tests inspect returned statuses, owner invariants, and retained contents. CLI tests inspect records/fields and process statuses rather than compiler-dependent whitespace or source stdout hashes. A timeout means failure/investigation, not proof that performance is inadequate; use a generous configurable test-process timeout (default 120 seconds), with no product timing claim.

## 2. Baseline replay and numerical oracles

**TC-07-01 baseline commands**, from the existing repository root, using a dedicated clean sibling checkout:

```bash
git rev-parse HEAD
git status --short
git diff eff9baaea6a08c22cd317449a18b89894ef020dd -- a.c circuit.txt
git worktree add --detach ../pqc-baseline-eff9baa eff9baaea6a08c22cd317449a18b89894ef020dd
python3 -m zipfile -e docs/baseline-evidence.zip build/baseline-replay
python3 build/baseline-replay/verify_baseline.py ../pqc-baseline-eff9baa
```

If the dedicated worktree already exists, verify its commit/clean state and reuse it; never overwrite another checkout. Preserve the archive bytes. Record environment, source SHA-256, output and failures. The expected pinned `a.c` hash is in DOC-04/06. This replay exercises the old baseline, not new module behavior. Keep new implementation evidence separate.

Use exact equality for statuses, paths, classifications, source ordinals, counts and costs. For the baseline's small gate cases use absolute complex-amplitude error <=1e-12; for specified short composite circuits/PQC probabilities use <=1e-10. Norm error is `abs(sum(abs(amplitude)^2) - 1)` and must be <=1e-10 for the specified valid states. Independent norm/probability references use Python `math.fsum` or wider long-double accumulation on the verified Linux environment; do not reuse the production Kahan routine as its own oracle. CLI four-decimal values allow 5e-5 plus the internal probability tolerance. Fixed-initialization repeated unrounded predictions allow 1e-6; record causes of any difference. Never substitute rounded CLI output for precise internal checks.

## 3. Cases and expected outcomes

| Case | Files / task | Checks and acceptance |
|---|---|---|
| **TC-07-01 Baseline** | Archived verifier; T06-01 | Pin/replay the original build and recorded valid menu/file/training/gate behavior. Record any drift; do not retarget or rewrite the archived evidence. |
| **TC-07-02 Allocation/accounting** | `test_common.c`; T06-01, T06-07 | Check status/exit mapping and checked-size overflow. Positive malloc/calloc and realloc(NULL,n): exactly one attempt and one new live block on success, unchanged live count on failure. Existing-block realloc success preserves bytes/live count; injected failure preserves original pointer/bytes/live count. Test zero requests, realloc(existing,0), free(NULL), and calloc overflow per amended DES-05-03. Never inspect an old pointer after successful realloc or demand that growth physically moves. |
| **TC-07-03 Circuit ownership** | `test_circuit.c`; T06-01, T06-07 | Valid header-only Circuit, ordered appends/source lines, designated control/target roles, invalid gate rejection, idempotent destroy. Inject failure at the initial vector allocation and at append 17 after capacity 16. Count/capacity/pointer/retained operations stay unchanged on failure; later cleanup restores starting live count. |
| **TC-07-04 Graph ownership** | `test_graph.c`; T06-02 | V=1, no edges, isolated vertices, symmetric adjacency and unique edge count. Reversed duplicate inserts no nodes. Reject self-loops/out-of-range endpoints. Fail Graph/head allocation and each of the two edge-node allocations; no half-edge or owner leak remains. |
| **TC-07-05 BFS** | `test_graph.c`; T06-02 | Line path 0->1->2->3 has distance 3; adjacent distance 1; disconnected path unavailable; API self-path [source], distance 0. Diamond `{0–1,0–2,1–3,2–3}` returns 0->1->3 regardless of row order/duplicates. Validate edges/minimal length against exhaustive simple-path enumeration on small fixed graphs, not on 64-vertex graphs. Check a 64-vertex path and bounded queue/reconstruction. |
| **TC-07-06 Valid parser/provenance** | `test_input.c`; T06-03 | Original circuit.txt, LF/CRLF, final no-newline, blank/space-only lines, tabs, signed integers within bounds and decimal/scientific angles accepted. All six gates retain order and roles. On the DOC-04 fixture, CX gate ordinals 5/6/7 and physical lines 6/7/8 are exact; inserted blank lines change only physical-line provenance. Topology duplicates emit W_DUPLICATE_EDGE and store once. |
| **TC-07-07 Invalid parser/atomicity** | `test_input.c`, later `test_cli.py`; T06-03, T06-06 | Malformed/missing header, unknown/lowercase opcode, wrong arity/trailing tokens, noninteger/invalid IDs, equal CX operands, NUL/BOM/bare CR, malformed angles and unsupported syntax fail. Format errors -> E_FORMAT; nonfinite/overflow/underflow angle conversion -> E_RANGE. A bad final record leaves the returned owner empty. File-mode CLI executes no gate and prints no successful simulation/analysis summary for an invalid file. |
| **TC-07-08 Read/loader failures** | `test_input.c`; T06-03, T06-07 | Missing/unreadable files and injected checked-read errors return E_IO. Fail reader-buffer allocation, vector growth, topology construction and duplicate-warning writes. Exercise each observed loader allocation attempt on small fixtures; close/free temporary resources, preserve primary diagnostic, and return empty outputs. A readonly output stream may test write failure without a new production mock layer. |
| **TC-07-09 Cost/provenance** | `test_analysis.c`; T06-04 | d=1 gives swaps/extra/total 0/0/1; d=3 gives 4/12/13; distance 0 is rejected. From the real parsed fixture extract `(0,1),(0,3),(2,3)` with exact CX/gate/line ordinals. Reversed operand roles are retained. The separate repeated sequence `(0,1),(0,3),(0,3)` on the line counts all three and totals 8/24/27. |
| **TC-07-10 Mapping/summary** | `test_analysis.c`, `analysis_only_test.c`; T06-04 | V<n -> E_MAPPING even if unused logical qubits have no CX. Zero-CX -> complete zero totals; mixed blocked -> full costs unavailable, never zero. Extra physical vertices may form a route without changing logical dimensions. Build/run 30-qubit analysis without sim/qml objects. Inject row-allocation failure: empty Analysis and no owned-block leak. |
| **TC-07-11 Gate/state regression** | `test_sim.c`, reference probe/Python; T06-05, T06-07 | Port the baseline's 77 gate/Bell comparisons to public interfaces, including complex phases and both CX directions. Check independent short-circuit amplitudes and actual odd-parity probability. Invalid gates do not mutate valid state; NaN/Inf/zero norm or norm error >1e-10 returns E_NUMERIC, marks unusable state as specified, and cleanup succeeds. Empty/error outputs never masquerade as probability zero. |
| **TC-07-12 Compensated 20-qubit checks** | `test_sim.c`, reference probe/Python; T06-05, T06-07 | Use the dense/skewed/valid-circuit fixtures below. Compare production checks/normalization/readout to an independent accurate sum; valid identity gates must not be falsely rejected. Retain existing tolerances and numerical-failure rejection. Allocate/destroy these states sequentially, not simultaneously or across a cross-product of maximal workloads. |
| **TC-07-13 Actual XOR paths** | `test_qml.c`, reference probe/Python; T06-05, T06-07 | Actual encoding-only, initial full PQC, trained full PQC and zero-parameter full-PQC controls on ordered inputs 00/01/10/11. Record initial/final vector, high-precision probabilities, MSE, threshold accuracy, settings and pre/post-update loss meaning. Repeat the same initial vector in two fresh processes. Preserve menu RNG consumption; do not reseed production or implement another optimizer. Inject early, gradient-perturbation and late forward allocation failures; abort/clean up without using fake predictions. |
| **TC-07-14 Bounds** | Module tests + CLI; T06-07 | Check each independent cap and one-above: file 1,048,576 bytes, line 255 content bytes, V=64, 10,000 gates/raw edge rows, logical analysis n=30, file simulation n=20, interactive n=10. File/gate/count/vertex/simulation cap excess -> E_LIMIT; overlong line -> E_FORMAT. n/V<=0 or invalid operand -> E_RANGE. Above simulation cap fails before amplitude allocation. Do not combine 20 qubits with 10,000 simulated gates merely to test two bounds. |
| **TC-07-15 CLI/menu/report** | `test_cli.py`; T06-05 menu, T06-06 all | Preserve modes 1–4 and valid Bell/file/Train behavior. Invalid interactive count has no silent fallback; invalid gate leaves state unchanged; long terminal line drains correctly. Check help, both batch option orders, missing/duplicate/unknown arguments -> exit 2; missing file -> 3; resource limit -> 4. Valid disconnected analysis -> 0. Verify NOTE qualifications, ordered fields, escaped paths, stderr warnings, NA costs, and no successful report on validation failure. Test output failure/flush detection on the verified Linux target. |
| **TC-07-16 Same-workload experiment** | `test_cli.py`, examples; T06-06 | Run the unchanged DOC-04 §9 circuit against line/shortcut/disconnected graphs. Expected summaries and commands are below. Record circuit SHA-256 before/after; compare ideal logical probabilities independently of topology. No gate insertion/cancellation or angle changes. |
| **TC-07-17 Build/sanitizers** | Makefile, all applicable tests; T06-07 | Clean C11 CPU build, warnings review, production objects without test controls, O(V+E) BFS/storage review, separate ASan/UBSan build. No GPU/quantum SDK/Python/network needed to run qmlc. Resolve real findings. If the hosted LeakSanitizer restriction recurs, disable only leak checking and label it unavailable; never call that a passing leak test. |
| **TC-07-18 Evidence/delivery review** | README, actual outputs/report/presentation; T06-08 and post-core delivery | Reproduction commands, hardware/software, model assumptions, source attribution, real screenshots and honest XOR controls support DOC-01 REP-01–09/RUB-01–03. The final report and Vite + React presentation must actually exist and reflect the completed core to pass their portions. Leave undelivered components pending; do not infer whole-project completion from core tests. |

### 3.1 Exact numerical fixtures for TC-07-12

- **Dense:** a test-owned 20-qubit SimState with N=2^20 amplitudes all real `2^-10`; squared norm is 1. Apply identity RX(0), a representative H/rotation and inverse pair, and verify norm/readout against accurate sums.
- **Skewed counterexample:** let `p=2^-54`, `tail=(N−2)*p`. Set internal amplitudes to real `2^-27`, with endpoints `sqrt(0.6*(1−tail))` and `sqrt(0.4*(1−tail))`. For the review regression variant, scale once using the naive sequential norm in **test preparation only**. The accurately measured norm remains within 1e-10 while sequential checking may cross it. Apply RX(0) through the actual production interface; require OK and accurately normalized output. Log the discrepancy, but do not require a platform-specific rounded discrepancy value as the oracle.
- **Circuit-generated:** start from sim_create(20), apply RY(i, `0.01*(i+1)`) for i=0..19, then CX(i,i+1) for i=0..18 through production gates. Require finite normalized state and accurate odd-parity sum. This is a short supported circuit, not a maximal-depth timing benchmark.
- **Negative controls:** NaN, Inf, all-zero amplitudes, and a finite state with squared norm 1+1e-6 must fail with the specified unusable/cleanup behavior. The test-owned mutation is a verification seam, not a new state-loading product feature.

### 3.2 Failure-injection scope

For small constructor/loader/append/edge/analysis operations, first run successfully to measure allocation attempts, then inject each attempt in turn with fresh owners and the same input. Disable injection before final cleanup; compare live-block counts and object contents to the proper starting state. Check both wrapper-level realloc(NULL,n) and Circuit's chosen initial/growth paths. A live-count check is not a substitute for content preservation or sanitizer checking.

Do not exhaustively rerun the entire 200-epoch trainer once for every allocation number. Exercise selected distinct production paths: initial observation, base prediction, positive/negative finite-difference perturbations, and final observation, using successful-run counters and the known forward-call order to select boundaries. Require unchanged caller-owned initial parameters, no leaked temporary state, and explicit failure propagation. Do not add training callbacks or another allocator framework.

### 3.3 TC-07-16 commands and exact expected summaries

```bash
./qmlc --analyze examples/pqc4.circuit --topology examples/line4.graph
./qmlc --analyze examples/pqc4.circuit --topology examples/shortcut4.graph
./qmlc --analyze examples/pqc4.circuit --topology examples/disconnected4.graph
```

| Topology | Distances in CX order | Direct/indirect/unreachable | SWAP/extra-CX/total-CX full totals |
|---|---|---|---|
| Line `{0–1,1–2,2–3}` | 1,3,1 | 2/1/0 | 4/12/15 |
| Line plus `{0–3}` | 1,1,1 | 3/0/0 | 0/0/3 |
| Disconnected `{0–1,2–3}` | 1,NA,1 | 2/0/1 | NA/NA/NA |

For the upper-cost integer case, form a 64-vertex path ordered `[0,30,31,...,63,1,2,...,29]`. A logical n=30 circuit containing 10,000 occurrences of CX(0,29) has d=63, giving per-CX 124/372/373 and totals 1,240,000/3,720,000/3,730,000. Analyze only; allocate no statevector. This exercises the specified maximum cost without a large simulation.

## 4. Coverage of existing verification obligations

| DOC-04 obligation | Cases |
|---|---|
| VT-04-01 | TC-07-01, TC-07-18 |
| VT-04-02 | TC-07-06, TC-07-07, TC-07-10, TC-07-16 |
| VT-04-03 | TC-07-06, TC-07-09 |
| VT-04-04 | TC-07-07, TC-07-14, TC-07-15 |
| VT-04-05 | TC-07-04, TC-07-06, TC-07-14 |
| VT-04-06 | TC-07-05, TC-07-09, TC-07-10 |
| VT-04-07 | TC-07-05 |
| VT-04-08 | TC-07-09, TC-07-16 |
| VT-04-09 | TC-07-10, TC-07-15 |
| VT-04-10 | TC-07-16 |
| VT-04-11 | TC-07-15 |
| VT-04-12 | TC-07-02–04, TC-07-08, TC-07-10–14 |
| VT-04-13 | TC-07-05, TC-07-10, TC-07-17 |
| VT-04-14 | TC-07-11–13, TC-07-15 |
| VT-04-15 | TC-07-13 |
| VT-04-16 | TC-07-18 |

DOC-05 §12 already maps all 22 SRS requirements to these obligations. Retain that map and this case coverage; do not create a second requirement hierarchy. Test names/fixtures and make targets here are fixed verification conventions for DOC-06, not new application features.

## 5. Completion and evidence rules

For each executed case record case ID, candidate commit, command, observed result/exit status and log location in the task response or existing evidence records. Capture stdout/stderr separately where statuses matter. A failure stops the affected increment until explained/fixed. Mark unavailable environment checks explicitly; unrun tests remain pending. Reuse an unchanged passing run rather than repeating broad checks without cause.

**Core acceptance:** relevant TC-07-01–17 checks pass, no unexplained regression or ownership failure remains, and reviewed numerical/model claims match evidence. **Academic completion:** TC-07-18's report and presentation portions also pass after those deliverables exist. The final planning review checks DOC-05 v0.2, DOC-06, and this plan for consistency once; subsequent authorized work is incremental Codex implementation, not another documentation expansion.
