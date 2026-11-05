# PQC-GRAPH-C — Live Demonstration Script & Faculty Evaluation Guide

## 1. Quick Overview & Timing (60–90 Seconds)
This live demonstration proves the core technical contribution of PQC-GRAPH-C:
**The logical quantum circuit remains completely unchanged while the hardware connectivity topology changes, altering the BFS shortest paths and resulting in deterministic routing overheads: 15 → 3 → NA.**

---

## 2. Environment Launch Commands

### Primary Launch (Interactive Workbench)
Open PowerShell in the project directory:
```powershell
cd "d:\Users\Documents\Symbiosis\Academics\Sem 3\High Credit\DS\Lab\Project\PQC-Simulator-in-C-PPC-Mini-Project-\presentation"
npm run bridge
```
In a second terminal window (or background process):
```powershell
npm run dev
```
Open in Google Chrome or Microsoft Edge:
```text
http://127.0.0.1:5177/#lab
```
*(Note: If port 5177 is busy, check the terminal output for the active Vite port, or access the slide presentation at `#slides`)*

---

## 3. Fixtures & File Locations

| Asset | File Path | Contents |
| :--- | :--- | :--- |
| **Circuit Under Test** | `demos/circuits/topology-comparison.circuit` | 4 qubits, 3 CX gates (`CX 0 1`, `CX 0 3`, `CX 2 3`) with RY/RZ rotations |
| **Topology 1 (Line)** | `demos/topologies/line4.graph` | 4 vertices, 3 edges: `0-1`, `1-2`, `2-3` |
| **Topology 2 (Shortcut)** | `demos/topologies/shortcut4.graph` | 4 vertices, 4 edges: adds edge `0-3` |
| **Topology 3 (Disconnected)**| `demos/topologies/disconnected4.graph` | 4 vertices, 2 components: `0-1`, `2-3` |
| **Compiled C Engine** | `qmlc.exe` | Compiled with GCC MinGW (C11, `-O2`) |
| **Graph Module** | `graph.c`, `graph.h` | Adjacency list representation & deterministic BFS shortest path |
| **Analysis Module** | `analysis.c`, `analysis.h`| `routing_cost_restore3cx` implementation & gate classification |

---

## 4. Live Demonstration Step-by-Step Sequence

### Step 1: Open the Workbench & Select "Same circuit: line"
1. In the left panel, find **Topology comparison** → click **Same circuit: line**.
2. Point out:
   - The circuit editor displays the exact circuit lines: `CX 0 1`, `CX 0 3`, `CX 2 3`.
   - The circuit diagram dynamically renders the wires ($q_0, q_1, q_2, q_3$) with gate boxes and labeled CX lines (`CX 0 1`, `CX 0 3`, `CX 2 3`).
   - The hardware graph shows the line graph `0 — 1 — 2 — 3`.
3. Click the bright **Run analyze** button.

### Step 2: Observe Line Graph Results (Cost = 15)
- Look at the **CX routing records** table:
  - `CX 0 1`: DIRECT (`0->1`), distance = 1, SWAPs = 0, total CX = 1
  - `CX 0 3`: **INDIRECT** (`0->1->2->3`), distance = **3**, estimated SWAPs = **4**, extra CX = **12**, total CX = **13**
  - `CX 2 3`: DIRECT (`2->3`), distance = 1, SWAPs = 0, total CX = 1
- Point out the **Summary Output**:
  ```text
  SUMMARY cx_total=3 cx_direct=2 cx_indirect=1 cx_unreachable=0 cost_status=COMPLETE estimated_swaps_total=4 extra_cx_equivalents_total=12 total_cx_equivalents=15
  ```
- **Speaking line:** *"Here, gate CX 0 3 requires routing across distance 3. Under RESTORE-3CX-v1, that requires 4 SWAPs (12 extra CX), bringing this single gate to 13 CX equivalents, and the full circuit to 15."*

### Step 3: Switch to "Same circuit: shortcut" (Cost = 3)
1. In the left panel, click **Same circuit: shortcut**.
2. Notice the circuit text has **not changed a single character**. Only the physical graph added edge `0 — 3`.
3. Click **Run analyze**.
4. Observe the result:
   - `CX 0 3`: now **DIRECT** (`0->3`), distance = **1**, SWAPs = 0, total CX = 1
   - Summary:
     ```text
     SUMMARY cx_total=3 cx_direct=3 cx_indirect=0 cx_unreachable=0 cost_status=COMPLETE estimated_swaps_total=0 extra_cx_equivalents_total=0 total_cx_equivalents=3
     ```
- **Speaking line:** *"With a single physical shortcut between qubits 0 and 3, all gates execute directly. Total cost drops from 15 to 3 CX equivalents."*

### Step 4: Switch to "Same circuit: disconnected" (Cost = NA)
1. In the left panel, click **Same circuit: disconnected**.
2. Click **Run analyze**.
3. Observe the result:
   - `CX 0 3`: **UNREACHABLE**, path = `NA`, distance = `NA`
   - Summary:
     ```text
     SUMMARY cx_total=3 cx_direct=2 cx_indirect=0 cx_unreachable=1 cost_status=INCOMPLETE_UNREACHABLE estimated_swaps_total=NA extra_cx_equivalents_total=NA total_cx_equivalents=NA
     ```
