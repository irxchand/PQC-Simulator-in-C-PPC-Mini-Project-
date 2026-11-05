\---

doc\_id: "DOC-02"

title: "Research and Feasibility"

project\_id: "PQC-GRAPH-C"

version: "1.0"

status: "RESEARCH\_COMPLETE\_WITH\_RECOMMENDATION"

decision\_status: "RECOMMEND\_NARROW\_PENDING\_REQUIREMENTS\_APPROVAL"

document\_type: "research\_and\_feasibility"

date: "2026-10-20"

parent\_document: "DOC-01"

source\_repository: "https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-"

recommended\_project\_direction: "Graph-Based PQC Connectivity and Routing-Cost Analyzer"

primary\_language: "C"

presentation\_stack:

&#x20; - "Vite"

&#x20; - "React"

critical\_path:

&#x20; - "Verify baseline"

&#x20; - "Approve requirements"

&#x20; - "Implement graph algorithms"

&#x20; - "Integrate with circuit analysis"

&#x20; - "Test"

&#x20; - "Prepare report"

&#x20; - "Prepare presentation"

\---



\# DOC-02 — Research and Feasibility



\## 0. Agent Instructions



\### 0.1 Purpose



This document evaluates the technical feasibility, existing alternatives, potential research contribution, implementation effort, and risks of the project proposed in `DOC-01`.



Its purpose is to select a project direction \*\*before implementation begins\*\*.



\### 0.2 Instructions for AI Agents



Any agent acting on this document MUST:



1\. Read `DOC-01` first.

2\. Treat the academic rubric and explainability constraints in `DOC-01` as mandatory.

3\. Use the repository linked in this document as the baseline.

4\. Distinguish verified existing functionality from proposed changes.

5\. Use the cited sources to understand the capabilities already provided by established tools.

6\. Never claim novelty simply because multiple existing components are integrated.

7\. Never claim a performance improvement without measurements from a fair comparison.

8\. Never introduce CUDA, a large SDK architecture, or a complex frontend into the critical path without an approved justification.

9\. Preserve the existing simulator unless a specific technical defect justifies changing it.

10\. Convert the approved recommendation into requirements in `DOC-03`; do not start coding directly from this research document.



\### 0.3 Evidence Labels



| Label | Definition |

|---|---|

| `FACT` | Supported by inspected code or a cited source. |

| `INFERENCE` | A reasoned conclusion derived from facts. |

| `HYPOTHESIS` | A proposition requiring experimentation. |

| `PROPOSAL` | A candidate feature or design. |

| `UNKNOWN` | Information not established by this investigation. |

| `DECISION` | An approved project choice. |

| `RECOMMENDATION` | The recommended choice that should be used for subsequent planning unless rejected explicitly. |



\### 0.4 Research Limitations



This document provides a focused feasibility review based on the existing repository, official technical documentation, and selected research. It is \*\*not an exhaustive systematic literature review\*\* and does not establish scientific novelty across all published work.



Where novelty cannot be established from the reviewed sources, this document states that limitation rather than treating the absence of a discovered example as proof that none exists.



\---



\## 1. Executive Decision



\### 1.1 Recommendation



\*\*Recommended outcome: `NARROW`.\*\*



Retain the existing C-based quantum circuit simulator and PQC classifier, but make the core project contribution a \*\*Graph-Based Connectivity and Routing-Cost Analyzer for Parameterized Quantum Circuits\*\*.



Working project name:



`PQC-GRAPH-C`



Proposed project description:



> A small, explainable C application that represents quantum-device connectivity as a graph and uses graph algorithms to analyze the two-qubit interactions in a parameterized quantum circuit, identify reachable and unreachable interactions, find shortest connectivity paths, and estimate routing overhead under an explicitly defined simplified model.



The existing PQC classifier supplies the quantum machine learning context. The new Graph module supplies the data structure and algorithmic contribution.



This is recommended as an undergraduate DSA/AIML project, \*\*not as a novel quantum compiler or a globally novel graph algorithm\*\*.



\### 1.2 Why This Direction Is Preferred



\- It uses the existing C simulator instead of rebuilding it.

\- The Graph has a direct operational purpose: representing connectivity constraints.

\- BFS solves a concrete problem: finding the shortest path between qubits.

\- Reachability and path length produce deterministic, testable outputs.

\- Routing-cost estimates provide an interpretable connection to quantum-circuit execution.

\- The student can explain adjacency lists, BFS, paths, and the assumptions behind a SWAP estimate.

\- It avoids requiring a successful QML training result as the only proof that the project works.

\- It does not require local CUDA hardware or a claim that C is faster than established simulators.



\### 1.3 What This Project Will Not Claim



The project MUST NOT claim that:



\- Graph-based quantum-circuit analysis is new.

\- BFS is a new algorithm.

\- It replaces Qiskit transpilation.

\- The estimated routing cost equals real hardware runtime.

\- Its C simulator is faster than Qiskit Aer, QuEST, or Qulacs.

\- It has a CUDA backend unless one is actually implemented and tested.

\- It demonstrates quantum advantage.



\### 1.4 Decision Summary



| Item | Recommendation |

|---|---|

| Overall direction | `NARROW` |

| Main programming language | C |

| Main data structure | Graph, using an adjacency list |

| Main algorithm | Breadth-First Search (BFS) |

| Primary application | Connectivity analysis and simplified routing-cost estimation for PQC circuits |

| Existing simulator | Retain and extend |

| Existing XOR classifier | Retain as a demonstration and case study, subject to baseline validation |

| Global novelty claim | Not established |

| Academic differentiation | Transparent, integrated, graph-algorithm-driven analysis with reproducible examples |

