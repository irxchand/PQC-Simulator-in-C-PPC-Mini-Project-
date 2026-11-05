---
doc_id: "DOC-04"
title: "Software Requirements Specification"
project_id: "PQC-GRAPH-C"
version: "0.1"
date: "2026-10-22"
status: "DRAFT_FOR_REVIEW"
parent_document: "DOC-03 — Product Requirements Document, version 0.1"
parent_approval: "Direction and scope approved with seven conditions in the student's DOC-04 instruction"
baseline_status: "ESTABLISHED_FOR_DOCUMENTED_CASES"
baseline_commit: "eff9baaea6a08c22cd317449a18b89894ef020dd"
implementation_status: "NOT_STARTED"
primary_language: "C11"
next_document: "DOC-05 — Software Design Description"
evidence_bundle: "baseline-evidence.zip"
---

# DOC-04 — Software Requirements Specification

## 1. Authority, status, and scope

This SRS specifies testable behavior for the approved Graph-Based Hardware-Connectivity and Routing-Cost Analyzer for Parameterized Quantum Circuits. DOC-01 supplies the rubric and mandatory report sections; DOC-02 supplies research and feasibility boundaries; DOC-03 supplies product requirements and acceptance criteria.

The student approved DOC-03's direction and scope, requiring repository-based decisions, extraction of actual ordered CX operations, the qualified `RESTORE-3CX-v1` model, component separation, identical-workload topology comparisons, and validation of the XOR controls. Those conditions govern this SRS. Deadline information is not a dependency, and no additional faculty requirement is inferred.

**Status distinction:** the baseline findings below are observed facts. Requirements and acceptance values for the extension are specifications pending review, not implemented results. This work compiled and probed the unchanged upstream source; it did not add a Graph, BFS, estimator, production parser refactor, or new application command. The source working tree remained clean.

MUST/SHOULD/MAY retain DOC-01's meanings. Stable `SRS-04-*` IDs identify normative requirements. Supporting contracts in §§4–8 are normative when referenced by a requirement. DOC-05 decides function signatures, module files, ownership details, and implementation organization without silently changing these contracts.

## 2. Repository baseline and evidence

### 2.1 Pinned source and environment

| Item | Observed value |
|---|---|
| Repository | [PQC-Simulator-in-C-PPC-Mini-Project-](https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-) |
| Commit | `eff9baaea6a08c22cd317449a18b89894ef020dd` (`main` and HEAD at inspection) |
| Source files | `a.c` (463 lines), `circuit.txt`, `README.md`; also `Presentation.pdf` |
| Source SHA-256, `a.c` | `cbf9846fdb646a1b850023b62b0086e92cca379a97298183b0bb236ae7386f87` |
| Circuit SHA-256 | `56f391b351625bc73f380289d56a656a39f8f949fb4888f3db1343d0d239b745` |
| Verification environment | Linux x86-64, kernel 6.18.55; GCC 13.3.0; glibc 2.39; Python 3.12.14 |
| Virtualized CPU reported | AMD EPYC 9V74; this is the hosted verification environment, not a measurement of the student's laptop |
| README build | `gcc a.c -o qmlc -lm` — successful |
| Additional build | `gcc -std=c11 -Wall -Wextra -Wpedantic a.c -o qmlc-strict -lm` — successful, no warnings |
| Upstream modification | None; `git status --short` was empty after verification |

The [baseline evidence bundle](baseline-evidence.zip) contains `verify_baseline.py`, the generated verification-only C probe, fixtures, input transcripts, raw outputs, build diagnostics, and `results.json`. It excludes upstream source copies and compiled binaries. Its README gives exact replay commands against the pinned commit.

### 2.2 Source facts that determine the requirements

The following pointers refer to the pinned [`a.c`](https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-/blob/eff9baaea6a08c22cd317449a18b89894ef020dd/a.c), not an assumed future layout.

| Source | Observed behavior | Requirement consequence |
|---|---|---|
| `struct gt`, lines 19–24 | Stores gate type, target, control, and angle; CX is type 5. | Extract CX from parsed logical gate records; preserve explicit control/target roles. |
| `get_gt()`, lines 199–223 | Parses `X/H q`, `RX/RY/RZ q angle`, and `CX control target` using `sscanf`. | Preserve the valid format but validate complete tokens and finite angles. |
| `r_fl()`, lines 325–388 | Reads the qubit count then immediately executes each recognized gate; continues after bad lines. No persistent circuit sequence exists. | Introduce one validated logical sequence for file analysis and ideal execution; file errors must not produce partial successful results. This is future work, not an existing facility. |
| `fwd()`, lines 242–264 | Executes RY encoding, RY parameters, generated nearest-neighbor CXs, another RY layer, and odd-parity readout. | Retain the actual implementation. Do not replace its interactions with a hand-maintained specification. |
| CX construction, lines 253–255 | For each `i`, `c=i`, `t=i+1`. | Built-in two-qubit XOR has exactly CX(0,1). It is not a nonlocal-routing demonstration on the two-vertex line. |
| `trn()`, lines 276–323 | Two features, four parameters, 200 epochs, learning rate 0.5, central finite differences with epsilon `1e-4`. Uses `rand()` without `srand()`. | Keep settings and parameter order; record initialization explicitly in verification. Do not promise portable `rand()` sequences. |
| `make_st()`, lines 26–42; `r_cmd()`, lines 390–431 | Allocator accepts 1–30 qubits; interactive input limits to 10. Raw state storage is `sizeof(struct cx) × 2^n`. | Keep analysis independent of state allocation; apply a documented preallocation bound for file simulation. |
| `pr_p()`, lines 66–79 | Prints only probabilities above `1e-4`, to four decimals. | Verify internal probabilities/amplitudes, not a sum of displayed rounded values. |

