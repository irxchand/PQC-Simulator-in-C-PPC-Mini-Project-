import React from 'react'
import notesMarkdown from '../speaker-notes.md?raw'

const notes = {}
for (const match of notesMarkdown.matchAll(/^## Slide (\d+):[^\n]*\n\n([\s\S]*?)(?=\n## Slide|$)/gm)) notes[Number(match[1])] = match[2].trim()

const qiskit = 'https://qiskit.qotlabs.org/docs/guides/transpile'
const qsim = 'https://github.com/quantumlib/qsim'
const quest = 'https://quest.qtechtheory.org/'

function Gate({ name, role, formula }) { return <div className="gate-chip"><b>{name}</b><span>{role}</span><code>{formula}</code></div> }
function Source({ href, children }) { return <a className="slide-source" href={href} target="_blank" rel="noreferrer">{children}</a> }

function CircuitGraphMotif({ compact = false }) {
  return <div className={`motif ${compact ? 'compact' : ''}`} aria-label="Circuit to graph to BFS path to routing cost">
    <div className="mini-circuit"><span>q0</span><i/><b>H</b><i/><em/><i/><span className="target">⊕</span><i/></div><strong>›</strong>
    <svg viewBox="0 0 430 135" role="img" aria-label="Four qubit line topology"><path d="M45 68 H385" />{[0,1,2,3].map(n => <g key={n}><circle cx={45+n*113} cy="68" r="25"/><text x={45+n*113} y="77">{n}</text></g>)}</svg>
    <strong>›</strong><div className="motif-cost"><small>CIRCUIT › GRAPH + BFS</small><b>ROUTING COST</b><small>15 › 3 › NA</small></div>
  </div>
}

function Topology({ type, active = false }) {
  const edges = type === 'disconnected' ? [[0,1],[2,3]] : [[0,1],[1,2],[2,3]]
  return <svg className={`topology ${active ? 'active' : ''}`} viewBox="0 0 430 135" role="img" aria-label={`${type} hardware topology`}>
    {edges.map(([a,b]) => <line key={`${a}-${b}`} x1={45+a*113} y1="68" x2={45+b*113} y2="68" />)}
    {type === 'shortcut' && <path d="M45 57 C130 -5 300 -5 384 57"/>}
    {[0,1,2,3].map(n => <g key={n}><circle cx={45+n*113} cy="68" r="25"/><text x={45+n*113} y="77">{n}</text></g>)}
  </svg>
}

export const slides = [
  {
    eyebrow: 'DS LAB MINI PROJECT',
    title: 'PQC-GRAPH-C',
    className: 'cover-slide',
    body: <div className="cover-grid">
      <div className="cover-copy">
        <h2>A C-Native Quantum Circuit Workbench</h2>
        <p>From circuit simulation to hardware-aware reasoning.</p>
        <div className="cover-rule"/>
        <p style={{ color: '#25dfff', fontFamily: "'DM Mono', monospace", fontSize: '15px', letterSpacing: '0.08em', marginTop: '12px' }}>
          CIRCUIT → GRAPH + BFS → ROUTING COST
        </p>
      </div>
      <CircuitGraphMotif />
    </div>,
    notes: notes[1]
  },
  {
    eyebrow: 'THE FOUNDATION',
    title: 'THE SIMULATOR ESTABLISHED CIRCUIT-LEVEL BEHAVIOR',
    body: <div className="transition-layout">
      <div className="code-slab">
        <span>CIRCUIT FILE · ORDERED GATES</span>
        <code>4{`\n`}H 0{`\n`}RY 1 1.5708{`\n`}CX 0 3{`\n`}RZ 2 0.7854</code>
      </div>
      <div className="sim-chain">
        <div>CIRCUIT FILE</div><b>›</b>
        <div>PARSER</div><b>›</b>
        <div>STATEVECTOR</div><b>›</b>
        <div>PROBABILITIES</div>
      </div>
      <div className="transition-cut">
        <h3>The next engineering question</h3>
        <p>A mathematically valid circuit may request a two-qubit interaction that the selected hardware topology does not directly support.</p>
        <div className="logic-physical">
          <code>LOGICAL REQUEST: CX 0 3</code>
          <Topology type="line"/>
        </div>
      </div>
    </div>,
    notes: notes[2]
  },
  {
    eyebrow: 'QUANTUM FUNDAMENTALS',
    title: 'SIX GATES TRANSFORM ONE STATEVECTOR',
    body: <div className="fundamentals">
      <div className="state-box">
        <code>|ψ⟩ = α|0⟩ + β|1⟩</code>
        <p>P(0) = |α|²,  P(1) = |β|²</p>
        <strong>|α|² + |β|² = 1</strong>
      </div>
      <div className="gate-grid">
        <Gate name="X" role="Bit flip" formula="[0 1; 1 0]"/>
        <Gate name="H" role="Superposition" formula="1/√2 [1 1; 1 -1]"/>
        <Gate name="RX(θ)" role="X-axis rotation" formula="angle in radians"/>
        <Gate name="RY(θ)" role="Y-axis rotation" formula="also used for encoding"/>
        <Gate name="RZ(θ)" role="Relative-phase" formula="phase rotation"/>
        <Gate name="CX" role="Controlled flip" formula="involves two qubits"/>
      </div>
      <div className="cx-focus">
        <span>CONTROL</span><b>●</b><i/><b>⊕</b><span>TARGET</span>
        <p>Rotation angles are in radians. <b>CX</b> is the connection between quantum-circuit logic and my graph-analysis problem.</p>
      </div>
    </div>,
    notes: notes[3]
  },
  {
    eyebrow: 'THE PROBLEM AND EXISTING OPTIONS',
    title: 'LOGICAL INSTRUCTIONS MEET PHYSICAL CONNECTIVITY',
    body: <div className="position-layout">
      <div className="mismatch-panel">
        <code>Circuit requests:{`\n`}CX 0 3{`\n`}Selected hardware:{`\n`}0 — 1 — 2 — 3</code>
        <Topology type="line" active/>
        <div className="path-label">Direct edge 0–3? NO · Shortest path: 0 → 1 → 2 → 3</div>
        <div className="distinctions">
          <span><b>LOGICAL</b> requested gate</span>
          <span><b>PHYSICAL</b> available edges</span>
          <span><b>ROUTING</b> added movement</span>
        </div>
      </div>
      <div className="tool-compare">
        <div><b>Qiskit</b><span>Quantum SDK, hardware-aware transpilation and routing</span></div>
        <div><b>qsim</b><span>High-performance statevector simulation</span></div>
        <div><b>QuEST</b><span>High-performance simulation, with parallel and accelerated execution options</span></div>
        <div className="ours"><b>PQC-GRAPH-C</b><span>Inspectable C implementation linking circuit instructions, adjacency lists, BFS paths and explicit cost estimates</span></div>
        <p>My focus: transparent implementation and reproducible hardware-connectivity reasoning.</p>
        <div className="sources">
          <Source href={qiskit}>IBM Quantum</Source>
          <Source href={qsim}>qsim</Source>
          <Source href={quest}>QuEST</Source>
        </div>
      </div>
    </div>,
    notes: notes[4]
  },
  {
    eyebrow: 'COMPLETE SOLUTION ARCHITECTURE',
    title: 'ONE VALIDATED CIRCUIT. TWO INDEPENDENT ANALYSES.',
    body: <div className="architecture">
      <div className="input-pair">
        <div>CIRCUIT FILE</div>
        <div>TOPOLOGY FILE</div>
      </div>
      <div className="down-link">COMPLETE INPUT VALIDATION › VALIDATED INPUTS</div>
      <div className="validated">
        <span>VALIDATED CIRCUIT</span>
        <span>SEPARATE PHYSICAL GRAPH</span>
      </div>
      <div className="branches">
        <section>
          <h3>SIMULATION</h3>
          <p>Initialize statevector</p>
          <p>Apply gates</p>
          <p>Check norm</p>
          <p>Probabilities</p>
        </section>
        <section className="graph-branch">
          <h3>HARDWARE ANALYSIS</h3>
          <p>Extract CX operations</p>
          <p>Check adjacency</p>
          <p>Run BFS</p>
          <p>Classify CX</p>
          <p>Estimate cost</p>
        </section>
      </div>
      <div className="output-band">CLI / WORKBENCH / ACTUAL PROGRAM OUTPUT</div>
      <p className="separation">Critical invariant: a CX instruction never creates a hardware edge.</p>
    </div>,
    notes: notes[5]
  },
  {
    eyebrow: 'THE CORE ALGORITHM',
    title: 'GRAPH + BFS EXPOSES THE SHORTEST HARDWARE ROUTE',
    className: 'core-slide',
    body: <div className="bfs2">
      <div className="bfs2-graph">
        <div className="bfs2-req"><b>REQUEST</b> CX 0 3 <i>›</i> BFS from 0 to 3</div>
        <Topology type="line" active/>
        <code className="bfs2-adj">{`adj[0] = {1}       adj[1] = {0, 2}\nadj[2] = {1, 3}    adj[3] = {2}`}</code>
        <p><b>Vertex</b> physical qubit · <b>Edge</b> direct connection · <b>Distance</b> 3 edges</p>
      </div>
      <table className="bfs2-trace">
        <thead>
          <tr><th>step</th><th>pop</th><th>queue[]</th><th>visited[]</th><th>parent[]</th></tr>
        </thead>
        <tbody>
          <tr><td>0</td><td>–</td><td>0</td><td>0</td><td>parent[0] = -1</td></tr>
          <tr><td>1</td><td>0</td><td>1</td><td>0 1</td><td>parent[1] = 0</td></tr>
          <tr><td>2</td><td>1</td><td>2</td><td>0 1 2</td><td>parent[2] = 1</td></tr>
          <tr className="hit"><td>3</td><td>2</td><td>3</td><td>0 1 2 3</td><td>parent[3] = 2 · target reached</td></tr>
        </tbody>
      </table>
      <div className="bfs2-path">
        <span>reconstruct</span><strong>3 ← 2 ← 1 ← 0</strong>
        <span>shortest path</span><strong>0 → 1 → 2 → 3</strong>
        <em>distance = 3</em>
      </div>
      <div className="bfs2-why">
        <p><b>visited[]</b> prevents repeated visits · <b>queue[]</b> explores in breadth-first order · <b>parent[]</b> reconstructs the path. BFS guarantees the shortest path in unweighted graphs.</p>
        <b className="big">O(V + E)</b>
      </div>
    </div>,
    notes: notes[6]
  },
  {
    eyebrow: 'FROM BFS DISTANCE TO ROUTING COST',
    title: 'ONE CIRCUIT. THREE TOPOLOGIES. THREE OUTCOMES.',
    className: 'cost-slide',
    body: <div className="cost-layout">
      <div className="formula-panel">
        <h3>RESTORE-3CX-v1</h3>
        <code>SWAPs = 2(d - 1){`\n`}Extra CX = 6(d - 1){`\n`}Total CX = 1 + 6(d - 1)</code>
        <Topology type="line" active/>
        <p><b>Single gate CX(0,3) (d = 3):</b><br/>4 estimated SWAPs · 12 extra CX · 13 total CX equivalents</p>
      </div>
      <div className="topology-experiment">
        <div className="experiment-card">
          <b>LINE: 0–1–2–3</b>
          <Topology type="line"/>
          <strong>15</strong>
          <span>dist: 1, 3, 1</span>
          <small>4 SWAPs | 12 extra CX</small>
        </div>
        <div className="experiment-card shortcut">
          <b>SHORTCUT: ADD 0–3</b>
          <Topology type="shortcut"/>
          <strong>3</strong>
          <span>dist: 1, 1, 1</span>
          <small>0 SWAPs | 0 extra CX</small>
        </div>
        <div className="experiment-card disconnected">
          <b>DISCONNECTED: 0–1 2–3</b>
          <Topology type="disconnected"/>
          <strong>NA</strong>
          <span>dist: 1, NA, 1</span>
          <small>complete cost unavailable</small>
        </div>
        <div className="result-ribbon">
          <b>15</b><i>›</i><b>3</b><i>›</i><b>NA</b>
          <span>Connectivity changes estimated cost or makes an interaction unreachable</span>
        </div>
      </div>
      <p className="model-note">Modeled gate counts under RESTORE-3CX-v1, not hardware runtime or optimal-routing measurements. For the 3-CX circuit: 1 + 13 + 1 = 15.</p>
    </div>,
    notes: notes[7]
  },
  {
    eyebrow: 'APPLICATION DEMONSTRATION',
    title: 'THE WORKBENCH EXPOSES THE C ENGINE',
    body: <div className="demo-proof">
      <figure>
        <img src="/evidence/demo-lab.png" alt="Verified PQC-GRAPH-C Workbench running circuit and topology analysis"/>
        <figcaption>Actual local Workbench connected to the compiled C production executable</figcaption>
      </figure>
      <div className="demo-sequence">
        <span>01 <b>Load three-CX circuit</b></span>
        <span>02 <b>Select line topology</b></span>
        <span>03 <b>Run compiled C application</b></span>
        <span>04 <b>Inspect BFS path and estimated cost</b></span>
        <span>05 <b>Switch to shortcut topology and rerun</b></span>
        <span>06 <b>Try disconnected topology (NA)</b></span>
        <span>07 <b>Inspect graph.c & analysis.c in C source</b></span>
        <code>classification = INDIRECT{`\n`}path = 0 -&gt; 1 -&gt; 2 -&gt; 3{`\n`}distance = 3{`\n`}estimated_swaps = 4{`\n`}extra_cx_equivalents = 12{`\n`}total_cx_equivalents = 13</code>
      </div>
    </div>,
    notes: notes[8]
  },
  {
    eyebrow: 'CONCLUSION AND FUTURE DIRECTION',
    title: 'A FOUNDATION FOR HARDWARE-AWARE REASONING',
    className: 'conclusion-slide',
    body: <div className="conclusion">
      <div className="capabilities">
        <section>
          <span>IMPLEMENTED</span>
          <h3>C Gate Simulator</h3>
          <p>Validated quantum gate simulation and statevector probabilities.</p>
        </section>
        <section>
          <span>IMPLEMENTED</span>
          <h3>Graph + BFS Analyzer</h3>
          <p>Adjacency-list graph, deterministic BFS, direct/indirect/unreachable classification.</p>
        </section>
        <section>
          <span>IMPLEMENTED</span>
          <h3>Cost Model + Workbench</h3>
          <p>RESTORE-3CX-v1 estimation, reproducible topology comparison and interactive workbench.</p>
        </section>
      </div>
      <div className="future">
        <b>FUTURE ENGINEERING</b>
        <span>Actual circuit transformation with SWAP insertion and updated qubit mapping</span>
        <span>More advanced placement and routing strategies</span>
        <span>Reproducible performance benchmarks</span>
        <span>Investigation of optional GPU-accelerated classical simulation</span>
      </div>
      <blockquote>Don't just simulate the circuit. Understand its relationship with the machine it is meant to run on.</blockquote>
    </div>,
    notes: notes[9]
  },
  {
    eyebrow: 'REFERENCES',
    title: 'SOURCES AND REPRODUCIBLE PROJECT EVIDENCE',
    body: <div className="references">
      <a href={qiskit} target="_blank" rel="noreferrer">
        <b>IBM Quantum</b>
        <span>Transpilation stages, layout and routing</span>
        <code>qiskit.qotlabs.org/docs/guides/transpile</code>
      </a>
      <a href={qsim} target="_blank" rel="noreferrer">
        <b>Google Quantum AI</b>
        <span>qsim statevector simulator</span>
        <code>github.com/quantumlib/qsim</code>
      </a>
      <a href={quest} target="_blank" rel="noreferrer">
        <b>QuEST</b>
        <span>Quantum Exact Simulation Toolkit</span>
        <code>quest.qtechtheory.org</code>
      </a>
      <div>
        <b>Project evidence</b>
        <span>Approved project specifications (DOC-04 SRS, DOC-05 SDD, DOC-07)</span>
        <code>docs/</code>
      </div>
      <div>
        <b>Production source</b>
        <span>Production C codebase and test suite</span>
        <code>graph.c, analysis.c, sim.c, input.c, qml.c</code>
      </div>
      <div>
        <b>Execution records</b>
        <span>Actual topology-analysis execution records (15, 3, NA)</span>
        <code>build/evidence/ & docs/demo-evidence/</code>
      </div>
    </div>,
    notes: notes[10]
  }
]