| CPU performance comparison against Qiskit | Defer |

| CUDA backend | Exclude from critical path |

| React presentation | Build after implementation and tests stabilize |



\---



\## 2. Research Method and Evidence Register



\### 2.1 Research Questions



| ID | Research question | Finding |

|---|---|---|

| `RQ-01` | Does graph-based quantum-circuit connectivity analysis already exist? | Yes. It is an established capability in quantum-circuit tooling. |

| `RQ-02` | Does an established tool already route circuits against device connectivity? | Yes. Qiskit's transpiler includes layout and routing stages and may insert SWAP operations. |

| `RQ-03` | Are high-performance C/C++ quantum simulators already available? | Yes. QuEST and Qulacs are relevant examples; Qiskit Aer is another established simulator. |

| `RQ-04` | Does C automatically make the existing simulator faster than those alternatives? | No. This remains unproven and must not be assumed. |

| `RQ-05` | Is the proposed project implementable without CUDA? | Yes. Graph analysis and simplified path-cost estimation are CPU tasks. |

| `RQ-06` | Can the graph have a functional role without building a new quantum algorithm? | Yes. It can analyze connectivity and the estimated routing requirements of circuit interactions. |

| `RQ-07` | Is global novelty established? | No. The proposed educational integration is not evidence of research novelty. |

| `RQ-08` | Which project direction is most feasible under the rubric? | The connectivity and routing-cost analyzer is the strongest low-risk direction found in this review. |



\### 2.2 Evidence Categories



The findings use three main forms of evidence:



1\. \*\*Repository inspection:\*\* the existing README, C implementation, example circuit file, and root-level file listing.

2\. \*\*Official tool documentation:\*\* current documentation from IBM Quantum, Qiskit, NVIDIA, and the simulator projects.

3\. \*\*Published research:\*\* a selected peer-reviewed paper on interaction-graph-based characterization of quantum benchmarks.



Research sources were checked on 20 October 2026. Links are provided in Section 13.



\### 2.3 Verification Boundary



The repository files were inspected through GitHub. This was a static source review; it did not execute the program or independently validate numerical results.



Therefore:



\- Existing features described below are based on the inspected source.

\- Correctness, convergence, and performance remain to be tested locally.

\- The next implementation phase must begin with a successful baseline build and regression checks.



\---



\## 3. Existing Repository Assessment



\### 3.1 Repository



Source repository:



https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-



Files inspected:



\- \[`README.md`](https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-/blob/main/README.md)

\- \[`a.c`](https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-/blob/main/a.c)

\- \[`circuit.txt`](https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-/blob/main/circuit.txt)



The inspected repository's root directory contains those files along with `Presentation.pdf`. No dedicated Graph module or test suite appeared in the inspected root listing. This does not establish that no tests exist elsewhere; the current implementation shown in `a.c` does not include a dedicated graph-analysis subsystem.



\### 3.2 Existing Capabilities



| Component | Existing implementation | Status |

|---|---|---|

| Statevector | Complex amplitudes represented by a custom C structure | Present |

| Quantum gates | X, H, RX, RY, RZ, CX | Present in inspected source |

| Circuit simulation | Interactive and file-driven execution | Present |

| Circuit parser | Parses gate definitions from text | Present |

| Probability output | Computes basis-state probabilities | Present |

| PQC forward pass | Data encoding, parameterized rotations, CX interactions, parity probability | Present |

| Training example | Two-feature XOR experiment with four trainable parameters | Present |

| Training method | Finite-difference gradients and gradient-descent-style updates | Present |

| Graph representation | Dedicated Graph module | Not found in inspected source |

| BFS/DFS | Graph traversal module | Not found in inspected source |

| Hardware-connectivity analysis | Compares circuit interactions with a separate connectivity graph | Not found in inspected source |

| Routing-cost estimation | Graph-derived estimated overhead | Not found in inspected source |

| CUDA backend | GPU-specific execution path | Not found in inspected source |

| Automated tests | Dedicated test suite | Not found in inspected root listing |



\### 3.3 Statevector Memory Constraint



The implementation allocates \\(2^n\\) amplitudes, each containing two `double` values.



Assuming 8 bytes per `double`, the raw amplitude storage is:



\\\[

M(n)=16\\times2^n\\text{ bytes}

\\]



| Qubits | Approximate amplitude storage only |

|---:|---:|

| 20 | 16 MiB |

| 25 | 512 MiB |

| 28 | 4 GiB |

| 30 | 16 GiB |



These figures exclude the rest of the program, operating-system use, allocation overhead, and any additional buffers.



The code allows requests up to 30 qubits in `make\_st()`, but that should not be interpreted as practical support for every permitted size. The allocation or subsequent initialization may fail or exceed the machine's available memory.



\*\*Feasibility conclusion:\*\* the project should operate on small circuits, such as two to six qubits for initial tests. It does not need a large statevector to demonstrate Graph, BFS, and routing-cost concepts.



\### 3.4 Existing PQC Structure



The inspected forward function `fwd()` performs these steps:



1\. Initializes a statevector.

2\. Encodes the input features using RY rotations.

3\. Applies trainable RY rotations.

4\. Applies nearest-neighbour CX gates.

5\. Applies another trainable rotation layer.

6\. Returns an odd-parity measurement probability.



The nearest-neighbour entangling gates are generated internally through a loop. The current implementation does not use a Graph object to determine its interactions.



This means that adding an adjacency list alone would not integrate Graph with the existing PQC. The graph must be used to evaluate actual circuit interactions or influence a clearly defined execution decision.



\### 3.5 XOR Experiment: Important Validation Concern



