\---

doc\_id: "DOC-01"

title: "Project Brief and Vision"

project\_id: "PQC-GRAPH-C"

version: "1.0"

status: "PROPOSED"

decision\_status: "PENDING\_RESEARCH\_AND\_FEASIBILITY"

document\_type: "project\_brief"

primary\_language: "C"

presentation\_stack:

&#x20; - "Vite"

&#x20; - "React"

source\_repository: "https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-"

academic\_context: "CA 2 Mini-Project"

academic\_weight: 10

individual\_project: true

next\_document: "DOC-02 Research and Feasibility"

\---



\# DOC-01 — Project Brief and Vision



\## 0. Agent Instructions



\### 0.1 Purpose



This document establishes the project's context, goals, constraints, academic requirements, and intended direction.



It is the initial planning baseline, \*\*not the final technical specification\*\*.



\### 0.2 Instructions for Any AI Agent Reading This Document



1\. Read this document completely before proposing architecture, implementation, or code changes.

2\. Treat the academic rubric in Section 5 as a mandatory constraint.

3\. Treat existing repository functionality as existing work, not as a new feature.

4\. Do not describe proposed functionality as implemented.

5\. Do not claim novelty, correctness, speed improvements, or hardware acceleration without evidence.

6\. Do not commit to a final architecture until `DOC-02 Research and Feasibility` is complete.

7\. Prefer the smallest technically credible solution that meets the academic requirements.

8\. Ensure that every proposed feature is explainable by the student.

9\. Preserve existing working functionality unless a justified change is documented.

10\. When making recommendations, distinguish:

&#x20;   - `FACT`: verified information.

&#x20;   - `HYPOTHESIS`: plausible but unverified explanation.

&#x20;   - `PROPOSAL`: suggested design or feature.

&#x20;   - `UNKNOWN`: information requiring investigation.

&#x20;   - `DECISION`: an approved project choice.



\### 0.3 Requirement Language



The terms below have specific meanings throughout the project documents.



| Keyword | Meaning |

|---|---|

| MUST | Mandatory requirement |

| MUST NOT | Prohibited action or outcome |

| SHOULD | Recommended unless there is a documented reason not to |

| MAY | Optional feature |

| TBD | Not yet decided |

| BLOCKED | Cannot proceed until a dependency is resolved |



\---



\## 1. Executive Summary



\### 1.1 Project Concept



\*\*Working name:\*\* Graph-Driven Quantum Machine Learning in C



\*\*Project identifier:\*\* `PQC-GRAPH-C`



\*\*Concept:\*\* Extend an existing C-based quantum circuit simulator and parameterized quantum circuit (PQC) experiment into a small, graph-based quantum machine learning project.



The project should demonstrate a practical use of the \*\*Graph data structure\*\*, an algorithm operating on that structure, and a relevant AIML application.



The system may combine:



1\. A C-based quantum circuit simulator.

2\. A graph representation of qubit interactions.

3\. Graph algorithms to inspect or analyse circuit connectivity.

4\. A small PQC-based machine learning experiment.

5\. Measured experiments for model behaviour and performance.

6\. A Vite + React presentation that explains the completed project.



The exact feature set and novelty claim are \*\*TBD until research and feasibility analysis are complete\*\*.



\### 1.2 Primary Objective



Build the smallest useful, technically sound, and explainable project that:



\- Meets the CA 2 Mini-Project rubric.

\- Uses a Tree or Graph as a functional component.

\- Demonstrates an appropriate algorithm.

\- Applies the data structure to an AIML-relevant problem.

\- Extends the existing repository rather than needlessly rebuilding it.

\- Includes tested, reproducible results.

\- Can be explained by the student during the report presentation and viva.



\### 1.3 Core Constraints



| ID | Constraint | Priority |

|---|---|---|

| `CON-01` | The project must satisfy the supplied academic rubric. | Critical |

| `CON-02` | A Graph or Tree must be used functionally, not decoratively. | Critical |

| `CON-03` | The solution must remain understandable to the student. | Critical |

| `CON-04` | Development time and AI coding-agent usage must be controlled. | High |

| `CON-05` | Existing working C functionality should be reused. | High |

| `CON-06` | Performance and novelty claims must be supported by evidence. | High |

| `CON-07` | The presentation must ultimately use Vite + React. | Required deliverable |

| `CON-08` | Presentation development comes after the core implementation and testing. | Workflow constraint |



\---



\## 2. Academic Context and Source of Truth



\### 2.1 Assignment



