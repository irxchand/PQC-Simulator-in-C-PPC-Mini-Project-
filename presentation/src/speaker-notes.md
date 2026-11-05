## Slide 1: PQC-GRAPH-C (15 seconds)

My project, PQC-GRAPH-C, builds on a quantum-circuit simulator I had already implemented in C. I extended that foundation with graph-based hardware-connectivity analysis, using BFS to identify routes between qubits and estimate the resulting routing overhead.

## Slide 2: The foundation (25 seconds)

The original program accepts gate instructions through text files, parses them and simulates the circuit using a statevector. That gave me a way to inspect how gates transform a quantum state. But simulating a CX operation mathematically is a different question from checking whether a particular hardware topology directly supports that interaction. That is the gap I chose to investigate.

## Slide 3: Quantum fundamentals (35 seconds)

The simulator represents the state using complex amplitudes. The square of an amplitude's magnitude gives the probability of measuring the corresponding basis state. X flips the basic states, H creates superposition, and RX, RY and RZ rotate the state about different axes. CX flips its target when its control is one. It may create entanglement, depending on the input state. Unlike a single-qubit gate, CX involves a pair of qubits, making it the relevant operation for this project's connectivity analysis.

## Slide 4: Problem and existing alternatives (40 seconds)

Established systems already provide powerful quantum simulation and compilation capabilities. Qiskit includes hardware-aware layout and routing; qsim focuses on high-performance statevector simulation; and QuEST supports advanced simulation configurations. I am not claiming that my implementation replaces these systems or exceeds their performance. My goal is narrower: make the connection between a circuit instruction, its hardware graph, the BFS path and the estimated routing cost directly visible and understandable in C. That also makes it a meaningful Data Structures application.

## Slide 5: Architecture (25 seconds)

The architecture separates the circuit from the topology. After validating the inputs, the same circuit can be simulated mathematically or analyzed against a separate hardware graph. The simulation branch calculates the state; the analysis branch extracts the CX operations and uses BFS to evaluate connectivity. This separation lets me compare the same logical circuit against different hardware layouts without changing its instructions.

## Slide 6: The core algorithm (75 seconds)

This is the central Data Structures contribution. Each physical qubit is a vertex, each direct hardware connection is an edge, and the adjacency list stores the neighbors of every vertex.

When the analyzer sees CX from 0 to 3, it doesn't add an edge. It queries the graph. BFS starts at 0, discovers 1, then 2, then 3. The parent array remembers the vertex from which each new vertex was discovered. Following the parent links backward reconstructs the route.

BFS is important because the graph is unweighted and we need a shortest path. DFS could also find out whether the destination is reachable, but it might find a longer path first. The usual time complexity of BFS with adjacency lists is O of V plus E, because it processes the vertices and their edges. The resulting distance becomes an input to our routing-cost model.

## Slide 7: Routing cost and results (75 seconds)

The model assumes that routing movement is followed by restoration of the original mapping, and counts each SWAP as three CX equivalents. At distance three, that gives four SWAPs and twelve additional CX equivalents. Including the original logical gate, the long-range CX costs thirteen equivalents under this model.

But the full circuit has three CX gates, not one. The other two are direct in the line topology, so the aggregate is one plus thirteen plus one, giving fifteen. Add a shortcut between 0 and 3 and all three gates are direct, reducing the modeled total to three. In the disconnected topology, one CX is unreachable, so the complete cost is unavailable rather than zero. This is the key demonstration of why hardware connectivity matters.

## Slide 8: Application demonstration (75 seconds)

I'll now demonstrate the actual application. First, I load the same circuit and select the line graph. The compiled C engine reports that CX from 0 to 3 is indirect and gives us the route, distance and estimated cost. Next, I change the topology by adding the shortcut and run the same circuit again. The modeled total drops from fifteen to three CX equivalents. Finally, I demonstrate the disconnected graph, where the complete cost becomes unavailable. The important point is that the circuit stays unchanged: only the topology changes.

The source browser then allows me to connect the output to the actual graph and analysis functions in C.

## Slide 9: Conclusion and future direction (25 seconds)

The current project provides an inspectable foundation connecting circuit simulation, graph algorithms and routing-cost analysis. A natural next step would be to transform circuits by inserting SWAPs while correctly tracking the mapping, followed by measured comparisons of routing strategies. GPU acceleration is another potential research direction, not a capability I claim for this version.

## Slide 10: References (5 seconds)

These sources provide the context for existing simulators and transpilation systems. The project's own source code, specifications and reproducible execution records support the implementation claims.