The input encoding uses RY rotations with angles proportional to binary inputs. For binary inputs, RY encoding with angles 0 and \\(\\pi\\) maps the initial \\(|0\\rangle\\) state to computational-basis values corresponding to those bits, up to the exact convention and numerical behaviour of the implementation.



The parity readout of that encoded state, \*\*before the trainable layers and entangling gate are applied\*\*, can already equal the XOR of the binary inputs.



The current code does include additional rotations and a CX operation, so this observation does not prove that the actual trained model is trivial or incorrect. However, it creates a potential shortcut that must be investigated.



Before claiming that the experiment demonstrates learning XOR, `DOC-07` should require:



\- A baseline measurement of the encoding/readout behaviour without the learned transformation.

\- Validation of the output probabilities against expected values.

\- A comparison of the initial and trained model performance.

\- A clear explanation of what training improves, if anything.

\- A check of repeatability across runs.



\*\*Research implication:\*\* the XOR demo is suitable as a small QML case study, but it should not serve as the project's sole evidence of innovation or learning effectiveness.



\### 3.6 Baseline Quality Risks



Several implementation details deserve validation before new functionality is integrated.



| ID | Observation | Why it matters | Required action |

|---|---|---|---|

| `BASE-01` | The simulator permits large qubit counts even though memory grows exponentially. | A user may request an impractical allocation. | Validate sizes and handle allocation failures predictably. |

| `BASE-02` | State normalization is invoked after each gate. | It adds repeated work and may mask numerical or gate-implementation errors. | Test gate correctness and inspect norm drift before considering optimization. |

| `BASE-03` | The QML experiment uses finite-difference gradients. | Training performs many forward evaluations. | Preserve this simple implementation unless a demonstrated correctness or runtime issue demands a change. |

| `BASE-04` | The interaction topology in the PQC is hard-coded. | Graph analysis is not currently integrated. | Define the role of the Graph module explicitly. |

| `BASE-05` | No dedicated Graph tests were identified in the inspected root files. | A new data structure needs its own correctness checks. | Add deterministic unit tests for Graph and BFS operations. |

| `BASE-06` | The XOR experiment's initialization and readout may create a shortcut. | Claims about training need evidence. | Measure baseline and trained performance separately. |



The purpose is not to rewrite the existing simulator pre-emptively. These are validation tasks to prevent the extension from inheriting undocumented assumptions.



\---



\## 4. Existing Solutions and Competitive Landscape



\### 4.1 Qiskit and Quantum-Circuit Graphs



\*\*Evidence: verified.\*\*



IBM's Qiskit documentation defines `CouplingMap` as a graph for physical-qubit connectivity. Its documented functionality includes graph edges, connected components, connectivity checks, distances, and shortest-path operations. \[Source: Qiskit `CouplingMap`](https://qiskit.qotlabs.org/docs/api/qiskit/qiskit.transpiler.CouplingMap)