The project is an individual \*\*CA 2 Mini-Project worth 10 marks\*\*.



The assignment requires a real-world application involving a Tree or Graph data structure. The selected data structure must have a meaningful role in solving or analysing the problem.



The project must also include a report and a presentation during lab hours, as specified in the assignment instructions.



\### 2.2 Source-of-Truth Hierarchy



Resolve conflicts using this order:



1\. \*\*`SRC-01`: Supplied CA 2 Mini-Project rubric and assignment requirements.\*\*

2\. \*\*`SRC-02`: Existing project repository and its actual implementation.\*\*

3\. \*\*`SRC-03`: Research findings and verified technical documentation.\*\*

4\. \*\*`SRC-04`: Approved requirements and architecture in subsequent project documents.\*\*

5\. \*\*`SRC-05`: AI-agent recommendations and unverified suggestions.\*\*



A lower-priority source MUST NOT override a higher-priority source without an explicit, documented reason.



\### 2.3 Mandatory Report Structure



The final academic report MUST contain all nine sections below.



| ID | Section | Required content |

|---|---|---|

| `REP-01` | Problem Statement | The precise problem being solved. |

| `REP-02` | Motivation | Why the problem matters. |

| `REP-03` | Objectives | Clear and verifiable objectives. |

| `REP-04` | Methodology / Implementation | System design and how the solution works. |

| `REP-05` | Hardware / Software Used | Actual hardware, programming language, tools, and dependencies. |

| `REP-06` | Algorithm / Operations Implemented | Graph/Tree algorithms and relevant computational operations. |

| `REP-07` | Executable Code | The runnable implementation or an appropriately referenced code listing. |

| `REP-08` | Result Analysis with Output Screenshots | Actual outputs, experimental observations, and analysis. |

| `REP-09` | References | Sources used in the research and implementation. |



These sections are mandatory even if later research changes the technical direction.



\---



\## 3. Evaluation Rubric



\*\*The following rubric is mandatory and takes precedence over optional technical ambitions.\*\*



| Criterion ID | Criterion | Marks | Excellent performance requires |

|---|---|---:|---|

| `RUB-01` | Relevance to Recent AIML Applications | 2 | Addresses a current, impactful AIML problem with real-world relevance. |

| `RUB-02` | Implementation \& Technical Complexity | 4 | Correct implementation using appropriate algorithms/models and sufficient technical depth. |

| `RUB-03` | Innovation, Real-world Applicability and Understanding | 4 | Demonstrates creativity, originality, practical usefulness, and clear understanding. |

| `RUB-TOTAL` | Total | \*\*10\*\* | Meets the overall assignment expectations. |



\### 3.1 Design Implications



\- The selected Graph/Tree algorithm must be visible in the implementation and explainable in the report.

\- The AIML component must be more than an unrelated label attached to a data-structure demonstration.

\- The report must explain how the components work together.

\- Novelty must be established through research, not assumed from implementation effort.

\- Experimental results must support claims about accuracy, efficiency, usefulness, or performance.



\### 3.2 Rubric Traceability



Subsequent documents MUST explicitly map requirements and tests to the rubric.



For example:



\- `RUB-02` → implementation requirements → algorithm implementation → test cases → screenshots.

\- `RUB-03` → identified research gap → design decision → demonstrated practical benefit → result analysis.



The exact mapping is to be completed in `DOC-02` and the subsequent requirements documents.



\---



\## 4. Existing Repository Baseline



\### 4.1 Repository



`SRC-02`



https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-



The repository is the starting point for this project.



\### 4.2 Existing Capabilities



The current codebase has been inspected previously. The following features were identified in the existing implementation; they MUST be reverified against the repository before implementation work begins.



| Component | Existing functionality |

|---|---|

| Complex numbers | Custom complex-number structure. |

| State representation | Statevector with `2^n` amplitudes for `n` qubits. |

| Quantum gates | X, H, RX, RY, RZ, and CX operations. |

| Circuit execution | Interactive execution and file-based circuit execution. |

| Circuit parsing | Reads gates from a circuit file. |

| PQC forward pass | Applies data encoding, parameterized rotations, and nearest-neighbour CX interactions. |

| QML experiment | Small XOR classification experiment with four trainable parameters. |

| Training | Finite-difference gradient estimation and gradient-descent-style updates. |

| Output | Computes an odd-parity probability for the PQC experiment. |



The following files were previously identified:



\- `a.c` — main C implementation.

\- `circuit.txt` — example circuit.

