---
doc_id: "DOC-03"
title: "Product Requirements Document"
project_id: "PQC-GRAPH-C"
version: "0.1"
date: "2026-10-22"
status: "DRAFT_FOR_REVIEW"
project_direction_status: "APPROVED"
implementation_status: "NOT_STARTED_BY_THIS_DOCUMENT"
parent_documents:
  - "DOC-01 — Project Brief and Vision, version 1.0"
  - "DOC-02 — Research and Feasibility"
primary_language: "C"
presentation_stack: ["Vite", "React"]
source_repository: "https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-"
next_document: "DOC-04 — Software Requirements Specification"
---

# DOC-03 — Product Requirements Document

## 1. Purpose, authority, and evidence

**Product:** Graph-Based Hardware-Connectivity and Routing-Cost Analyzer for Parameterized Quantum Circuits in C.

This PRD converts DOC-02's narrowed recommendation into product requirements. It references DOC-01 for the academic rubric, mandatory report sections, baseline-preservation rules, and established constraints. It does not replace those documents.

MUST, SHOULD, and MAY have the meanings defined in DOC-01 §0.3. Requirement IDs in this document are scoped to DOC-03. A requirement defines intended behavior; it is not evidence that behavior is implemented.

### 1.1 Confirmed decisions

| ID | Status | Decision and evidence |
|---|---|---|
| DEC-03-01 | APPROVED | Adopt DOC-02's narrowed connectivity/routing-cost analyzer direction. The student answered “DO IT” to the explicit scope-approval question. This includes the recommended CPU-only adjacency-list/BFS scope and retention/validation of the simulator and XOR demo. |
| DEC-03-02 | APPROVED | Do not use deadline or available-hours tracking as a planning dependency. The student answered “IRRELEVANT.” No submission date is inferred. |
| DEC-03-03 | APPROVED | The supplied rubric is the complete current academic requirement set. The student answered “THAT IS IT” when asked about additional faculty instructions. |
| DEC-03-04 | APPROVED | Proceed with DOC-03. Implementation still follows the approved-specification workflow in DOC-01 and the master prompt. |

### 1.2 Reconciliation with earlier documents

DOC-01 §5 proposed a graph of circuit interactions. DOC-02 §§6–8 narrowed the primary graph to **physical-qubit connectivity**. Under DEC-03-01, the latter is the product direction. The circuit's ordered two-qubit operations are analyzed against that graph; the connectivity graph does not replace the circuit or encode its complete execution semantics.

DOC-01's broad feature candidates remain candidates unless adopted here. CUDA and cross-framework speed comparisons are excluded from this MVP. The XOR experiment supplies QML context, subject to validation; it is not the primary proof of the new analyzer's usefulness.

The supplied files are Markdown-escaped exports. Later repository copies SHOULD use normal Markdown and valid YAML front matter while preserving their meaning and stable IDs. Their contents have not been edited by this PRD.

### 1.3 Evidence boundary

- **FACT — supplied documents:** DOC-02 reports a static inspection of `a.c`, `circuit.txt`, and `README.md`, with no executed baseline or numerical validation.
- **FACT — this session:** the current repository source could not be retrieved. No current commit, circuit grammar, build result, simulator output, or model result has been independently verified here.
- **UNKNOWN:** current source compatibility, baseline defects, and actual XOR learning behavior.
- **PROPOSAL:** detailed product behavior below. Approval of the project direction does not imply that this newly written PRD has already been reviewed or implemented.

External research findings remain attributed to DOC-02 §13. This PRD establishes no new novelty or performance claim.

## 2. Problem, users, and objectives

### 2.1 Problem statement

A small logical PQC can request a CX interaction between qubits that are not adjacent in a supplied hardware-connectivity model. The student needs an explainable way to identify direct, indirect, and unreachable interactions and quantify a simplified routing overhead. The existing ideal simulator alone does not establish whether an interaction satisfies those connectivity constraints.

The product MUST connect the analysis to real circuit operations or a documented PQC interaction specification. It MUST support comparison of the same interactions against different connectivity graphs without changing the logical workload.

### 2.2 Intended users