Qiskit also uses a graph-based directed acyclic graph representation of circuit operations and provides a modular transpilation pipeline. The documented pipeline includes mapping logical qubits to physical qubits, routing interactions to satisfy hardware connectivity, and inserting SWAP operations when necessary. \[Source: Qiskit transpilation guide](https://qiskit.qotlabs.org/docs/guides/transpile)



\*\*Implication:\*\* representing qubit connectivity as a Graph and using shortest-path algorithms for quantum circuits are not new ideas.



A project that only implements an adjacency list, BFS, and a shortest path between qubits would be a legitimate DSA exercise, but it would not be a defensible research novelty claim.



\### 4.2 Interaction-Graph Research



A 2023 research article, \*Interaction graph-based characterization of quantum benchmarks for improving quantum circuit mapping techniques\*, studies interaction-graph-based characterization for quantum-circuit mapping. \[Source: Springer, DOI 10.1007/s42484-023-00124-1](https://doi.org/10.1007/s42484-023-00124-1)



\*\*Implication:\*\* interaction graphs and their use in circuit mapping have already received research attention. The proposed project should not claim to introduce this general concept.



\### 4.3 High-Performance C/C++ Quantum Simulation



Relevant established projects include:



| Tool | Verified relevance | Implication for this project |

|---|---|---|

| \[QuEST](https://github.com/QuEST-Kit/QuEST) | Quantum statevector and density-matrix simulation; C/C++ interface; CPU, parallel, distributed, and GPU-oriented capabilities | A C/C++ quantum simulator and GPU acceleration are not new contributions by themselves. |

| \[Qulacs](https://github.com/qulacs/qulacs) | Fast parametric quantum-circuit simulation, C++ backend, Python interface, and GPU support | A performance-focused parametric simulator already exists in the ecosystem. |

| \[Qiskit Aer](https://github.com/Qiskit/qiskit-aer) | Established high-performance quantum-circuit simulation and noise models; GPU-enabled configurations are available | A valid performance claim needs a rigorous comparison against existing implementations. |



Qulacs specifically documents optimized CPU execution, parallelization options, parametric circuits, and GPU support. QuEST documents C/C++ access and several acceleration approaches. These capabilities materially weaken any claim that simply using C makes the proposed project novel or automatically faster. \[QuEST](https://github.com/QuEST-Kit/QuEST), \[Qulacs](https://github.com/qulacs/qulacs)



Qiskit Aer documents GPU-enabled simulation configurations and the requirements for supported GPU builds. \[Qiskit Aer](https://github.com/Qiskit/qiskit-aer)



\### 4.4 Established QML Classifiers



Qiskit Machine Learning provides quantum neural-network components and higher-level classifier/regressor interfaces, including a Variational Quantum Classifier. \[Source: Qiskit Machine Learning classifier tutorial](https://qiskit-community.github.io/qiskit-machine-learning/tutorials/02\_neural\_network\_classifier\_and\_regressor.html)



\*\*Implication:\*\* a small parameterized-circuit classifier is a useful AIML demonstration, but building one in C does not, on its own, establish a new QML method.



\### 4.5 Competitive Landscape Conclusion



The reviewed landscape establishes the following:



\- Graph-based circuit analysis already exists.

\- Circuit connectivity and routing are established tool capabilities.

\- QML classifiers are available in existing frameworks.

\- C/C++ quantum simulators already include performance optimizations.

\- GPU acceleration is available in established projects.



The proposed project must therefore focus on its \*\*specific educational implementation, explanation, measurable outputs, and integration with the existing simulator\*\* rather than claiming to introduce these underlying technologies.



\---



\## 5. Candidate Project Directions



Four candidate directions were evaluated against the project constraints.



Scores are qualitative planning assessments, not measured results. Each category uses a scale from 1 (weak) to 5 (strong).



| Candidate | Graph/algorithm relevance | QML relevance | Feasibility | Result predictability | Main concern |

|---|---:|---:|---:|---:|---|

| `OPT-01` Graph-based hardware-connectivity and routing-cost analyzer | 5 | 4 | 4 | 5 | Existing transpilers already provide more complete routing. |

| `OPT-02` Compare how interaction-graph topologies affect PQC learning | 4 | 5 | 3 | 2 | Training behaviour and meaningful comparisons may take substantial experimentation. |

| `OPT-03` Generic Graph/Tree visualizer alongside the existing XOR model | 4 | 2 | 5 | 5 | The Graph may feel disconnected from the AIML application. |

| `OPT-04` Optimize the C simulator and add a GPU/CUDA backend | 2 | 4 | 1 | 2 | High effort; GPU access is not established, and Graph use is secondary. |



\### 5.1 `OPT-01`: Hardware-Connectivity and Routing-Cost Analyzer



\*\*Concept:\*\* Analyze a PQC's two-qubit interactions against a configurable graph of connected physical qubits.



The tool can identify direct connections, find paths for nonadjacent interactions, detect unreachable pairs, and estimate SWAP overhead under an explicitly simplified routing model.



\*\*Advantages:\*\*



\- Uses Graph and BFS in the main workflow.

\- Produces deterministic outputs that are easy to test.

\- Has a clear connection to hardware-aware QML circuits.

\- Can be integrated with the current simulator.

\- Does not depend on a new classifier architecture or successful training experiment.

\- Is explainable in an undergraduate viva.



\*\*Limitations:\*\*



\- The general task is already handled by established transpilers.

\- A simplified path-cost analyzer is not a replacement for hardware-aware compilation.

\- Its contribution must be framed as an accessible, transparent educational tool, not a novel transpilation algorithm.



\*\*Feasibility verdict:\*\* best candidate for the available constraints.



\### 5.2 `OPT-02`: Graph Topologies and PQC Learning Behaviour



\*\*Concept:\*\* Use different interaction graphs to generate entangling layers and compare their effect on model behaviour, training results, gate counts, and runtime.



Potential topologies include a line, ring, star, and fully connected graph.



\*\*Advantages:\*\*



\- Graph structure influences circuit configuration directly.

\- Creates an experimental question about topology and model behaviour.

\- Could produce meaningful comparative results if designed carefully.



\*\*Limitations:\*\*



\- The existing classifier uses two features and two qubits. That configuration provides little meaningful variation in undirected connectivity topology.

\- A larger circuit or dataset would require additional implementation and validation.

\- Training results are not guaranteed to be better on a particular topology.

\- The existing XOR encoding/readout shortcut requires careful control experiments.

\- It is difficult to make a reliable novelty claim without more extensive research.



\*\*Feasibility verdict:\*\* potentially interesting, but less safe as the main deliverable. A small topology experiment may be added only if the core analyzer is complete and tested.



\### 5.3 `OPT-03`: Generic Graph/Tree Visualizer



\*\*Concept:\*\* Implement BFS, DFS, degree counting, and connected components separately from the quantum simulator.



\*\*Advantages:\*\* easy to implement and demonstrate.



\*\*Limitations:\*\* the Graph may be perceived as an unrelated DSA exercise; integration with the QML application would be weak.



\*\*Feasibility verdict:\*\* technically easy, but inferior against the academic requirement for a useful, integrated application.



\### 5.4 `OPT-04`: C Optimization and CUDA



\*\*Concept:\*\* Optimize the simulator, measure its performance, and potentially implement GPU execution.



\*\*Advantages:\*\* offers a performance-engineering learning opportunity.



\*\*Limitations:\*\*



\- C does not guarantee superiority over highly optimized alternatives.

\- The existing implementation has not been benchmarked fairly against comparable simulators.

\- GPU execution requires a real supported hardware/software path.

\- The known laptop configuration does not establish access to an NVIDIA CUDA-capable GPU.

\- This approach risks consuming the project budget while leaving the Graph requirement insufficiently addressed.



NVIDIA publishes GPU-specific CUDA compute-capability information. The correct hardware must be verified rather than inferred from the presence of a graphics processor. \[Source: NVIDIA CUDA GPU list](https://developer.nvidia.com/cuda/gpus)



\*\*Feasibility verdict:\*\* exclude from the critical path.



\---



\## 6. Recommended Product Direction



\### 6.1 Proposed Product



\*\*Working title:\*\* Graph-Based Hardware-Connectivity and Routing-Cost Analyzer for Parameterized Quantum Circuits



\*\*Product ID:\*\* `PQC-GRAPH-C`



\*\*Problem statement:\*\*



A logical quantum circuit may contain a two-qubit interaction between qubits that are not directly connected in a target connectivity graph. Before execution on constrained hardware, the circuit may require routing operations. The project provides a transparent, small-scale C implementation that analyzes those interactions using Graph algorithms and reports simplified connectivity and routing-cost information.



\### 6.2 Intended Users



| User | Need |

|---|---|

| Student learning DSA | A meaningful Graph application with a demonstrable algorithm. |

| Student learning quantum computing | A transparent explanation of how connectivity constrains two-qubit operations. |

| Student studying QML | A way to inspect interactions in a parameterized quantum circuit. |

| Academic evaluator | Runnable code, explainable algorithms, deterministic examples, and evidence of correct behaviour. |



These are intended users for the project concept, not verified external customer research.



\### 6.3 Core Input



The system accepts:



1\. A logical quantum circuit or a list of two-qubit interactions.

2\. A connectivity graph describing which physical qubits are neighbours.

3\. A defined mapping between logical and physical qubits, with identity mapping as the initial simplified case.

4\. An explicit routing-cost assumption.



\### 6.4 Core Output



For each requested two-qubit interaction, the system should report:



\- The interaction's control and target identifiers.

\- Whether the two physical qubits are directly adjacent in the supplied graph.

\- A shortest path when one exists.

\- Path length in edges.

\- Whether the interaction is unreachable under the supplied graph.

\- The estimated extra routing cost under the selected model.

\- A concise explanation of the calculation.



The program should also display a summary of the analyzed circuit, including the number of interactions, directly adjacent interactions, unreachable interactions, and estimated overhead.



\### 6.5 Important Scope Boundary



The recommended MVP is an \*\*analyzer and estimator, not a full transpiler\*\*.



It will not claim to produce a fully hardware-ready circuit, optimize all gate ordering, or account for real hardware noise and calibration.



\---



\## 7. Graph Design and Algorithm Feasibility



\### 7.1 Graph Definition



For the MVP, define the connectivity graph as:



\\\[

G=(V,E)

\\]



Where:



\- \\(V\\) is the set of physical qubits.

\- \\(E\\) is the set of allowed physical-neighbour connections under the project's simplified model.



Use an adjacency-list representation unless implementation testing identifies a specific reason to select another representation.



\### 7.2 Required Graph Operations



| ID | Operation | Purpose | Priority |

|---|---|---|---|

| `GRAPH-01` | Initialize graph | Create a graph for a given number of qubits. | Core |

| `GRAPH-02` | Add connection | Represent a physical-neighbour connection. | Core |

| `GRAPH-03` | Query adjacency | Determine if two qubits are directly connected. | Core |

| `GRAPH-04` | BFS shortest path | Find a shortest route between two qubits. | Core |

| `GRAPH-05` | Detect unreachable target | Determine whether a path exists. | Core |

| `GRAPH-06` | Report path length | Estimate routing effort from distance. | Core |

| `GRAPH-07` | Connected components | Explain isolated or disconnected parts of a topology. | Optional |

| `GRAPH-08` | Degree statistics | Summarize local connectivity. | Optional |



\### 7.3 Why BFS Is Appropriate



The MVP uses an unweighted graph: each connection contributes one edge to a path.



BFS explores vertices layer by layer. In an unweighted graph, it finds a path containing the minimum number of edges.



For adjacency-list storage, the expected time complexity is:



\\\[

O(|V|+|E|)

\\]



The complexity is easy to explain and verify with a hand-worked example.



\### 7.4 Demonstration Example



Consider four physical qubits connected in a line:



```text

Q0 --- Q1 --- Q2 --- Q3

```



Requested logical interaction:



```text

CX(Q0, Q3)

```



The physical endpoints are not adjacent. BFS discovers a shortest path:



```text

Q0 -> Q1 -> Q2 -> Q3

```



The graph distance is three edges.



Under a simplified routing strategy that moves one logical state along the path until the operands are adjacent, applies the requested CX, then restores the original mapping:



\- Distance: 3 edges.

\- Required movement: 2 SWAP operations.

\- Mapping restoration: 2 additional SWAP operations.

\- Total estimated SWAP operations: 4.



If every SWAP is decomposed into three CX-equivalent operations and the requested CX counts as one additional operation, the idealized estimate is:



\\\[

N\_{\\text{CX-equivalent}}=6(d-1)+1

\\]



For \\(d=3\\):



\\\[

N\_{\\text{CX-equivalent}}=6(2)+1=13

\\]



This calculation is valid \*\*only under the stated simplified assumptions\*\*. It is not a claim that a real transpiler must use 13 native gates or that the resulting hardware execution will have a particular latency.



Real routing can exploit alternative mappings, gate direction rules, native gate sets, cancellation, scheduling, hardware calibration, and optimization. Those are outside the MVP.



\### 7.5 Directionality Limitation



Qiskit represents its coupling information using directed edges for permitted CNOT direction, as documented by `CouplingMap`. \[Source: Qiskit `CouplingMap`](https://qiskit.qotlabs.org/docs/api/qiskit/qiskit.transpiler.CouplingMap)



To keep the implementation understandable, the recommended MVP may use an \*\*undirected connectivity graph\*\* for shortest-path analysis.



If this simplification is chosen:



\- The system MUST report physical adjacency, not certify that the desired directed native CNOT is executable.

\- The SWAP calculation MUST be described as an idealized estimate.

\- The report MUST state that directed gate constraints and real hardware properties are not modeled.



Support for directed couplings MAY be considered only after the undirected version is correct and all core requirements are met.



\---



\## 8. Routing-Cost Model and Its Limitations



\### 8.1 Required Assumptions



The cost estimator must explicitly state:



\- The graph is unweighted.

\- The graph is undirected for the MVP.

\- All edges have equivalent cost.

\- The initial mapping is known, with identity mapping as the default.

\- Each SWAP is approximated as three two-qubit CX-equivalent operations.

\- The logical interaction is applied after moving operands to adjacency.

\- The original mapping is restored for the displayed formula.

\- No hardware-specific error rates, durations, noise, or calibration values are included.



\### 8.2 What the Estimate Represents



The estimate is a simple gate-count proxy.



It does not equal:



\- Actual runtime.

\- Native-gate latency.

\- Error probability.

\- Circuit fidelity.

\- A globally optimal routing cost.

\- A guarantee that a circuit can be executed on a particular commercial device.



\### 8.3 Why the Limitation Is Acceptable



The project's academic purpose is to demonstrate how Graph representation and BFS inform analysis of quantum-circuit connectivity. A transparent estimate with correct assumptions is better than a sophisticated-looking result with unjustified accuracy.



The limitation should appear in the implementation, report, and presentation.



\---



\## 9. Integration with the Existing C Simulator



\### 9.1 Integration Plan



The current simulator accepts circuit gate operations and supports a PQC classifier. The recommended extension adds a graph-based analysis path.



Proposed conceptual workflow:



```text

Existing C Application

&#x20;       |

&#x20;       +-------------------------+

&#x20;       |                         |

&#x20;       v                         v

Circuit Execution          Connectivity Analyzer

&#x20;       |                         |

&#x20;       v                         v

State / Probabilities      Graph + BFS

&#x20;       |                         |

&#x20;       |                         v

&#x20;       |                  Shortest Path

&#x20;       |                         |

&#x20;       |                         v

&#x20;       |                  Cost Estimate

&#x20;       |                         |

&#x20;       +------------+------------+

&#x20;                    |

&#x20;                    v

&#x20;             Results / Report

```



The analyzer should be a separate, clearly identified module or subsystem. It should not be mixed into individual gate arithmetic functions.



\### 9.2 Recommended Implementation Boundaries



The exact file architecture belongs in `DOC-05`, but the following boundaries are recommended:



| Component | Responsibility |

|---|---|

| Graph module | Store connectivity and expose adjacency/path operations. |

| Circuit analysis | Identify the two-qubit interactions to analyze. |

| Routing-cost estimator | Convert a path length into an estimate under named assumptions. |

| CLI integration | Accept the chosen mode and present outputs. |

| Existing simulator | Preserve statevector execution and gate application. |

| Test suite | Verify graph correctness, cost calculations, and existing behaviour. |



\### 9.3 Integration Requirement



The project MUST demonstrate that the analyzer consumes real circuit interactions or a clearly defined PQC interaction specification.



A generic graph demo that happens to run in the same executable is insufficient.



A practical integration example is:



1\. Load a four-qubit circuit.

2\. Extract its CX interactions.

3\. Analyze the interactions against a line connectivity graph.

4\. Display BFS paths and estimated routing cost.

5\. Show the existing simulator's circuit output where relevant.

6\. Repeat with a different connectivity graph and explain the difference.



The existing two-qubit XOR classifier remains a separate QML demonstration; it should not be forced to demonstrate nonlocal routing when its current CX interaction is between adjacent qubits.



\---



\## 10. Benchmark and Performance Feasibility



\### 10.1 Is Performance Research Required?



A rigorous cross-framework speed comparison is \*\*not recommended for the MVP\*\*.



The project can satisfy its Graph/Tree requirement without proving that the entire application outperforms a commercial or research-grade simulator.



However, it must demonstrate that the Graph algorithm works and that its outputs are useful for the selected task.



\### 10.2 Recommended Measurements



Use small, repeatable tests for:



\- Directly adjacent interactions.

\- Nonadjacent interactions with an available path.

\- Unreachable interactions.

\- Graphs with different numbers of vertices and edges.

\- Path length and cost estimates.

\- Runtime of the BFS operation if practical.



For a student project, the clearest primary evidence is the correctness of known paths and the accuracy of the cost calculation under its assumptions.



A performance benchmark may be added if it is inexpensive, but timing must not replace correctness validation.



\### 10.3 Why a C-versus-Qiskit Speed Claim Is Not Feasible Yet



The current repository has not been benchmarked against Qiskit Aer, QuEST, or Qulacs under matched workloads.



Furthermore:



\- A simple C simulator and an optimized simulator may execute different workloads.

\- Circuit initialization, statevector size, gate count, data movement, and compilation can affect runtime.

\- Timing a tiny two-qubit example may mostly measure overhead.

\- The graph analyzer and statevector simulator perform different tasks and must not be conflated.



Therefore, the project may state that it is implemented in C and uses an explicit statevector representation, but MUST NOT claim a speed advantage without experimental evidence.



\---



\## 11. CUDA Feasibility



\### 11.1 Current Assessment



`CUDA-STATUS: NOT FEASIBLE FOR THE CRITICAL PATH`



The known development laptop has an Intel Core i7-13620H and integrated Intel graphics. No NVIDIA CUDA-capable GPU has been established for local development.



NVIDIA publishes supported GPU compute-capability information and hardware requirements. A compatible NVIDIA GPU and software environment must be verified before a CUDA implementation can be planned. \[Source: NVIDIA CUDA GPU list](https://developer.nvidia.com/cuda/gpus)



\### 11.2 Recommended Action



Do not implement a CUDA backend as part of the minimum viable project.



A future feasibility review may examine access to a remote supported NVIDIA GPU, but that introduces additional environment, setup, and testing dependencies.



\### 11.3 Decision



The project must remain complete and demonstrable using CPU execution alone.



GPU support MAY be reconsidered after the Graph analyzer, integration, testing, report evidence, and presentation are finished, if time and hardware permit.



\---



\## 12. Novelty and Academic Differentiation



\### 12.1 Novelty Status



`NOVELTY: NOT ESTABLISHED`



The research reviewed confirms that graph-based circuit representations, hardware-connectivity analysis, circuit routing, quantum simulation, GPU acceleration, and quantum classifiers already exist.



The proposed project should not claim to introduce any of those general ideas.



\### 12.2 Potential Contribution



The contribution may be framed as:



> A small, transparent C implementation that demonstrates how Graph representation and BFS can be used to analyze connectivity constraints and estimate routing overhead for parameterized quantum circuits, with reproducible examples and explicit assumptions.



The project's value is its explainability, implementation transparency, integration with the existing simulator, and use as an undergraduate Graph/Tree application.



That is a defensible \*\*educational contribution\*\*, but it is not equivalent to proving a new scientific technique.



\### 12.3 What Would Strengthen the Contribution?



The report and demonstration should show:



1\. The exact problem being analyzed.

2\. Why the Graph is the appropriate data structure.

3\. A working BFS implementation.

4\. A known example whose shortest path can be verified manually.

5\. An example of an unreachable interaction.

6\. A correct, clearly qualified SWAP/gate-count estimate.

7\. How different connectivity graphs change the result.

8\. The limits of the simplified model.

9\. How the analysis applies to a PQC circuit used in QML.



\### 12.4 What Would Not Strengthen the Contribution?



The following add effort without necessarily increasing academic value:



\- Renaming the simulator as an SDK.

\- Adding a React page before the core algorithm is correct.

\- Claiming C is inherently faster.

\- Listing many graph algorithms without using them.

\- Claiming CUDA without a working implementation.

\- Adding a complex dependency stack for a four-qubit demonstration.

\- Describing existing quantum-circuit concepts as newly invented.



\### 12.5 Novelty Decision



No globally novel algorithm is required to produce a good undergraduate mini-project. However, the report must distinguish the existing state of the art from the specific implementation and demonstration provided by this project.



\---



\## 13. Source Register



The following sources support the principal external findings. They should also be used by subsequent agents where relevant.



| Source ID | Source | Relevant finding |

|---|---|---|

| `SRC-02-01` | \[Qiskit `CouplingMap` documentation](https://qiskit.qotlabs.org/docs/api/qiskit/qiskit.transpiler.CouplingMap) | Graph-based coupling, connectivity checks, connected components, distances, and shortest paths. |

| `SRC-02-02` | \[Qiskit transpilation guide](https://qiskit.qotlabs.org/docs/guides/transpile) | Hardware-aware layout, routing, SWAP insertion, and circuit transformations. |

| `SRC-02-03` | \[Qiskit Machine Learning classifier/regressor tutorial](https://qiskit-community.github.io/qiskit-machine-learning/tutorials/02\_neural\_network\_classifier\_and\_regressor.html) | Established QML classifier and variational quantum classifier APIs. |

| `SRC-02-04` | \[QuEST repository](https://github.com/QuEST-Kit/QuEST) | C/C++ simulator with CPU, parallel, distributed, and GPU-oriented capabilities. |

| `SRC-02-05` | \[Qulacs repository](https://github.com/qulacs/qulacs) | Fast parametric quantum-circuit simulation, C++ implementation, and GPU support. |

| `SRC-02-06` | \[Qiskit Aer repository](https://github.com/Qiskit/qiskit-aer) | Established quantum-circuit simulator, noise models, and GPU-enabled configurations. |

| `SRC-02-07` | \[NVIDIA CUDA GPU compute capability list](https://developer.nvidia.com/cuda/gpus) | GPU hardware and compute-capability verification. |

| `SRC-02-08` | \[Medina Bandic, Almudever, and Feld (2023), \*Interaction graph-based characterization of quantum benchmarks for improving quantum circuit mapping techniques\*](https://doi.org/10.1007/s42484-023-00124-1) | Published work on interaction-graph-based circuit characterization and mapping. |

| `SRC-02-09` | \[Existing project README](https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-/blob/main/README.md) | Declared current capabilities and limitations. |

| `SRC-02-10` | \[Existing C implementation](https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-/blob/main/a.c) | Source inspection for statevector, gates, circuit execution, and PQC training. |

| `SRC-02-11` | \[Existing example circuit](https://github.com/irxchand/PQC-Simulator-in-C-PPC-Mini-Project-/blob/main/circuit.txt) | Current circuit input format. |



\*\*Research note:\*\* external references establish existing capabilities and relevant context. They do not prove that no similar educational implementation exists elsewhere.



\---



\## 14. Proposed MVP Scope



This section identifies the smallest credible direction for the next planning documents. It is a recommendation, not yet a complete implementation specification.



\### 14.1 Must Have



| ID | Feature | Acceptance evidence |

|---|---|---|

| `MVP-01` | Preserve existing C simulator | Baseline build and existing core modes remain functional. |

| `MVP-02` | Graph representation | Connectivity is stored and queried through a real Graph module. |

| `MVP-03` | BFS shortest path | Known routes return shortest paths. |

| `MVP-04` | Unreachable-pair detection | Disconnected vertices produce a clear result. |

| `MVP-05` | Circuit interaction analysis | The system analyzes actual CX interactions from a circuit or documented PQC input. |

| `MVP-06` | Simplified routing-cost estimate | The estimate follows a documented formula and explicit assumptions. |

| `MVP-07` | Regression and unit tests | Graph operations and existing simulator behaviour are checked. |

| `MVP-08` | Report evidence | The mandatory report contains actual outputs, analysis, and references. |

| `MVP-09` | Vite + React presentation | A presentation explains the implemented system after core functionality is stable. |



\### 14.2 Optional, Only After the Core Works



| ID | Feature | Condition |

|---|---|---|

| `OPT-MVP-01` | Connected-component analysis | Add only if it meaningfully improves the demonstration. |

| `OPT-MVP-02` | Several built-in connectivity graphs | Add once the basic analyzer is validated. |

| `OPT-MVP-03` | A topology comparison experiment | Add if simple, reproducible examples are available. |

| `OPT-MVP-04` | Performance measurement | Add only with an appropriate timing design. |

| `OPT-MVP-05` | Graph visualization in React | Add after the core C functionality and tests are stable. |



\### 14.3 Explicitly Excluded from MVP



\- CUDA backend.

\- Claims of C-versus-Qiskit speed superiority.

\- A general-purpose quantum SDK.

\- Full hardware-aware transpilation.

\- Hardware-specific fidelity or runtime prediction.

\- A new quantum learning algorithm.

\- A large-scale QML training framework.

\- A complex frontend before the C implementation is stable.



\---



\## 15. Risk Register



| ID | Risk | Probability | Impact | Mitigation |

|---|---|---|---|---|

| `R-01` | Project duplicates well-known tool capabilities without a clear educational contribution. | High | High | Position it honestly as a transparent C implementation and document its specific purpose. |

| `R-02` | Graph analysis becomes detached from QML. | Medium | High | Analyze actual interactions from a PQC or relevant circuit input. |

| `R-03` | Routing-cost calculations are misleading. | Medium | High | State all assumptions; label them as estimates, not real hardware costs. |

| `R-04` | Existing simulator behaviour regresses. | Medium | High | Record baseline behaviour and run regression tests. |

| `R-05` | XOR performance is overstated. | Medium | Medium | Validate the encoding-only control and compare initial versus trained results. |

| `R-06` | The student spends too much effort on CUDA. | High | High | Exclude CUDA from the critical path. |

| `R-07` | The frontend delays implementation. | Medium | Medium | Build the presentation after core functionality is tested. |

| `R-08` | The report claims unsupported novelty. | Medium | High | Use the source register and distinguish prior work from project-specific implementation. |

| `R-09` | Scope expands to full transpilation. | Medium | High | Keep the analyzer and estimator boundary explicit. |



\---



\## 16. Final Recommendation



\### 16.1 Decision



`RECOMMENDATION: NARROW`



Proceed with:



\*\*Graph-Based Hardware-Connectivity and Routing-Cost Analyzer for Parameterized Quantum Circuits in C.\*\*



Retain the existing simulator and classifier as the project foundation. Implement the Graph and BFS algorithm as a meaningful tool for analyzing two-qubit interactions, with a transparent estimate of routing overhead under stated simplifying assumptions.



\### 16.2 Why Not Proceed with the Original Broad Vision?



The original ambition combined simulator development, an SDK, graph-driven PQC configuration, performance optimization, Qiskit comparison, CUDA, and a React presentation.



That scope is too broad for the constraints and makes it difficult to identify the feature that earns the DSA marks.



The narrower direction has:



\- A concrete input and output.

\- A necessary Graph representation.

\- A named, understandable algorithm.

\- Deterministic correctness tests.

\- A specific connection to QML circuits.

\- A feasible CPU-only implementation.

\- A realistic report and presentation story.



\### 16.3 Claims Allowed After Successful Implementation



The project may state that it implements:



\- A Graph-based qubit-connectivity representation.

\- BFS shortest-path analysis.

\- Circuit-interaction connectivity analysis.

\- Simplified routing-cost estimation.

\- A C-based integration with an existing PQC simulator.

\- Tests and reproducible examples.



Claims of novelty, speed superiority, or real-hardware performance require additional evidence and must remain out of the project narrative unless properly established.



\---



\## 17. Handoff to DOC-03



The next document is `DOC-03 — Product Requirements Document (PRD)`.



It must convert the recommended direction into an explicit product scope.



\### 17.1 Required Tasks for the PRD Agent



1\. Adopt the recommendation from Section 16 unless the student explicitly changes it.

2\. Define the precise problem, intended users, and core use cases.

3\. Define the accepted circuit and connectivity input formats.

4\. Define exact output fields and behaviour for direct, indirect, and unreachable interactions.

5\. Define the Graph/BFS feature set and explicit exclusions.

6\. Define measurable success criteria and report evidence.

7\. Map requirements to the assignment rubric in `DOC-01`.

8\. Define functional requirements at product level without prematurely choosing every implementation detail.

9\. Keep the implementation CPU-only.

10\. Defer Vite + React development until the core C implementation and tests are stable.



\### 17.2 Dependencies



Before implementation planning:



\- The current repository must be built and run locally.

\- Existing behaviour must be recorded.

\- The selected Graph model and cost assumptions must be approved.

\- The output format and sample circuits must be specified.

\- The acceptance criteria must be testable.



\### 17.3 Stop Condition



\*\*Do not start implementation from DOC-02 alone.\*\*



The next step is `DOC-03`, followed by the SRS, SDD, implementation plan, and test plan described in `DOC-01`.



\---



\## End of DOC-02



\*\*Outcome:\*\* `NARROW`



\*\*Recommended project:\*\* Graph-Based Hardware-Connectivity and Routing-Cost Analyzer for Parameterized Quantum Circuits in C.



\*\*Core algorithm:\*\* BFS.



\*\*Core data structure:\*\* Graph represented with an adjacency list.



\*\*Implementation status:\*\* Not started by this document.



\*\*Next required document:\*\* `DOC-03 — Product Requirements Document`.