\- `README.md` — project documentation.



\### 4.3 Known Constraints of the Existing Implementation



The current design has the following reported limitations:



\- Statevector memory grows exponentially with the number of qubits.

\- The existing training experiment is small.

\- Finite-difference gradients require multiple forward evaluations.

\- GPU acceleration has not been established.

\- A general graph-analysis subsystem has not been established.

\- A Qiskit integration and rigorous benchmark suite have not been established.



These statements describe the previously inspected baseline, not a claim that no future branch or repository update has changed it.



\### 4.4 Baseline Preservation Rules



Before modifying code, an implementation agent MUST:



1\. Inspect the current repository and working tree.

2\. Identify the actual entry point and build procedure.

3\. Build and run the baseline program.

4\. Record the existing behaviour and any existing failures.

5\. Avoid silently deleting or replacing existing functionality.

6\. Establish a regression test or repeatable baseline run.



\---



\## 5. Proposed System Direction



\*\*Status:\*\* `PROPOSAL — NOT YET APPROVED`



\### 5.1 Candidate Architecture



The following is a conceptual direction for research, not a frozen architecture.



```text

Input Data / Circuit Configuration

&#x20;             |

&#x20;             v

&#x20;      Graph Representation

&#x20;             |

&#x20;             v

&#x20;   Graph Algorithm / Analysis

&#x20;             |

&#x20;             v

&#x20;     C-Based PQC Execution

&#x20;             |

&#x20;             v

&#x20;   Training / Prediction / Evaluation

&#x20;             |

&#x20;             v

&#x20;      Results and Measurements

&#x20;             |

&#x20;             v

&#x20;     Report and React Presentation

```



Each component must have a clear responsibility and an explicit interface.



\### 5.2 Candidate Graph Model



One potential model is a graph representing qubit interactions.



| Element | Candidate meaning |

|---|---|

| Vertex | Qubit |

| Edge | An interaction between two qubits, such as a CX operation |

| Storage | Adjacency list, subject to feasibility analysis |

| Candidate algorithms | BFS, DFS, degree calculation, and connected-component analysis |

| Possible use | Inspecting interaction topology or comparing alternative interaction structures |



\### 5.3 Graph Correctness Conditions



The implementation MUST NOT treat a graph as a complete representation of a quantum circuit unless the representation captures all required circuit information.



In particular:



\- An ordinary interaction graph does not, by itself, encode gate ordering.

\- It does not fully encode gate types, rotation parameters, measurement operations, or execution semantics.

\- Connectivity alone does not establish that a quantum state is entangled.

\- A graph algorithm is not meaningful merely because it exists in the code.



If the chosen graph is only a representation of connectivity, the implementation and report MUST describe it as an \*\*interaction graph\*\* or \*\*connectivity graph\*\*, rather than a complete circuit representation.



\### 5.4 Required Decision



`DOC-02` MUST determine whether this graph model provides a useful, defensible contribution to the selected application.



If the graph adds only superficial functionality, the project direction MUST be revised before implementation.



\---



\## 6. Novelty and Differentiation Policy



\### 6.1 Current Novelty Status



`NOV-STATUS: UNVERIFIED`



No final novelty claim has been approved.



A C-based simulator, a graph representation of qubit connectivity, a PQC classifier, or a GPU backend should not individually be presented as novel without supporting research.



\### 6.2 Candidate Contribution



A potential contribution is an integrated, transparent, reproducible experimental system that combines:



1\. An understandable C implementation.

2\. Explicit graph algorithms used in the experimental workflow.

3\. A clearly defined QML task.

4\. Measured results for alternative configurations.

5\. Reproducible tests and benchmarks.

6\. An analysis of when the graph representation is useful.



Research MUST establish whether this combination addresses a meaningful gap or is primarily an educational integration.



\### 6.3 Prohibited Unsupported Claims



Agents MUST NOT claim any of the following unless demonstrated and appropriately qualified:



\- “Novel quantum computing algorithm.”

\- “Faster than Qiskit.”

\- “CUDA accelerated.”

\- “Quantum advantage.”

\- “Production-ready quantum SDK.”

\- “More accurate than existing alternatives.”



A result on a small XOR dataset is not evidence of general superiority in quantum machine learning.



\### 6.4 Acceptable Evidence



Potential evidence includes:



\- Correctness tests against independently verified small examples.

\- Comparative measurements under specified conditions.

\- Ablation experiments that remove or change the graph component.

\- Task-specific model results.

\- Clear measurements of runtime, memory use, and repeatability.