### 2.3 Executed baseline checks

| Check | Observed result | Boundary |
|---|---|---|
| 77 statevector comparisons | Passed; maximum absolute complex-amplitude error `1.1443916996305594e-16` | Single-qubit gates at both two-qubit positions, four rotation angles, all ordered three-qubit CX pairs on every basis state, and a Bell state. Not an exhaustive correctness proof. |
| Four initialized PQC predictions versus independent mathematical-operator reference | Passed; maximum probability error `2.220446049250313e-16` | Seed-1 initialization on this environment and the four binary inputs. |
| Interactive Bell example | Two gates applied; final displayed P(00)=P(11)=0.5000 | Qubit 0 is the least-significant state index bit. |
| Supplied `circuit.txt` | Four gates applied successfully | Uses CRLF and a final line without newline; both must remain accepted. |
| Fresh-process training replay | Two outputs identical in the same environment | Upstream does not explicitly set a seed. Repeating Train within one process advances the RNG and is not the same initialization. |
| Four-qubit fixture in §9 | Eight logical gates executed successfully in upstream file mode | No topology analyzer exists yet. |
| Address/undefined sanitizers on the probe | Exit 0, empty diagnostics with leak detection disabled | Default LeakSanitizer failed because this hosted runtime restricts process inspection. Leak freedom has not been established. Both the initial failure and the subsequent ASan/UBSan logs are preserved. |

### 2.4 XOR baseline: observed values and interpretation

Input order is `[0,0], [0,1], [1,0], [1,1]`; targets are `0,1,1,0`. Classification uses `p >= 0.5` as class 1. MSE is the mean of four squared probability errors.

| Input | Encoding/readout only | Initial full PQC | Trained full PQC, printed to 4 decimals |
|---|---:|---:|---:|
| [0,0] | 0 | 0.4987879292530146 | 0.0059 |
| [0,1] | 1 | 0.3218387251802193 | 0.9956 |
| [1,0] | 1 | 0.2972578757966163 | 0.9939 |
| [1,1] | approximately `7.50e-33` | 0.8821154697701501 | 0.0045 |
| Accuracy on these same four examples | 100% | 25% | 100% |
| MSE | approximately `1.41e-65` | 0.4951665770428492 | approximately `2.79075e-5`, computed from rounded predictions |

The probe recreated initialization with `srand(1)` and called the original `trn()`; it did not implement a second training loop. Initial parameters were `[2.6395275598496144, 1.2389905055961037, 2.4601787683919056, 2.5083733435004203]`.

**Interpretation:** training improved this initialized full PQC on its four training examples. Encoding/readout alone already solves their XOR labels. These observations establish neither generalization nor a new learning method. An encoding-only ablation removes parameter and CX layers; it is not the full PQC with parameters set to zero. The latter produced `0,1,0,1` in this probe. Upstream Epoch 200 loss is printed before its final parameter update; it must not be presented as an exact post-update loss.

### 2.5 Confirmed defects and bounded corrections

| Finding | Evidence | Required treatment |
|---|---|---|
| Trailing tokens can be accepted | `get_gt("CX 0 1 extra")` and `get_gt("X 0 extra")` returned success | Reject extra tokens in shared file parsing and interactive gate input. Valid existing gate lines remain supported. |
| Nonfinite angles can be accepted | `RY 0 nan` and `RY 0 inf` both passed parsing and index checks | Reject nonfinite or out-of-range numeric conversion before execution or analysis. |
| Header may have trailing junk; invalid file lines are skipped | A file starting `2junk` executed `X 0 extra`, reported later errors, continued with `H 1`, and printed “Gates applied: 2” | Validate the complete file first and report failure without simulation or an analysis success summary. |
| State size can be impractical before allocation failure is observable | Source permits 30 qubits: approximately 16 GiB raw amplitudes on this environment | Apply explicit file-simulation limits before allocation. Do not test a huge allocation merely to reproduce an OOM. |
| `fwd()` represents allocation failure as probability zero | Lines 243–244 return `0.0` if state initialization fails | Future training/error plumbing must propagate failure distinctly; never count it as a valid class-0 prediction. |

These are planned corrections to malformed-input/error/resource behavior. No fix was applied during this specification phase, and no numerical gate defect was found in the documented gate cases.

## 3. Resolved technical decisions

These choices resolve DOC-03's open items using repository evidence and engineering judgment. They become implementation requirements when this SRS is approved.