| User | Product need |
|---|---|
| Student/developer | Understand and demonstrate adjacency lists, BFS, connectivity constraints, and the cost calculation in C. |
| Student studying QML | Inspect how connectivity affects interactions used by a PQC while retaining the existing classifier demonstration. |
| Academic evaluator | Reproduce deterministic outputs, inspect runnable code, and assess the Graph's functional role and the student's understanding. |

These are intended educational users, not evidence of external customer demand.

### 2.3 Objectives

| ID | Verifiable objective |
|---|---|
| OBJ-03-01 | Use a real Graph and BFS in the analyzer's main workflow. |
| OBJ-03-02 | Produce correct, reproducible classifications, paths, and estimates for known examples. |
| OBJ-03-03 | Integrate analysis with a traceable circuit/PQC workload and preserve validated simulator functionality. |
| OBJ-03-04 | Produce evidence supporting DOC-01 RUB-01–03 and REP-01–09 without unsupported claims. |
| OBJ-03-05 | Keep every core component explainable at second-year undergraduate level and runnable on CPU. |

## 3. Scope and boundaries

### 3.1 Required MVP

1. Retain and validate the existing C simulator and XOR demonstration.
2. Represent a user-supplied unweighted, undirected connectivity graph using adjacency lists.
3. Analyze ordered CX occurrences from an existing-format circuit or a documented PQC interaction specification.
4. Query adjacency and use BFS to find shortest paths or establish unreachability.
5. Report per-interaction results and a circuit summary under the fixed cost model in §5.
6. Demonstrate the same suitable PQC/circuit workload on contrasting topologies.
7. Supply deterministic graph/cost tests, simulator regression evidence, reproduction instructions, and actual report outputs.
8. Build the required Vite + React presentation after the core C implementation and tests are stable.

### 3.2 Exclusions

| Item | Status | Reason |
|---|---|---|
| Full transpilation, emitted routed circuits, persistent remapping, optimal layout search | EXCLUDED_FROM_MVP | The product analyzes connectivity and estimates a named strategy's cost. |
| Directed/weighted hardware models, native-gate constraints, calibration, noise, fidelity or latency prediction | EXCLUDED_FROM_MVP | These require a different cost model and evidence. |
| CUDA, remote GPU setup, Qiskit/Aer dependency, cross-framework speed competition | EXCLUDED_FROM_MVP | They do not support the required critical path. |
| New classifier architecture, large datasets, topology-versus-learning study | EXCLUDED_FROM_MVP | Preserve the existing experiment and validate its interpretation first. |
| DFS, degree statistics, component enumeration, GUI graph editor, general SDK | DEFERRED | Add only after a demonstrated requirement justifies the work. |

Two small topology fixtures used in acceptance testing are required evidence, not a commitment to a topology-generation feature. No accuracy-improvement or speed-superiority target is imposed.

## 4. Inputs, outputs, and core workflows

### 4.1 Input contracts

| Input | Accepted product form | Required meaning and validation |
|---|---|---|
| Circuit | Existing repository circuit text format, using its verified supported gate syntax | Preserve accepted legacy syntax. Validate declarations and operands before analysis; inspect current `circuit.txt` and parser before freezing exact grammar in DOC-04. |
| PQC interaction specification | A documented ordered sequence of CX operand pairs plus declared logical-qubit count, associated with a named PQC | Allowed where PQC operations are generated internally. Document its source function/layer and prove that the occurrences match the actual PQC. An arbitrary unrelated pair list is insufficient integration. |
| Connectivity | A text edge list declaring physical-qubit count and unordered endpoint pairs | Physical IDs are zero-based contiguous integers. Isolated vertices are valid. Reject out-of-range endpoints and self-loops; repeated undirected edges SHOULD be accepted once with an explicit notice. |
| Mapping | Identity mapping for the MVP | Logical qubit `i` corresponds to physical vertex `i`. Require enough physical vertices for all declared logical qubits, including those unused in CX operations. Extra physical vertices MAY be intermediate routing vertices. |
| Cost model | Fixed model `RESTORE-3CX-v1` (§5) | Display the model and its assumptions. No interactive model selection or custom cost weights are required. |