\- Reproduction instructions and documented limitations.



Which measurements are necessary will be decided in `DOC-02`.



\---



\## 7. Performance and Hardware Policy



\### 7.1 Performance Objective



\*\*Objective:\*\* Investigate whether the proposed implementation offers measurable benefits for the selected workload.



The performance objective is a research question, not a guaranteed outcome.



\### 7.2 Benchmarking Requirements



Any performance comparison MUST specify:



\- The workload and input sizes.

\- The number of qubits and circuit configuration.

\- The number of repetitions and timing method.

\- The hardware and software environment.

\- Whether compilation, startup, and training time are included.

\- Whether the compared systems perform equivalent operations.

\- The measured results and relevant limitations.



Comparisons MUST NOT be presented as fair if the workloads or execution conditions are materially different.



\### 7.3 CUDA



`CUDA-STATUS: STRETCH\_GOAL\_NOT\_COMMITTED`



The student's known laptop contains an Intel Core i7-13620H and Intel integrated graphics, not a known NVIDIA CUDA-capable GPU.



Consequently:



\- CUDA MUST NOT be part of the critical path.

\- CUDA feasibility MUST be researched before allocating effort.

\- Remote NVIDIA GPU access MAY be evaluated if genuinely available.

\- A CUDA implementation MUST NOT be claimed without an actual implementation and successful tests.

\- CPU optimisation and benchmarking MAY be sufficient for the final project.



\### 7.4 Qiskit Comparison



Qiskit or Qiskit Aer may be considered as a reference implementation in the feasibility study. Their exact capabilities, benchmarking suitability, and current support must be verified using authoritative documentation.



The project MUST NOT assume that writing the simulator in C automatically makes it faster than established alternatives.



\---



\## 8. Development Workflow



\### 8.1 Agent Responsibilities



The development workflow separates planning from implementation to reduce rework and unnecessary coding-agent usage.



| Role | Responsibility |

|---|---|

| Student | Owns the project decisions, understands the implementation, validates outputs, and prepares for the viva. |

| GPT-6 planning assistant | Conducts research, develops specifications, identifies risks, reviews architectural choices, and prepares implementation instructions. |

| Codex implementation agent | Implements approved specifications, runs tests, reports code changes, and documents failures or deviations. |



These are workflow responsibilities, not claims of technical novelty.



\### 8.2 Governing Rule



\*\*Codex MUST implement approved specifications, not independently invent the product while coding.\*\*



Before implementation begins, the relevant requirements, interfaces, acceptance criteria, and testing expectations should be documented.



If Codex discovers a contradiction or technical blocker, it MUST report the issue rather than silently changing the intended design.



\### 8.3 Documentation Sequence



| Document ID | Document | Purpose |

|---|---|---|

| `DOC-01` | Project Brief and Vision | Establishes purpose, scope, constraints, rubric, and direction. |

| `DOC-02` | Research and Feasibility | Investigates existing solutions, possible research gaps, risks, and viability. |

| `DOC-03` | Product Requirements Document (PRD) | Defines target users, problems, functionality, exclusions, and success metrics. |

| `DOC-04` | Software Requirements Specification (SRS) | Defines verifiable functional and non-functional requirements. |

| `DOC-05` | Software Design Description (SDD) | Defines architecture, components, interfaces, algorithms, and data structures. |

| `DOC-06` | Implementation Plan | Breaks approved designs into small, ordered implementation tasks. |

| `DOC-07` | Test Plan and Test Cases | Defines correctness, regression, integration, and performance validation. |

| `DOC-08` | Implementation and Code Review | Tracks completed work, test evidence, known limitations, and review findings. |



Documents MUST reference each other using these stable IDs instead of relying on filenames alone.



The exact document contents may evolve, but later documents MUST NOT silently contradict approved earlier requirements.



\---



\## 9. Scope and Prioritisation



\### 9.1 Critical Scope



The final critical scope MUST include:



\- A functioning application that builds and runs.

\- A meaningful Tree/Graph data structure.

\- At least one appropriate algorithm that operates on that structure.

\- A relevant AIML application or experiment.

\- Demonstrable integration between the selected data structure and the application.

\- Correctness tests and observable outputs.

\- A report satisfying all mandatory sections.

\- A presentation built in Vite + React.

\- Sufficient simplicity for the student to explain the system.



\### 9.2 Candidate Features Requiring Feasibility Review



| Feature ID | Candidate feature | Status |

|---|---|---|