| Decision | DOC-03 open item | Resolution and rationale |
|---|---|---|
| DEC-04-01 | OPEN-03-01 | Use existing-format circuit files as the required analyzer input. Parse once into the logical ordered gate sequence; extract CX records from that sequence. Both analysis and file simulation use the shared loader. No manual interaction-list input is required. |
| DEC-04-02 | OPEN-03-02 | Use a count-plus-edge-pairs topology file (§4) and a minimal batch-analysis CLI (§6). Preserve the no-argument menu and its existing choices. |
| DEC-04-03 | OPEN-03-03 | Use ascending physical vertex ID as BFS neighbor order, full-token parsing, fixed input bounds (§7), explicit statuses, and numeric tolerances (§8). |
| DEC-04-04 | OPEN-03-03, OPEN-03-04 | Preserve analysis circuit counts 1–30 and interactive simulation 1–10. Bound file simulation to 1–20 qubits before allocation (16 MiB raw amplitudes at 20). Reject 21–30 for simulation with a resource diagnostic, while allowing their topology analysis. This intentionally narrows the former file-mode allocation range; it is a safety/resource bound, not an experimentally optimal capacity claim. |
| DEC-04-05 | OPEN-03-04 | Correct the confirmed parsing, partial-file execution, and numeric/error-signaling defects while retaining valid gate semantics, menu modes, and training settings. Do not rewrite the simulator or optimize away normalization without separate evidence. |
| DEC-04-06 | OPEN-03-05 | Use the four-qubit parameterized-circuit fixture in §9 for the topology comparison. It is read and executed through the real circuit path; it adds no classifier architecture. |
| DEC-04-07 | All, plus student conditions | Keep the logical sequence, physical Graph, estimator, and ideal simulator separate. The graph never rewrites the logical workload in this MVP. |

The built-in XOR implementation remains supporting context; adding a production command to analyze its generated sequence is not required. Its actual ordered CX generation is documented and verified against source. If later exposed to the analyzer, it must use actual generated records or an execution trace, not a duplicated interaction list. No additional student scope decision is pending.

## 4. Input and workload contracts

### 4.1 Common text rules

Files accept LF and CRLF, a final line without newline, blank/space-only lines, spaces, and horizontal tabs. Trim surrounding spaces/tabs for tokenization. Count physical lines from 1, including blank lines, for diagnostics and provenance. Reject embedded NUL, unsupported control characters, and lines/files exceeding §7 bounds. UTF-8 BOM, comments, symbolic angles, and inline annotations are not part of either file format.

Integer tokens match `[+-]?[0-9]+`, must convert without overflow, and must satisfy their semantic bounds. In particular, decimal-point qubit IDs are invalid. Angles are decimal finite numbers in radians, including ordinary scientific notation, matching `[+-]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)(?:[eE][+-]?[0-9]+)?`. Hexadecimal floats and `nan`/`inf` are invalid. Conversion must consume the entire token, have no range error, and produce a finite `double`. Limits are acceptance bounds, not guarantees of angle accuracy for arbitrarily large arguments.

### 4.2 Circuit format

The first nonblank line is exactly one integer: the logical-qubit count `n`. Each subsequent nonblank line is exactly one of:

```text
X target
H target
RX target angle
RY target angle
RZ target angle
CX control target
```

Gate names are uppercase and case-sensitive. Each operand lies in `[0,n−1]`. For CX, control and target differ. Wrong arity, extra tokens, unknown gates, invalid operands, or invalid angles fail the entire file. A valid header with no gates is valid. Repetitions are retained, including CX pairs that would cancel if optimized; this analyzer performs no circuit optimization.

Each logical operation retains its gate ordinal (1-based, including single-qubit gates), source physical line, gate type, operands, and angle where applicable. Extracted CX occurrences additionally have a 1-based CX ordinal. The output control/target convention MUST follow source `c`/`t`, not their order in the C structure initializer (`tp,t,c,th`).

The file must be fully validated before either successful analysis output or ideal gate execution. The existing `circuit.txt` is a required compatibility fixture. Permissive legacy acceptance of malformed text is explicitly not a compatibility target.

### 4.3 Connectivity format

The first nonblank line is exactly one integer `V`: the physical-vertex count. Each later nonblank line contains exactly two integer endpoints `u v`. Example:

```text
4
0 1
1 2
2 3
```

Edges are undirected, unweighted, and have equal model cost. Vertices are `0..V−1`; isolated vertices and a graph with no edges are valid. Reject self-loops and invalid endpoints. Treat `u v` and `v u` as the same edge. Accept a repeated edge once and emit `W_DUPLICATE_EDGE` with its physical line; duplicate entries must not alter edge count, adjacency, or path selection. The raw-edge-row bound applies before deduplication.

### 4.4 Mapping and identity of the workload

Use identity mapping `logical i → physical i`, requiring `V >= n` even when some logical qubits never appear in a CX. Extra physical vertices may appear inside a route; the ideal simulator still evolves only `n` logical qubits.