The circuit and PQC specification are alternative workload routes. DOC-04 MUST choose the smallest route supported by source inspection and record the choice. Supporting both is not an MVP requirement. At least one route MUST be demonstrably connected to the simulator/PQC; implementing a second parser solely for feature count is discouraged.

Exact new-file grammar, numeric limits, CLI invocation, and error codes belong in DOC-04. These remain technical decisions owned by the requirements agent; the student's deadline is not a dependency. No existing circuit grammar is invented here.

### 4.2 Core workflows

| ID | Workflow | Expected result |
|---|---|---|
| UC-03-01 | Load a valid workload and connectivity graph; request analysis | Ordered CX occurrences receive connectivity/path/cost results and an aggregate summary. |
| UC-03-02 | Analyze the same workload against a second graph | Workload and mapping remain identical; changes in adjacency, distance, and estimated overhead are visible. |
| UC-03-03 | Analyze a valid disconnected graph | Unreachable operations are identified; full-circuit cost is unavailable, with any reachable-only subtotal clearly labeled. |
| UC-03-04 | Run the associated circuit or existing XOR demo in the simulator | Show ideal simulation/model results with their source and validation status; distinguish them from topology analysis. |
| UC-03-05 | Supply invalid input or encounter a resource/file error | Explain the problem and its location/context; do not present a successful analysis. |

### 4.3 Output contract

Each analyzed CX occurrence MUST include its ordinal, logical control/target, mapped physical endpoints, classification (`DIRECT`, `INDIRECT`, or `UNREACHABLE`), and the applicable fields below.

| Field | Reachable interaction | Unreachable interaction |
|---|---|---|
| Physical adjacency | Yes for distance 1; otherwise no | No |
| Shortest path | Ordered vertex IDs, including endpoints | Unavailable |
| Distance | Number of path edges | Unavailable; never a fake large distance |
| Estimated SWAP count | `2(d−1)` | Unavailable |
| Extra CX-equivalent count | `6(d−1)` | Unavailable |
| Total CX-equivalent count for this interaction | `1 + 6(d−1)` | Unavailable |

The summary MUST identify the workload, topology, qubit counts, edge count, mapping, cost model, and counts of total/direct/indirect/unreachable CX occurrences. Repeated CX occurrences MUST count separately. If all are reachable, report summed SWAP, extra-CX-equivalent, and total-CX-equivalent counts. If any are unreachable, mark full-workload cost unavailable; a subtotal MAY describe reachable interactions only.

These counts cover analyzed CX operations and their estimated routing additions. They do not count all single-qubit gates or represent the full native-gate cost of a circuit. A valid circuit containing zero CX operations MUST report zero analyzed interactions and zero routing overhead, with that scope stated.

## 5. Routing model and interpretation

**Model ID:** `RESTORE-3CX-v1`.

For distinct endpoints connected by a shortest path of `d ≥ 1` edges, move one logical state along `d−1` edges until the operands are adjacent, apply the requested CX, then reverse those SWAPs to restore the initial mapping.

\[
S(d)=2(d-1),\qquad E(d)=3S(d)=6(d-1),\qquad T(d)=E(d)+1.
\]

`S` is estimated SWAP count, `E` is extra CX-equivalent count, and `T` includes the requested logical CX. Each SWAP is modeled as three CX-equivalent operations. Unweighted undirected edges have equal cost; directionality, device-specific gate support, cancellations, scheduling, and errors are ignored.

Because mapping is restored after every interaction, reachable per-occurrence estimates can be added under this model. This is the cost of the specified strategy, not proof of a globally minimal routing cost. An interaction whose control equals its target is invalid CX input; distance-zero path handling in the Graph API is an internal test concern, not a valid cost-estimation case.

The CLI, report, and presentation MUST state that results are idealized gate-count estimates. Physical adjacency under this model does not certify a directed native CX on a named device. Ideal simulation outputs MUST be identified as such: this MVP does not execute an emitted routed circuit or simulate routing noise.

## 6. Product requirements