| `FEAT-01` | Graph-based qubit-interaction representation | Proposed |

| `FEAT-02` | BFS/DFS or another relevant graph algorithm | Proposed |

| `FEAT-03` | Graph-controlled or graph-informed PQC configuration | Proposed |

| `FEAT-04` | Small QML classification experiment | Existing baseline; verify |

| `FEAT-05` | Correctness and regression tests | Required |

| `FEAT-06` | Reproducible CPU performance benchmark | Proposed |

| `FEAT-07` | Qiskit/Aer comparison | Optional; feasibility-dependent |

| `FEAT-08` | CUDA backend | Stretch goal |

| `FEAT-09` | Vite + React presentation | Required deliverable; build after core code |



`DOC-02` MUST assess effort, usefulness, academic value, dependencies, and risks before these candidates become committed product requirements.



\### 9.3 Explicit Non-Goals Unless Reapproved



The initial project MUST NOT assume that it will deliver:



\- A general-purpose commercial quantum SDK.

\- Support for large-scale quantum systems.

\- A production-grade quantum compiler.

\- A full replacement for Qiskit.

\- Mandatory GPU/CUDA execution.

\- A large frontend application before the core C functionality is stable.



\---



\## 10. Explainability Requirements



\### 10.1 Student Understanding



The student MUST be able to explain, at an appropriate undergraduate level:



\- Why the chosen problem matters.

\- Why a Graph or Tree is appropriate.

\- How the selected data structure is represented in C.

\- What the graph algorithm does and why it is used.

\- How data enters and moves through the program.

\- How the existing simulator represents and evolves a quantum state.

\- How the chosen QML experiment works.

\- What the outputs mean.

\- How correctness is tested.

\- What the measurements demonstrate and what they do not demonstrate.



\### 10.2 Implementation Style



The implementation SHOULD favour:



\- Small functions with one clear responsibility.

\- Explicit data structures.

\- Descriptive identifiers.

\- Simple interfaces.

\- Assertions and tests where appropriate.

\- Comments explaining non-obvious reasoning.

\- Minimal external dependencies.

\- Reproducible input and output.

\- Clear separation between algorithm logic, simulation, and evaluation.



The project MUST NOT accumulate features that the student cannot explain.



\---



\## 11. Success Criteria



The following are project-level acceptance targets. Detailed and measurable test criteria will be established in `DOC-04` and `DOC-07`.



| ID | Criterion | Acceptance condition |

|---|---|---|

| `SC-01` | Academic compliance | All rubric requirements and report sections are addressed. |

| `SC-02` | Graph/Tree validity | The chosen data structure is implemented correctly and used in a meaningful way. |

| `SC-03` | Algorithm demonstration | At least one appropriate algorithm is implemented, tested, and explained. |

| `SC-04` | AIML relevance | The application has a defensible connection to an AIML problem. |

| `SC-05` | Integration | The data structure and algorithm contribute to the workflow rather than existing as isolated demonstrations. |

| `SC-06` | Correctness | Core behaviour is validated using appropriate tests and known examples. |

| `SC-07` | Reproducibility | A reviewer can follow the documented steps to reproduce key outputs. |

| `SC-08` | Evidence | All reported performance or model claims have supporting measurements or results. |

| `SC-09` | Explainability | The student can explain core concepts, algorithms, implementation, results, and limitations. |

| `SC-10` | Presentation | A Vite + React presentation accurately explains the completed implementation and its results. |



Passing these criteria establishes project completeness. It does not automatically establish scientific novelty or superior performance.



\---



\## 12. Risks and Mitigation



| Risk ID | Risk | Mitigation |

|---|---|---|

| `RISK-01` | Graph functionality is superficial. | Require an explicit link between the algorithm, the application, and a demonstrable result. |

| `RISK-02` | Existing tools already provide the proposed capabilities. | Investigate alternatives and define the actual contribution before implementation. |

| `RISK-03` | Scope expands into a full SDK. | Keep a minimal critical path and make extensions optional. |

| `RISK-04` | Performance claims are unsubstantiated. | Define fair, reproducible benchmarks before making comparisons. |

| `RISK-05` | CUDA becomes a time-consuming distraction. | Treat CUDA as optional and investigate hardware access first. |

| `RISK-06` | The student cannot explain complex implementation details. | Prefer simple designs and require explanations and tests for each core component. |

| `RISK-07` | Existing code breaks during extension. | Record and test baseline behaviour before modifying it. |

| `RISK-08` | Frontend work delays the core submission. | Develop the Vite + React presentation only after the core implementation is stable. |