The same circuit file bytes and accepted operation sequence, angles, operand roles, order, and multiplicity must be used for all topology comparisons. Record the circuit SHA-256 in the experiment evidence using external tooling; the application need not implement hashing. A file path alone is not proof that its contents stayed unchanged. The topology input changes; the workload and mapping do not.

## 5. Graph, cost, and simulator semantics

### 5.1 Separated responsibilities

| Component | Required responsibility | Boundary |
|---|---|---|
| Logical circuit | Validated ordered gate records and provenance | Contains no physical edges or inserted SWAPs. |
| Physical Graph | Adjacency-list storage and deterministic unweighted shortest paths | Represents physical adjacency only; not gate order, a statevector, or proof of entanglement. |
| Routing-cost estimator | Consume reachability/distance and calculate the named strategy's counts | Does not alter gates, simulate states, choose a persistent mapping, or predict hardware timing. |
| Ideal simulator | Execute the validated logical gate sequence with existing gate semantics | Does not consume the physical Graph or automatically route blocked interactions. |

Analysis must not allocate a statevector. This allows the accepted analysis qubit range to differ from the bounded ideal-execution range.

### 5.2 BFS and reachability

For each extracted CX, query mapped endpoint adjacency and run BFS as needed to obtain a shortest path or unreachable result. Visit neighbors in ascending vertex ID, mark discovered vertices when enqueued, and preserve the first discovered parent. This specifies a deterministic path even when multiple shortest paths exist. Early exit upon reaching the target is permitted.

A reachable result contains both endpoints and distance `path_vertex_count−1`. Classify distance 1 as `DIRECT` and greater distances as `INDIRECT`. A disconnected pair is `UNREACHABLE`; use no fake distance or cost. The underlying Graph path operation must also handle source=target as path `[source]`, distance 0 for unit testing, but such operands never form a valid analyzed CX.

### 5.3 Model `RESTORE-3CX-v1`

For a reachable valid CX with distance `d >= 1`:

\[
\text{estimated swaps}=2(d-1),\quad
\text{extra CX equivalents}=6(d-1),\quad
\text{total CX equivalents}=1+6(d-1).
\]

The strategy moves one operand until adjacent, applies the requested CX, and reverses the movement to restore the original mapping after each interaction. Each SWAP is modeled as three CX-equivalent operations. Because mapping is restored, per-occurrence estimates may be summed under this strategy.

This is an idealized gate-count proxy for the stated strategy. It is not an optimal-circuit-routing claim, native-device gate count, runtime prediction, noise model, or fidelity estimate. Undirected adjacency does not establish support for a particular directed native CX. Output must identify the model and state these limitations visibly. All single-qubit gates remain outside these counts.

For unreachable CX operations, do not call the finite-distance formula. Counts of total/direct/indirect/unreachable occurrences satisfy `total = direct + indirect + unreachable`. Repeated occurrences each count. If no CX is unreachable, full totals equal sums of per-occurrence counts. Otherwise full-workload totals are unavailable; clearly named reachable-only subtotals remain permitted. A zero-CX workload has zero counts and zero estimated overhead, even on a disconnected graph.

### 5.4 Ideal simulation

Preserve little-endian index semantics: qubit 0 is the least-significant bit; displayed kets print highest index on the left. X/H and rotations retain the mathematical conventions verified at baseline: `RX(theta)=cos(theta/2)I−i sin(theta/2)X`, `RY(theta)=[[c,−s],[s,c]]`, and `RZ(theta)=diag(exp(−i theta/2),exp(i theta/2))`. CX toggles target exactly when the control bit is 1. Preserve the existing normalization behavior unless a later documented correctness finding requires correction.

Applying the same logical circuit in the ideal simulator must be independent of which topology was analyzed. A blocked topology does not make ideal mathematical simulation undefined; output must label ideal simulation clearly and must not imply hardware executability or execution of a routed circuit.

## 6. CLI, output, and error contracts

### 6.1 Entry points

| Invocation | Required behavior |
|---|---|
| `./qmlc` | Retain menu: 1 Interactive, 2 File, 3 Train, 4 Exit. No menu renumbering. |
| `./qmlc --analyze CIRCUIT --topology GRAPH` | Validate arguments, both files, and mapping; analyze the real file's CX occurrences; print the complete report; never simulate the circuit as a side effect. |
| `./qmlc --help` | Print usage, formats/bounds summary, and reference the documented cost model; exit 0. |

The two named analysis options may occur in either order, each exactly once. Missing values, duplicate/unknown options, or extra positional arguments are usage errors. Do not add a second interaction-list input, hardware backend, routed-circuit output, multi-topology CLI, or mandatory structured-output format. The same circuit can be analyzed in three independent invocations for the experiment.

Menu file mode uses the same complete-file validation and logical sequence as batch analysis, then executes ideal simulation. Interactive mode may recover from an invalid gate line and accept the next one; the rejected gate must not mutate state, and any end-of-session summary must indicate input errors rather than imply every requested gate succeeded. Preserve `DONE`/`done` termination. Invalid interactive qubit counts must produce an explicit diagnostic rather than silently selecting a different workload.

### 6.2 Required result fields

Field meanings and tokens below are stable; table layout and whitespace are left to DOC-05. No JSON/CSV dependency is required.