| ID | Requirement | Priority | Acceptance |
|---|---|---|---|
| FR-03-01 | The product MUST preserve existing simulator modes unless a demonstrated defect requires a documented correction. | MUST | AC-03-01 |
| FR-03-02 | The analyzer MUST use an adjacency-list Graph for the supplied connectivity; graph construction and queries MUST support isolated and disconnected vertices. | MUST | AC-03-02, 04 |
| FR-03-03 | The analyzer MUST determine adjacency and use BFS to return a shortest unweighted path or an unreachable result. | MUST | AC-03-02–04 |
| FR-03-04 | The analyzer MUST consume ordered CX occurrences from at least one traceable workload route in §4.1 and preserve operand roles and repetitions. | MUST | AC-03-05 |
| FR-03-05 | The product MUST enforce identity mapping and validate all declared qubits and operands against their respective ranges. | MUST | AC-03-06 |
| FR-03-06 | The estimator MUST implement `RESTORE-3CX-v1`, without applying its finite-cost formula to unreachable or invalid interactions. | MUST | AC-03-02–04, 06 |
| FR-03-07 | The product MUST provide the per-occurrence fields and aggregate behavior in §4.3. | MUST | AC-03-05, 07 |
| FR-03-08 | The submission MUST demonstrate the same suitable PQC/circuit interactions on contrasting graphs. | MUST | AC-03-08 |
| FR-03-09 | Input and runtime failures MUST receive a clear diagnostic and MUST NOT produce a success summary. | MUST | AC-03-06 |
| FR-03-10 | The existing XOR demo MUST be retained, with encoding/readout control, initial-versus-trained results, and repeatability assessed before learning claims. | MUST | AC-03-09 |
| NFR-03-01 | Core implementation and tests MUST run on CPU without a GPU or quantum SDK dependency. | MUST | AC-03-10 |
| NFR-03-02 | Graph/BFS/cost logic MUST be separable from gate arithmetic and explainable through small functions and documented data structures. | MUST | AC-03-10 |
| NFR-03-03 | Repeated runs on identical inputs MUST produce identical classifications, distances, estimates, and a reproducibly selected shortest path. The path tie rule MUST be specified in DOC-04. | MUST | AC-03-11 |
| NFR-03-04 | The BFS design MUST have an adjacency-list traversal bound of O(V+E), with O(V) auxiliary traversal storage. No wall-clock target or speed superiority claim is required. | MUST | AC-03-10 |
| NFR-03-05 | Accepted sizes, allocation failures, resource release, and arithmetic bounds MUST be handled explicitly; numeric caps and tolerances MUST be defined in DOC-04 after baseline inspection. | MUST | AC-03-06, 10 |
| DEL-03-01 | The report MUST address DOC-01 REP-01–09 using actual implementation evidence and identified limitations. | MUST | AC-03-12 |
| DEL-03-02 | The Vite + React presentation MUST explain the stable implementation, Graph/BFS demonstration, measured results, and assumptions after core validation. | MUST | AC-03-12 |

## 7. Acceptance criteria and success metrics

These criteria define evidence to collect later; no result below is claimed as an observed implementation output.