- **Speaking line:** *"Because qubits 0 and 3 are in separate graph components, BFS deterministically confirms no path exists. The compiler flags this as UNREACHABLE and refuses to compute a misleading total cost."*

### Step 5: Inspect the Production C Code
1. Click the **graph.c** button under **Production modules**:
   - Scroll to function `graph_shortest_path` (line ~207): show the `queue`, `visited`, and `parent` arrays implementing BFS.
2. Click the **analysis.c** button:
   - Show `routing_cost_restore3cx` (line ~45): highlight formula `SWAPs = 2 * (distance - 1)`.

---

## 5. Terminal-Based Fallback (If Browser/UI Fails)

If the local browser fails or network port conflicts occur during evaluation, open PowerShell directly in the repository root and run these exact commands:

```powershell
# 1. Test Line Topology
.\qmlc.exe --analyze demos/circuits/topology-comparison.circuit --topology demos/topologies/line4.graph

# 2. Test Shortcut Topology
.\qmlc.exe --analyze demos/circuits/topology-comparison.circuit --topology demos/topologies/shortcut4.graph

# 3. Test Disconnected Topology
.\qmlc.exe --analyze demos/circuits/topology-comparison.circuit --topology demos/topologies/disconnected4.graph
```

### Observed Actual Terminal Output
```text
PS D:\...\PQC-Simulator-in-C-PPC-Mini-Project-> .\qmlc.exe --analyze demos/circuits/topology-comparison.circuit --topology demos/topologies/line4.graph
CX cx_index=1 gate_index=5 source_line=6 control=0 target=1 classification=DIRECT adjacent=YES path=0->1 distance=1 estimated_swaps=0 extra_cx_equivalents=0 total_cx_equivalents=1
CX cx_index=2 gate_index=6 source_line=7 control=0 target=3 classification=INDIRECT adjacent=NO path=0->1->2->3 distance=3 estimated_swaps=4 extra_cx_equivalents=12 total_cx_equivalents=13
CX cx_index=3 gate_index=7 source_line=8 control=2 target=3 classification=DIRECT adjacent=YES path=2->3 distance=1 estimated_swaps=0 extra_cx_equivalents=0 total_cx_equivalents=1
SUMMARY cx_total=3 cx_direct=2 cx_indirect=1 cx_unreachable=0 cost_status=COMPLETE estimated_swaps_total=4 extra_cx_equivalents_total=12 total_cx_equivalents=15
```

---

## 6. Expected Viva Questions & Concise Defensible Answers

### Q1: Why separate the circuit from the topology?
**Answer:** In real quantum systems, a logical algorithm is written independently of the target machine. By separating the circuit specification from the hardware coupling graph, we can evaluate the exact same circuit against different physical chips without mutating the circuit definition.

### Q2: Why use adjacency lists instead of an adjacency matrix?
**Answer:** Quantum processor coupling graphs are planar and sparse (each qubit usually couples to only 2 to 4 neighbors, so $|E| = O(V)$). An adjacency list consumes $O(V + E)$ memory, whereas a matrix wastes $O(V^2)$. Furthermore, iterating over neighbors in BFS takes time proportional to degree rather than $V$.

### Q3: Why is BFS used instead of DFS?
**Answer:** In unweighted graphs, BFS explores vertices layer-by-layer in increasing order of distance, guaranteeing that the first time the target is popped, the shortest path is found. DFS only proves reachability and can easily wander down an arbitrarily long path first. Our routing cost directly depends on shortest-path distance $d$.

### Q4: Explain `visited[]`, `queue[]`, and `parent[]`.
**Answer:**
- `queue[]`: Maintains the FIFO frontier so vertices are processed in breadth-first order (by distance).
- `visited[]`: Boolean array ensuring no vertex is processed more than once, preventing infinite loops in cycles and bounding runtime to $O(V + E)$.
- `parent[]`: Stores the predecessor that discovered each vertex. Once target is reached, tracing `parent` back from target to source reconstructs the exact routing path.

### Q5: Why is the cost 13 for gate CX(0,3) but 15 for the circuit?
**Answer:** Gate CX(0,3) has distance $d = 3$. Under RESTORE-3CX-v1:
$$\text{SWAPs} = 2(d-1) = 4$$
$$\text{Extra CX} = 6(d-1) = 12$$
$$\text{Total for CX(0,3)} = 1 + 12 = 13$$
The circuit also contains two adjacent gates: CX(0,1) with cost 1, and CX(2,3) with cost 1.
Total circuit cost = $1 + 13 + 1 = 15$.

### Q6: What is the difference between routing cost estimation and circuit transformation?
**Answer:** Our project currently implements **routing cost estimation** (deterministic metric calculation). **Circuit transformation** (actual insertion of SWAP gates into the circuit file and remapping logical-to-physical qubit IDs) is identified in our architecture as the next engineering phase.

### Q7: What is your unique contribution over Qiskit or QuEST?
**Answer:** Tools like Qiskit are complex industrial frameworks with thousands of dependencies. My contribution is a lightweight, inspectable, educational C-native implementation where faculty and students can trace every pointer, adjacency list, and BFS queue operation directly in standard C11 without abstraction bloat.