| Record | Required fields |
|---|---|
| Analysis header | `circuit_file`, `topology_file`, `logical_qubits`, `physical_vertices`, `unique_edges`, `mapping=IDENTITY`, `model=RESTORE-3CX-v1`; a plain-language qualification of the estimate |
| Each CX occurrence | `cx_index`, `gate_index`, `source_line`, logical `control`/`target`, mapped `physical_control`/`physical_target`, `classification`, `adjacent`, `path`, `distance`, `estimated_swaps`, `extra_cx_equivalents`, `total_cx_equivalents` |
| Summary | `cx_total`, `cx_direct`, `cx_indirect`, `cx_unreachable`, `cost_status`, `estimated_swaps_total`, `extra_cx_equivalents_total`, `total_cx_equivalents` |
| Optional subtotal, if supplied | Explicit `reachable_subtotal_` prefix on every subtotal field |

`classification` is exactly `DIRECT`, `INDIRECT`, or `UNREACHABLE`. `adjacent` is `YES` or `NO`. Display paths as ordered physical IDs from control to target, e.g. `0->1->2->3`. Unavailable values are `NA`, never `0`, `-1`, or infinity. On a mixed workload, `cost_status=INCOMPLETE_UNREACHABLE` and all three full-total fields are `NA`; otherwise `cost_status=COMPLETE`. Duplicate warnings go to stderr, successful report data to stdout. An error report is not a successful analysis.

### 6.3 Diagnostics and process status

| Status | Code category | Examples |
|---:|---|---|
| 0 | Completed command | Valid complete analysis, including valid input with unreachable interactions; help. |
| 2 | `E_USAGE`, `E_FORMAT`, `E_RANGE`, `E_MAPPING` | Bad arguments/tokens/line length, unsupported gate, invalid indices, insufficient physical vertices. |
| 3 | `E_IO` | Open/read failure. |
| 4 | `E_LIMIT`, `E_ALLOC` | File/gate/vertex/count bound exceeded, simulation above its resource cap, allocation failure, size arithmetic failure. |
| 5 | `E_NUMERIC`, `E_INTERNAL` | Nonfinite state/probability detected after valid input, broken internal invariant. |

Diagnostics must include the stable code, a short explanation, and file/physical-line information where applicable. Lexically numeric but nonfinite or unrepresentable input angles are input errors (`E_RANGE`, status 2), not successful gate operations. A disconnected pair is a valid analysis result, not an input error.

Fully validate and construct required analysis results before printing an analysis header or success summary. A failing batch command exits nonzero, prints the diagnostic to stderr, and emits no successful report. Release allocations on all paths. Menu actions may return to the menu after a diagnostic; the process need not exit on every action error, but the failed action must never be labeled successful. A clean menu exit/EOF retains upstream behavior.

## 7. Bounds, determinism, and resource requirements

| Quantity | Required supported bound | Rationale/behavior |
|---|---|---|
| Logical qubits in circuit file for analysis | 1–30 | Matches the existing accepted file declaration range without requiring state allocation. |
| File-mode ideal simulation | 1–20 | At 20, raw statevector storage is about 16 MiB with two 8-byte doubles. Enforce before allocation; 21–30 give `E_LIMIT`. |
| Interactive ideal simulation | 1–10 | Preserve the current interactive supported range, with explicit invalid-input rejection. |
| Built-in XOR | 2 qubits, 4 trainable parameters | Preserve the existing experiment. |
| Physical vertices | 1–64, with `V >= n` | Bounded educational topology; allows additional routing vertices. |
| Unique undirected edges | At most `V(V−1)/2` (maximum 2016) | No self-loops or duplicate storage. |
| Raw topology edge rows | At most 10,000 | Includes duplicate rows before deduplication. |
| Logical gates / CX occurrences | At most 10,000 gates; CX count is at most gate count | All logical occurrences retained; zero gates allowed. |
| File bytes | At most 1,048,576 bytes per input file | Bound applies to bytes including line terminators. |
| Content bytes per physical line | At most 255, excluding LF or CRLF | Reject an overlong line; never process it as multiple logical lines. |

These bounds are conservative product choices, not measured hardware capacities or performance claims. DOC-05 must use checked arithmetic before allocation and choose integer types sufficient for all bounds. At V=64, a shortest reachable distance is at most 63: at most 124 SWAPs, 372 extra CX-equivalents, and 373 total per occurrence. At 10,000 CX occurrences, the maximum full modeled total is 3,730,000. Use exact integer arithmetic for all graph/count/cost fields; do not use floating-point counters or sentinel values that can enter arithmetic.

One adjacency-list BFS has traversal time O(V+E) and auxiliary storage O(V); the Graph has O(V+E) storage. Neighbor sorting or ordered construction occurs outside the per-query BFS traversal. For C CX occurrences, uncached analysis may take O(C(V+E)); caching is not required. Fixed inputs must produce the same paths/counts irrespective of topology edge-row insertion order or duplicates. Runtime benchmarking and performance superiority targets remain excluded.