| ID | Acceptance criterion | Required evidence |
|---|---|---|
| AC-03-01 | Record the repository commit, build command, baseline outputs/failures, and repeatable checks before modification. After integration, preserved behavior passes regression checks or justified corrections are documented. | Before/after logs and regression results. |
| AC-03-02 | On line edges `{0–1,1–2,2–3}`, CX(0,1) is direct with path `[0,1]`, distance 1, 0 SWAPs, 0 extra CX-equivalents, total 1. | Graph/path and estimator checks plus CLI output. |
| AC-03-03 | On that line, CX(0,3) returns `[0,1,2,3]`, distance 3, 4 SWAPs, 12 extra CX-equivalents, total 13. | Hand-worked calculation and matching program output. |
| AC-03-04 | With declared vertices 0–3 and edges `{0–1,2–3}`, CX(0,3) is unreachable and has no finite path/distance/cost. | Disconnected and isolated-vertex tests. |
| AC-03-05 | A documented workload's extracted/specified CX occurrences match its actual operations in order and count. For `[CX(0,1), CX(0,3), CX(0,3)]` on the four-vertex line, counts are 3 total, 1 direct, 2 indirect; totals are 8 SWAPs, 24 extra CX-equivalents, and 27 including the requested CXs. | Source-to-workload trace and integration output. |
| AC-03-06 | Cover malformed files, missing files, invalid IDs, equal CX operands, insufficient physical vertices, self-loops, duplicates, oversize input, and supported allocation-failure handling. Invalid input receives a diagnostic without a success result; duplicate handling follows the documented policy. | Negative/boundary tests; resource checks appropriate to the C build environment. |
| AC-03-07 | A mixed reachable/unreachable workload marks full cost unavailable and never silently omits blocked interactions. A valid zero-CX workload produces explicit zero counts/overhead. | Summary checks for both cases. |
| AC-03-08 | Use a documented four-or-more-qubit PQC/circuit example containing a nonlocal CX. Analyze the identical workload on a line, a topology making that pair adjacent, and a disconnected topology. Explain the changed results. | Input fixtures, workload trace, outputs, and report explanation. |
| AC-03-09 | Report encoding/readout-only control, initialized model, and trained model on the same XOR examples using documented initialization, training settings, metric definitions, and repeat conditions. Apply explicit numerical checks before conclusions. No minimum training gain is required. | Actual model outputs/metrics, gate/probability validation, and qualified interpretation. |
| AC-03-10 | A clean documented CPU build runs the required modes and tests without GPU/quantum SDK dependencies. Code review explains Graph storage, BFS queue/visited/parent behavior, path reconstruction, complexity, and resource/error handling. | Build/test log, environment details, source review, and viva notes. |
| AC-03-11 | On a graph with multiple shortest paths, repeated runs follow the documented tie rule and return a valid minimum-length path with identical estimates. | Repeat-run and path validity checks. |
| AC-03-12 | All nine mandatory report sections are present, evidence has reproducible commands and real screenshots, and the Vite + React presentation accurately shows the completed system. | Report checklist, final artifacts, and presentation verification. |

**Completion metrics:** all MUST requirements have passing linked evidence; all required known-answer and invalid-input cases pass; no unexplained baseline regression remains; core results are reproducible; all report sections and the required presentation are complete. A failed baseline or model test must be resolved or explicitly bounded before the affected claim can pass review.

**Understanding metric:** the student can explain why the Graph is needed, trace one BFS, derive the cost at distance three, explain an unreachable result, distinguish ideal simulation from connectivity analysis, and describe the XOR control experiment. Complexity, feature count, and training accuracy alone are not completion measures.

## 8. Rubric and report traceability

| DOC-01 criterion | Product evidence | Acceptance links | Report sections |
|---|---|---|---|
| RUB-01 — Recent AIML relevance (2 marks) | A named PQC workload and a clear explanation of connectivity constraints relevant to QML circuit use; retained, validated classifier context. | AC-03-05, 08, 09 | REP-01–03, 08–09 |
| RUB-02 — Implementation and technical complexity (4 marks) | C Graph, BFS, path reconstruction, cost estimator, real workload integration, regression and boundary checks. | AC-03-01–07, 10–11 | REP-04–08 |
| RUB-03 — Innovation, applicability, understanding (4 marks) | Transparent educational implementation, contrasting topology evidence, derivable estimates, limitations, and viva explanation. Differentiate project-specific work from established techniques using DOC-02's references. | AC-03-03, 08–10, 12 | REP-01–04, 06, 08–09 |

This mapping supports evaluation; it does not predict marks or establish scientific novelty. REP-05 MUST describe the actual build/test hardware and software. REP-07 MUST identify runnable source and reproduction commands. REP-08 MUST contain actual outputs rather than illustrative acceptance values presented as measurements.

## 9. Risks, dependencies, and open technical decisions