| `RISK-09` | Novelty is claimed before literature review. | Leave novelty unresolved until `DOC-02` records evidence and limitations. |



\---



\## 13. Decisions and Open Questions



\### 13.1 Decisions Already Established



| Decision ID | Decision |

|---|---|

| `DEC-01` | The supplied academic rubric is the primary project constraint. |

| `DEC-02` | The existing C repository is the starting point. |

| `DEC-03` | The solution must be explainable by the student. |

| `DEC-04` | Planning and specification precede implementation. |

| `DEC-05` | The presentation will use Vite + React. |

| `DEC-06` | Core code and tests come before presentation development. |

| `DEC-07` | Novelty and performance must be supported by evidence. |



\### 13.2 Unresolved Questions



| Question ID | Question | Resolution document |

|---|---|---|

| `Q-01` | What concrete problem will the application solve? | `DOC-02` |

| `Q-02` | Does the proposed graph design provide a meaningful contribution? | `DOC-02` |

| `Q-03` | Which graph algorithm best fits the problem and rubric? | `DOC-02`, `DOC-03` |

| `Q-04` | What is the smallest credible implementation scope? | `DOC-02`, `DOC-03` |

| `Q-05` | Is Qiskit/Aer comparison necessary and feasible? | `DOC-02` |

| `Q-06` | Is there a realistic, worthwhile performance optimisation target? | `DOC-02` |

| `Q-07` | Is CUDA feasible within available hardware, time, and tooling? | `DOC-02` |

| `Q-08` | What measurable success thresholds should be adopted? | `DOC-03`, `DOC-04` |

| `Q-09` | Which interfaces and modules should the implementation use? | `DOC-05` |

| `Q-10` | Which presentation visualisations best communicate the actual results? | After implementation stabilises |



\---



\## 14. Handoff Specification: DOC-02



The next agent's task is to produce \*\*`DOC-02 — Research and Feasibility`\*\*.



\### 14.1 Required Investigation



The research agent MUST:



1\. Reinspect the current repository and verify the baseline in Section 4.

2\. Research established quantum simulators, quantum circuit tools, and relevant C/C++ implementations.

3\. Verify current Qiskit and Qiskit Aer capabilities using authoritative documentation.

4\. Investigate existing graph representations and algorithms for quantum-circuit connectivity.

5\. Determine which aspects of the proposed project already exist in established tools.

6\. Identify a specific, defensible project contribution—or conclude that the current idea has insufficient differentiation.

7\. Evaluate possible AIML applications and identify the one best suited to the rubric and limited implementation time.

8\. Evaluate candidate graph algorithms in terms of relevance, complexity, and demonstrable usefulness.

9\. Assess CPU optimisation, fair benchmarking, and the actual feasibility of CUDA.

10\. Estimate technical effort and identify the smallest practical implementation.

11\. Recommend one of the following outcomes:

&#x20;   - `PROCEED`: current direction is viable.

&#x20;   - `NARROW`: retain the idea but reduce scope.

&#x20;   - `PIVOT`: change the problem or central approach.

&#x20;   - `REJECT`: the direction is not defensible or practical.



\### 14.2 Evidence Standards



Research findings MUST distinguish facts, hypotheses, proposals, unknowns, and decisions.



For every important claim about competing tools, supported hardware, current features, or research novelty, record a source that can be checked.



Do not use repository features, graph algorithms, quantum simulation, or CUDA as novelty claims solely because they are being integrated into one project.



\### 14.3 Required DOC-02 Output



The next document MUST contain:



\- An executive recommendation.

\- A verified baseline assessment.

\- An existing-solutions comparison.

\- A research-gap analysis.

\- A comparison of candidate project directions.

\- A graph-algorithm suitability assessment.

\- A realistic performance and CUDA assessment.

\- A risk and effort assessment.

\- A minimal recommended scope.

\- A list of rejected or deferred features with reasons.

\- A go/no-go recommendation.

\- Inputs and decisions required for `DOC-03`.



\### 14.4 Stop Condition



\*\*Do not begin implementation merely because DOC-01 exists.\*\*



Implementation MUST wait until research has established a feasible project direction and the necessary requirements have been approved in the subsequent documents.



\---



\## End of DOC-01



\*\*Current state:\*\* Project context documented. Technical direction remains provisional.



\*\*Next required action:\*\* Produce `DOC-02 — Research and Feasibility`, using this document and the actual repository as the initial investigation baseline.