The application must build with a documented C11 compiler and libm on the verified Linux CPU environment; no GPU, Python, Qiskit, or network is required to run the core application. Python is verification tooling only. Other platforms may be documented after actual testing; no Windows compatibility result is asserted here.

## 8. Numerical and XOR validation contracts

Gate/state regression tests use independent known-answer/reference amplitudes with absolute complex error at most `1e-12` for the small baseline gate cases. For documented short composite circuits and PQC probabilities, use absolute error at most `1e-10`. Check finite amplitudes/probabilities, probability range within tolerance, and norm error at most `1e-10`; do not silently clamp serious violations into valid results. These are verification tolerances for defined cases, not universal guarantees for every accepted angle/circuit length.

CLI four-decimal probability output may be compared within `5e-5` plus the internal probability tolerance. Do not use that display precision for high-precision numerical validation, and do not treat omitted probabilities below `pr_p()`'s threshold as zero in internal checks. A normalization pass must not substitute for validating correct gate amplitudes. Comprehensive tests of unnormalized gate arithmetic may be designed in DOC-07; this SRS does not require removing normalization.

The XOR verification must compare three conditions on the same ordered four inputs: encoding plus readout only; initialized full PQC; trained full PQC. Include zero-parameter full-PQC output as a control explaining why it differs from encoding only. Report MSE and threshold-0.5 accuracy with denominators and data reuse explicit. No minimum accuracy gain is an acceptance condition.

Preserve the upstream training settings unless a later evidence-backed correction is separately recorded. Verification must capture seed or initial parameter vector, actual initial/final parameters or equivalent full-precision predictions, epoch count, learning rate, epsilon, platform/compiler/libc, and whether loss was measured before or after update. Use the real production forward and training paths; a test must not verify a separately reimplemented trainer in place of them. No new public audit CLI or optimizer is required; DOC-05 may expose a small internal verification interface.

At minimum, repeat a fixed-initialization run in two fresh processes on the same environment. Compare unrounded corresponding predictions within `1e-6`; record any difference and its cause rather than promise bitwise equality across compilers/libc. A second training run in the same upstream menu process must not be mislabeled as the same initialization. Seed/vector control for verification must not silently change the legacy menu's initialization behavior.

Allocation/numeric failures in training must stop the affected run with an explicit failure status; they must not become ordinary probabilities or enter gradient/MSE calculation. Report observed training improvement and the encoding shortcut together. The four examples are both training and evaluation data here; do not call their accuracy held-out performance.

## 9. Required topology experiment and fixtures

The following **specified fixture** is already accepted by, and was executed in, the unchanged ideal simulator. Its fixed numbers are one setting of a parameterized circuit family; they are not newly trained model weights.

```text
4
RY 0 0.3
RY 1 -0.2
RY 2 0.5
RY 3 0.7
CX 0 1
CX 0 3
CX 2 3
RZ 3 0.4
```

It has eight gates, with ordered CX records `(0,1),(0,3),(2,3)` at gate ordinals 5,6,7 and physical source lines 6,7,8. Analyze exactly this file with identity mapping against these three four-vertex graphs. Expected analyzer values below are hand-derived acceptance values, not observed analyzer output.

| Graph | Undirected edge set | Distances in CX order | Class counts (direct, indirect, unreachable) | Full totals (SWAP, extra CX-eq., CX-eq. including logical CXs) |
|---|---|---|---|---|
| Line | `{0–1,1–2,2–3}` | 1,3,1 | 2,1,0 | 4,12,15 |
| Line plus shortcut | `{0–1,1–2,2–3,0–3}` | 1,1,1 | 3,0,0 | 0,0,3 |
| Disconnected | `{0–1,2–3}` | 1,NA,1 | 2,0,1 | NA,NA,NA; optional reachable-only subtotal 0,0,2 |

The line's CX(0,3) path is `0->1->2->3`. The shortcut graph's corresponding path is `0->3`. The disconnected graph must identify CX occurrence 2 as unreachable. Ideal simulation of the logical file is the same computation regardless of these graph choices; do not insert SWAPs or adjust RY/RZ angles between comparisons.

The repeat-occurrence fixture from DOC-03 AC-03-05 remains a separate required check: `(0,1),(0,3),(0,3)` on the line gives 3 occurrences, 1 direct, 2 indirect, and totals 8,24,27. Its consecutive equal CXs must not be canceled by the analyzer.

## 10. Normative requirement register and traceability

Each row inherits the detailed contracts above. `VT-04-*` identifies the verification obligation in §11, not a completed extension test. All rows are MUST requirements.