| ID | Risk/dependency | Required action | Owner |
|---|---|---|---|
| RISK-03-01 | Current source and circuit grammar are unverified. | Inspect and pin the repository; record parser behavior and baseline evidence before freezing input-level requirements or implementation tasks. | DOC-04 author / implementation agent |
| RISK-03-02 | Analyzer is detached from QML. | Require the PQC/circuit trace and topology comparison in AC-03-05/08. A standalone pair-query demo cannot satisfy integration alone. | DOC-04 author / student |
| RISK-03-03 | Estimated totals are misread as hardware cost or optimal routing. | Display model identity, restoration assumption, units, unavailable costs, and interpretation limits. | Implementation / report author |
| RISK-03-04 | XOR encoding creates a misleading learning claim. | Complete AC-03-09; report weak or absent training gains honestly. Analyzer acceptance is independent of a positive training result. | Implementation / report author |
| RISK-03-05 | Existing simulator defects are inherited or hidden. | Validate gates/probabilities and preserve baseline evidence; isolate fixes and explain their effects. | Implementation agent |
| RISK-03-06 | Documentation or optional features consume core effort. | Keep required functionality and evidence bounded to this PRD; use DOC-04/05 for details rather than duplicating them here. | Project architect |

| Open ID | Decision to resolve | Recommended direction | Owner / resolution point |
|---|---|---|---|
| OPEN-03-01 | Which workload route best fits current code? | Reuse the existing parsed circuit operations if straightforward; otherwise use a documented internally generated PQC interaction sequence with explicit source trace. | DOC-04, after repository inspection |
| OPEN-03-02 | Exact connectivity grammar, file conventions, and CLI behavior | One compact text edge-list format and minimal CLI integration; preserve existing circuit syntax. | DOC-04 |
| OPEN-03-03 | Numeric bounds, error codes, tie rule, and test tolerances | Small demonstration circuits, deterministic shortest-path selection, explicit size/allocation/error handling, and justified numerical tolerances. | DOC-04 / DOC-07 |
| OPEN-03-04 | Baseline defects and extent of required correction | Make only evidence-backed corrections needed for valid preserved functionality. | Baseline review before coding tasks |
| OPEN-03-05 | Exact PQC fixture and source-trace mechanism | A small documented parameterized circuit containing a nonlocal CX, without adding a new learning architecture. | DOC-04 / DOC-05 |

These are technical work items, not requests for more student preferences. They do not prevent review of this PRD. They do prevent unsupported claims that the implementation specification is already complete.

## 10. Handoff to DOC-04

The next artifact is **DOC-04 — Software Requirements Specification**. Its author MUST:

1. Preserve DEC-03-01–04 and trace its detailed requirements back to this PRD.
2. Inspect the current repository and record its commit, build procedure, source interfaces, parser format, and actual baseline evidence. If execution is unavailable, record the limitation and keep baseline execution as an explicit prerequisite to implementation.
3. Resolve OPEN-03-01–05 through source inspection and technical judgment. Do not require deadline information or invent faculty requirements.
4. Freeze exact inputs, output fields, error semantics, size bounds, deterministic path policy, cost formulas, and numerical acceptance criteria.
5. Specify direct, indirect, unreachable, repeated-interaction, zero-interaction, invalid-input, and resource-boundary behavior without changing the product scope silently.
6. Record any source contradiction, its impact, and the smallest proposed correction. New scope or changed semantics require an explicit decision record.
7. Hand module interfaces and implementation choices to DOC-05; ordered coding tasks to DOC-06; executable test cases to DOC-07. Preserve the approved-specification gate before coding.

**Review state:** project direction is approved; this PRD is a reviewable draft. No implementation has been started or certified by this document. The approved workflow continues through DOC-04–07 before implementation and DOC-08 review.

## 11. Source pointers

- **DOC-01:** supplied `01-project-brief.md`, version 1.0; especially §2.3 (report), §3 (rubric), §4.4 (baseline), and §§8–11 (workflow, scope, understanding, success).
- **DOC-02:** supplied `02-research-feasiblity.md`; especially §§3.5–3.6 (XOR/baseline risks), §§6–9 (product/model/integration), §13 (external sources), and §§14–17 (MVP and handoff).
- **Operating instructions:** supplied `Pasted text.txt`, especially §§4, 6–8 (proposed direction, principles, sequence, document format).
- **Approval evidence:** student's three numbered responses in this conversation, recorded in §1.1.
- **Current implementation source:** [PQC simulator repository](https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-). Current source access and execution remain unverified in this session.

---

**End of DOC-03.**