| ID | Required behavior | DOC-03 parent requirement(s) | Verification |
|---|---|---|---|
| SRS-04-001 | Before changes, pin/replay the baseline and record any source drift; preserve valid modes and document justified corrections. | FR-03-01 | VT-04-01, VT-04-14 |
| SRS-04-002 | Use one validated ordered logical sequence for file analysis and ideal file execution, with provenance. | FR-03-04; NFR-03-02 | VT-04-02, VT-04-03 |
| SRS-04-003 | Enforce complete-token grammar, finite angles, valid operands, and full-file validation before execution/reporting. | FR-03-09; NFR-03-05 | VT-04-04, VT-04-12 |
| SRS-04-004 | Extract actual CX records in order, preserving control/target roles, source lines, and repetitions. | FR-03-04 | VT-04-03, VT-04-08 |
| SRS-04-005 | Build an undirected adjacency-list Graph with the specified edge grammar and duplicate policy. | FR-03-02; NFR-03-02 | VT-04-05 |
| SRS-04-006 | Enforce identity mapping and sufficient physical vertices for all declared logical qubits. | FR-03-05 | VT-04-04, VT-04-09 |
| SRS-04-007 | Return adjacency and deterministic BFS shortest paths or explicit unreachable results. | FR-03-03; NFR-03-03 | VT-04-06, VT-04-07 |
| SRS-04-008 | Apply only `RESTORE-3CX-v1` integer calculations to valid reachable CX records. | FR-03-06 | VT-04-08, VT-04-09 |
| SRS-04-009 | Report the specified per-CX fields and consistent complete/incomplete/zero-CX summaries. | FR-03-07 | VT-04-08, VT-04-09 |
| SRS-04-010 | Keep logical circuit, Graph, estimator, and ideal simulator semantically separate; analysis allocates no statevector. | NFR-03-02; FR-03-01, FR-03-04 | VT-04-02, VT-04-10, VT-04-12 |
| SRS-04-011 | Preserve menu choices and add only the defined analysis/help CLI, with specified option validation. | FR-03-01, FR-03-09 | VT-04-11 |
| SRS-04-012 | Implement diagnostics/status distinctions, no false success on failure, and no partial file execution. | FR-03-09 | VT-04-04, VT-04-11, VT-04-12 |
| SRS-04-013 | Enforce bounds before allocation, checked arithmetic, exact counters, and safe cleanup/error propagation. | NFR-03-05 | VT-04-12 |
| SRS-04-014 | Retain validated gate/state semantics and distinguish internal numerical checks from rounded display. | FR-03-01; NFR-03-05 | VT-04-14 |
| SRS-04-015 | Validate encoding-only, initialized, trained, and zero-parameter controls using actual production QML functions and qualified metrics. | FR-03-10 | VT-04-15 |
| SRS-04-016 | Reproduce fixed-initialization QML verification with explicit parameters/settings and appropriate tolerances. | FR-03-10; NFR-03-03 | VT-04-15 |
| SRS-04-017 | Demonstrate the unchanged logical workload against the three specified connectivity graphs and record input identity. | FR-03-08 | VT-04-10 |
| SRS-04-018 | Meet adjacency-list storage/BFS complexity bounds and deterministic neighbor selection without requiring optimization or timing claims. | NFR-03-03, NFR-03-04 | VT-04-07, VT-04-13 |
| SRS-04-019 | Build/run the documented C11 CPU application without GPU, quantum SDK, Python, or network runtime dependencies. | NFR-03-01 | VT-04-13 |
| SRS-04-020 | Clearly label ideal simulation and the estimate's assumptions; never claim optimal routing or real hardware runtime. | FR-03-06, FR-03-07; DOC-03 §5 | VT-04-09, VT-04-10, VT-04-16 |
| SRS-04-021 | Supply actual reproducible evidence for DOC-01 REP-01–09, preserving RUB-01–03 and distinguishing existing code from the extension. | DEL-03-01 | VT-04-16 |
| SRS-04-022 | Produce the required Vite + React presentation after the C core/tests stabilize, accurately reflecting implemented behavior and evidence. | DEL-03-02 | VT-04-16 |

## 11. Verification obligations

DOC-07 must turn these obligations into executable test cases and commands. Existing baseline checks are evidence for upstream behavior only; all affected cases must run against the eventual extension. Exact graph/count/cost/status expectations require exact equality; numerical checks use §8 tolerances.

| ID | Required verification | Parent acceptance coverage |
|---|---|---|
| VT-04-01 | Pin commit/source hashes, build the unchanged source, replay baseline evidence, and record differences if HEAD changes. No silent retargeting. | AC-03-01 |
| VT-04-02 | Show that analyzer and file simulator consume the same validated logical records; topology changes leave operations unchanged; analysis does not call state allocation. | AC-03-05, AC-03-10 |
| VT-04-03 | On the §9 fixture, extract exactly the three CXs with the stated gate/source/CX ordinals; preserve reversed operand roles and repeated operations in dedicated cases. | AC-03-05 |
| VT-04-04 | Reject missing/bad headers, unknown/lowercase gates, wrong arity, trailing junk, noninteger IDs, negative/out-of-range IDs, CX self-operands, nonfinite/overflowing/underflowing angle conversion, mapping failure, embedded NUL, and overlong lines. Invalid files must execute no gate and print no successful analysis. | AC-03-06 |
| VT-04-05 | Graph creation/adjacency tests cover V=1, no edges, isolated vertices, disconnected graphs, reversed duplicate edges, and duplicate row limits. Invalid topology rows fail; duplicate warnings identify physical lines. | AC-03-04, AC-03-06 |
| VT-04-06 | Verify line distances/paths and reachable/unreachable classifications, including Graph API self-distance 0. An invalid CX must not reach estimation. | AC-03-02–04 |
| VT-04-07 | On diamond edges `{0–1,0–2,1–3,2–3}`, BFS(0,3) returns `0->1->3`; permuting rows or adding duplicates does not change it. Check path edges and minimal length against a small independent exhaustive-path oracle. | AC-03-11 |
| VT-04-08 | Check d=1 → 0/0/1 and d=3 → 4/12/13; check repeat fixture totals 8/24/27; distance/count units and logical-CX inclusion must match. | AC-03-02, AC-03-03, AC-03-05 |
| VT-04-09 | Verify mixed reachable/unreachable totals are NA, reachable-only subtotals are explicitly labeled if present, zero-CX totals are zero, and extra physical vertices may form a path without increasing logical state dimension. | AC-03-04, AC-03-07 |
| VT-04-10 | Run the §9 unchanged fixture on all three graphs, checking expected distances/classes/costs and fixed input hash; verify identical ideal simulation semantics. Capture actual outputs later. | AC-03-08 |
| VT-04-11 | Exercise legacy menu modes, `--help`, both option orders, missing/duplicate/unknown arguments, unreadable/missing files, valid disconnected-analysis exit 0, nonzero batch error statuses, and interactive recovery. | AC-03-01, AC-03-06 |
| VT-04-12 | Exercise accepted bounds and one-above cases for file bytes/line size/vertices/gates/raw edges/qubit counts, simulation cap before allocation, 30-qubit analysis with a sufficient Graph and no state allocation, injected allocation failures, read errors, numeric failures, and resource cleanup. Do not actually request dangerous oversized statevectors. | AC-03-06, AC-03-10 |
| VT-04-13 | Clean C11 CPU build, warnings review, algorithm/storage/code review, and sanitizer checks appropriate to the environment. Separately report leak-check availability; do not label the hosted LeakSanitizer restriction an application defect or a passing leak test. | AC-03-10 |
| VT-04-14 | Replay the gate/PQC reference tests and valid interactive/file regressions. Any corrected error behavior is listed explicitly; no unexplained valid-input regression remains. | AC-03-01, AC-03-09 |
| VT-04-15 | Execute the actual production XOR paths and specified ablations, record high-precision values/settings, compare same-initialization repeats, and disclose training/evaluation reuse and the encoding shortcut. Require no positive gain. | AC-03-09 |
| VT-04-16 | Review mandatory report sections, real screenshots/outputs and commands, rubric traceability, limits/model wording, student explanations, and the later Vite + React presentation. | AC-03-12 |

Rubric links remain: **RUB-01** uses the explicit PQC workload and supporting XOR analysis; **RUB-02** uses the implemented Graph/BFS/estimator, shared workload extraction, and tests; **RUB-03** uses explainability, reproducible topology contrast, honest differentiation, and limitations. These are evidence links, not a predicted mark allocation or novelty claim.

## 12. Handoff, approval gate, and remaining work

All five DOC-03 open technical decisions have specified resolutions in §3. No unresolved scope or academic decision requires a student answer at this point. The following remain work to perform later, not missing product requirements:

1. Review this SRS, including the explicit file-simulation cap and malformed-input compatibility corrections.
2. In DOC-05, define modules, logical operation representation, parser/Graph/estimator interfaces, data ownership, error propagation, deterministic adjacency construction, and a minimal production-QML verification interface. Do not add a manual pair-list workflow or couple topology to ideal arithmetic.
3. In DOC-06, produce ordered tasks referencing SRS IDs, baseline findings, expected files, exclusions, and evidence. Resolve source drift before tasks become executable.
4. In DOC-07, implement the test-plan detail for VT-04-01–16, including allocation-failure injection and numeric-reference strategy. Baseline probes can inform the plan but do not replace extension tests.
5. Only after the relevant specifications are approved, implement and review the extension through DOC-08. Preserve baseline source/evidence and document every intentional behavior correction.

This phase ends with a reviewable DOC-04 and reproducible baseline evidence. No implementation authorization is inferred from approval to write this SRS.

## Appendix A. Baseline reproduction

Extract `baseline-evidence.zip` into a writable directory and run from that directory:

```bash
git clone https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-.git upstream
git -C upstream checkout eff9baaea6a08c22cd317449a18b89894ef020dd
python3 verify_baseline.py "$PWD/upstream"
```

The script builds outside the upstream working tree and regenerates `results.json` and raw transcripts. It uses only the standard Python library and GCC/libm/sanitizers. It includes unchanged `a.c` in a verification-only probe with the menu entry point renamed at preprocessing time. The independent reference applies mathematical operator elements in Python; the actual upstream `trn()` remains the trainer being exercised.

The original README compile and ordinary menu replay can also be reproduced directly:

```bash
cd upstream
gcc a.c -o ../qmlc -lm
printf '2\ncircuit.txt\n4\n' | ../qmlc
printf '3\n4\n' | ../qmlc
```

Baseline stdout hashes are recorded to identify captured runs, not imposed as cross-platform correctness requirements. No speed benchmark was performed.

---

**End of DOC-04.**
