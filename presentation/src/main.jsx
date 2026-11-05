import React, { useEffect, useMemo, useState } from 'react'
import { createRoot } from 'react-dom/client'
import './styles.css'
import './deck.css'
import { slides } from './slides.jsx'

const gateGuide = [
  ['X', 'Bit flip', '|0> becomes |1>'],
  ['H', 'Superposition', 'Splits one amplitude into two'],
  ['RX', 'X rotation', 'Mixes amplitudes with complex phase'],
  ['RY', 'Y rotation', 'Real amplitude rotation; used for encoding'],
  ['RZ', 'Z rotation', 'Changes relative phase'],
  ['CX', 'Controlled flip', 'Flips target only when control is 1'],
]

const legacySlides = [
  { eyebrow: 'PQC-GRAPH-C · DS Lab Mini Project', title: 'Quantum circuits on connected hardware', body: <div className="hero-grid"><div><p className="lead">A circuit simulator and graph analyzer that shows how hardware links change CX routing cost.</p><p className="byline">The C engine validates circuits, simulates gates, runs BFS and reports a transparent cost estimate.</p></div><div className="hero-mark"><span>same circuit</span><strong>3 / 15 / NA</strong><small>CX equivalents across three topologies</small></div></div>, notes: 'Introduce the project as a DS application. Logical circuits describe interactions, while hardware permits only graph edges. The software keeps the circuit fixed and changes the graph so the effect of connectivity is visible.' },
  { eyebrow: 'Problem and objective', title: 'Logical freedom meets physical limits', body: <div className="split"><div className="statement">A logical CX can name any two qubits. Hardware allows direct execution only across an edge.</div><ul><li>Store the circuit in gate order</li><li>Represent hardware with adjacency lists</li><li>Use BFS for shortest paths</li><li>Report direct, indirect or unreachable CX</li></ul></div>, notes: 'Explain that a valid circuit can still be difficult or impossible on a chosen topology. The project estimates routing work. It does not execute on real quantum hardware.' },
  { eyebrow: 'Six supported gates', title: 'Gate engine', body: <div className="gate-grid">{gateGuide.map(([gate, name, text]) => <div key={gate}><strong>{gate}</strong><span>{name}</span><small>{text}</small></div>)}</div>, notes: 'X swaps target amplitudes. H forms sums and differences divided by square root two. RX and RZ use complex arithmetic. RY mixes amplitude pairs and supports feature encoding. CX flips the target bit only when the control bit is one. Every gate uses the same production statevector engine.' },
  { eyebrow: 'Program structure', title: 'Separate data, algorithms and workflows', body: <div className="architecture"><span>input</span><b>validated circuit</b><i>simulation</i><i>analysis</i><b>hardware graph</b><span>CLI and Demo Lab</span></div>, notes: 'The input module validates the whole file before execution. Circuit stores ordered gates. Graph stores physical edges. Simulation never depends on the graph, and analysis never allocates a statevector. The browser calls the compiled program through a small localhost bridge.' },
  { eyebrow: 'Adjacency lists and BFS', title: 'Shortest hardware route', body: <div className="split"><div className="path-visual"><b>0</b><em /><b>1</b><em /><b>2</b><em /><b>3</b><p>path 0-1-2-3 · distance 3</p></div><ol><li>Enqueue the source</li><li>Mark vertices when discovered</li><li>Store the first parent</li><li>Reverse the parent chain</li></ol></div>, notes: 'BFS uses visited, queue and parent arrays. Sorted adjacency lists make the selected shortest path reproducible. Time complexity is O(V + E) per CX query, with O(V) auxiliary space.' },
  { eyebrow: 'RESTORE-3CX-v1', title: 'Distance to routing cost', body: <div className="formula-row"><div><span>SWAPs</span><b>2(d - 1)</b></div><div><span>extra CX</span><b>6(d - 1)</b></div><div><span>total CX</span><b>1 + 6(d - 1)</b></div><p>For distance 3: 4 SWAPs, 12 extra CX, 13 total for that gate.</p></div>, notes: 'The model moves qubits together and restores the identity mapping after each interaction. One SWAP counts as three CX equivalents. Clarify that this is a simplified estimate, not an optimal routing algorithm or device timing model.' },
  { eyebrow: 'Observed topology comparison', title: 'One circuit, three hardware graphs', body: <table><thead><tr><th>Topology</th><th>Distances</th><th>SWAPs</th><th>Extra CX</th><th>Total</th></tr></thead><tbody><tr><td>Line</td><td>1, 3, 1</td><td>4</td><td>12</td><td>15</td></tr><tr><td>Shortcut</td><td>1, 1, 1</td><td>0</td><td>0</td><td>3</td></tr><tr><td>Disconnected</td><td>1, NA, 1</td><td>NA</td><td>NA</td><td>NA</td></tr></tbody></table>, notes: 'These values came from the compiled analyzer using the same circuit bytes in all three runs. Only the topology changes. NA means at least one CX has no graph path, so a complete workload total would be misleading.' },
  { eyebrow: 'Interactive companion', title: 'Quantum Circuit Demo Lab', body: <div className="split"><ul><li>Tracked circuit and topology library</li><li>Editable inputs and real diagnostics</li><li>Simulation, analysis and QML modes</li><li>Actual C source browser</li></ul><div className="console-sample"><code>$ qmlc --analyze circuit --topology graph<br/>CX ... classification=INDIRECT<br/>path=0-&gt;1-&gt;2 distance=2<br/>estimated_swaps=2</code></div></div>, notes: 'The Demo Lab does not reproduce the algorithms in JavaScript. A localhost Node bridge writes bounded temporary inputs and starts the compiled executable without a shell. The interface shows stdout, stderr and exit status from that process.' },
  { eyebrow: 'PQC and XOR', title: 'Training with an essential control', body: <div className="split"><div className="circuit-strip"><b>RY(xπ)</b><b>RY(θ)</b><b>CX</b><b>RY(θ)</b><b>odd parity</b></div><div className="statement">Encoding and odd-parity readout already produce the four XOR labels. Training reuses those four examples, so it does not establish generalization.</div></div>, notes: 'The full circuit has four trainable angles and uses finite-difference gradient descent for 200 epochs. The encoding-only, initialized, trained and zero-parameter controls all call production QML functions. Present the experiment as a circuit and optimization demonstration.' },
  { eyebrow: 'Verification', title: 'Evidence from production paths', body: <div className="evidence"><div><strong>7</strong><span>module test groups</span></div><div><strong>20</strong><span>qubit numerical cases</span></div><div><strong>30</strong><span>qubit analysis-only case</span></div><div><strong>28</strong><span>tracked Demo Lab examples</span></div></div>, notes: 'The test suite covers allocation failures, parsing, graph paths, cost arithmetic, statevector numerics, QML controls and CLI behavior. Demo verification runs every valid example through the compiled application and checks every negative example fails as intended.' },
  { eyebrow: 'Limits and conclusion', title: 'A transparent teaching model', body: <div className="split"><div><h3>Models</h3><p>Ideal amplitudes, identity mapping, equal-cost undirected edges and restore-after-CX routing.</p></div><div><h3>Does not model</h3><p>Noise, fidelity, device time, GPU execution, optimal placement or cross-gate SWAP reuse.</p></div></div>, notes: 'Conclude with the defensible contribution: readable C data structures, deterministic BFS, a declared cost equation and a browser lab that executes the reference C engine. Avoid claims about real hardware performance.' }
]

function parseCircuit(text) {
  const lines = (text ?? '').split('\n').map(line => line.trim()).filter(line => line && !line.startsWith('#'))
  const qubits = Number(lines[0])
  if (!Number.isInteger(qubits) || qubits < 1 || qubits > 8) return null
  const gates = lines.slice(1).map((line, col) => {
    const part = line.split(/\s+/)
    const name = part[0]?.toUpperCase()
    if (name === 'CX') return { name, a: Number(part[1]), b: Number(part[2]), col }
    if (['X', 'H', 'RX', 'RY', 'RZ'].includes(name)) return { name, a: Number(part[1]), angle: part[2], col }
    return null
  }).filter(gate => gate && Number.isInteger(gate.a) && gate.a < qubits && (gate.name !== 'CX' || (Number.isInteger(gate.b) && gate.b < qubits)))
  return gates.length > 40 ? null : { qubits, gates }
}

const pqcCircuit = { qubits: 2, gates: [
  { name: 'RY', a: 0, angle: 'x0π', col: 0 }, { name: 'RY', a: 1, angle: 'x1π', col: 0 },
  { name: 'RY', a: 0, angle: 'θ0', col: 1 }, { name: 'RY', a: 1, angle: 'θ1', col: 1 },
  { name: 'CX', a: 1, b: 0, col: 2 },
  { name: 'RY', a: 0, angle: 'θ2', col: 3 }, { name: 'RY', a: 1, angle: 'θ3', col: 3 },
  { name: 'READ', a: 0, b: 1, col: 4 }
], stages: [[0, '1 ENCODE'], [1, '2 TRAINABLE'], [2, '3 ENTANGLE'], [3, '4 TRAINABLE'], [4, '5 READOUT']] }

const xorRows = [['00', 0, 0, 0], ['01', 0, 1, 1], ['10', 1, 0, 1], ['11', 1, 1, 0]]

function QmlLogic({ output }) {
  const predictions = Object.fromEntries((output?.stdout ?? '').split('\n').filter(line => line.startsWith('PREDICTION ')).map(line => { const f = Object.fromEntries(line.slice(11).split(' ').map(p => p.split('='))); return [f.input, f] }))
  return <section className="qml-logic">
    <div className="qml-steps"><h2>Circuit logic</h2><ol>
      <li><b>Encode</b> each input bit x as <code>RY(x·π)</code>: bit 0 → angle 0 (stays |0⟩), bit 1 → angle π (flips to |1⟩).</li>
      <li><b>Trainable layer</b> <code>RY(θ0)</code>, <code>RY(θ1)</code> rotates each qubit by a learned angle.</li>
      <li><b>Entangle</b> with <code>CX(control q1, target q0)</code> so q0 depends on q1.</li>
      <li><b>Trainable layer</b> <code>RY(θ2)</code>, <code>RY(θ3)</code> refines the result.</li>
      <li><b>Readout</b> P(odd parity) = probability that the two measured bits differ (|01⟩ or |10⟩). Class 1 if P &gt; 0.5. XOR is 1 exactly when the inputs differ.</li>
    </ol></div>
    <table className="qml-table"><thead><tr><th>input x0x1</th><th>encoding</th><th>XOR target</th><th>P(odd)</th><th>class</th><th>match</th></tr></thead><tbody>{xorRows.map(([name, x0, x1, target]) => { const p = predictions[name]; return <tr key={name}><td>{name}</td><td>RY({x0 ? 'π' : '0'}), RY({x1 ? 'π' : '0'})</td><td>{target}</td><td>{p ? Number(p.probability).toFixed(4) : '–'}</td><td>{p ? p.class : '–'}</td><td className={p ? (Number(p.class) === target ? 'ok' : 'bad') : ''}>{p ? (Number(p.class) === target ? '✓' : '✗') : 'run to fill'}</td></tr> })}</tbody></table>
  </section>
}

function getQmlCircuitText(qmlMode) {
  if (qmlMode === 'encoding-only') {
    return `# 2 Qubits: binary feature encoding only (input 0 1)\n2\nRY 0 0.000000\nRY 1 3.141593\n# Readout: odd parity on (q0, q1)`
  }
  if (qmlMode === 'zero') {
    return `# 2 Qubits: zero-parameter full PQC (all theta = 0)\n2\nRY 0 0.000000\nRY 1 3.141593\nRY 0 0.000000\nRY 1 0.000000\nCX 1 0\nRY 0 0.000000\nRY 1 0.000000\n# Readout: odd parity on (q0, q1)`
  }
  if (qmlMode === 'trained') {
    return `# 2 Qubits: trained PQC forward circuit (input 0 1)\n2\nRY 0 0.000000\nRY 1 3.141593\nRY 0 1.570796\nRY 1 1.570796\nCX 1 0\nRY 0 1.570796\nRY 1 1.570796\n# Readout: odd parity on (q0, q1)`
  }
  return `# 2 Qubits: parameterized quantum circuit (PQC)\n2\nRY 0 0.000000\nRY 1 3.141593\nRY 0 0.200000\nRY 1 0.400000\nCX 1 0\nRY 0 0.600000\nRY 1 0.800000\n# Readout: odd parity on (q0, q1)`
}

function CircuitDiagram({ circuit, routes = [], caption }) {
  if (!circuit) return <div className="visual-empty">Circuit diagram appears when the circuit text is valid.</div>
  const colW = 86, rowH = 66, left = 58
  const columns = Math.max(...circuit.gates.map(gate => gate.col), 0) + 1
  const top = circuit.stages ? 28 : 0
  const width = left + columns * colW + 24, height = circuit.qubits * rowH + 18 + top
  const wire = q => top + 36 + q * rowH, cx = col => left + col * colW + colW / 2
  let cxIndex = -1
  return <figure className="visual-card"><figcaption>{caption}</figcaption><svg className="circuit-svg" viewBox={`0 0 ${width} ${height}`} role="img" aria-label="Rendered quantum circuit">
    {Array.from({ length: circuit.qubits }, (_, q) => <g key={q}><text className="qlabel" x="8" y={wire(q) + 5}>q{q}</text><line className="wire" x1={left - 8} x2={width - 10} y1={wire(q)} y2={wire(q)} /></g>)}
    {circuit.stages?.map(([col, label]) => <text key={label} className="stage" x={cx(col)} y="16">{label}</text>)}
    {circuit.gates.map((gate, index) => {
      const x = cx(gate.col)
      if (gate.name === 'READ') return <g key={index} className="read"><rect x={x - 30} y={wire(gate.a) - 22} width="60" height={wire(gate.b) - wire(gate.a) + 44} rx="3" /><text x={x} y={(wire(gate.a) + wire(gate.b)) / 2 - 2}>P(odd)</text><text className="angle" x={x} y={(wire(gate.a) + wire(gate.b)) / 2 + 13}>parity</text></g>
      if (gate.name === 'CX') {
        cxIndex += 1
        const cls = routes[cxIndex]?.classification
        const tone = cls === 'INDIRECT' ? 'indirect' : cls === 'UNREACHABLE' ? 'unreachable' : cls === 'DIRECT' ? 'direct' : ''
        const minWire = Math.min(wire(gate.a), wire(gate.b))
        const maxWire = Math.max(wire(gate.a), wire(gate.b))
        return <g key={index} className={`cx ${tone}`}>
          <line x1={x} x2={x} y1={wire(gate.a)} y2={wire(gate.b)} />
          <circle className="ctrl" cx={x} cy={wire(gate.a)} r="7" />
          <circle className="tgt" cx={x} cy={wire(gate.b)} r="15" />
          <path d={`M${x - 15} ${wire(gate.b)} H${x + 15} M${x} ${wire(gate.b) - 15} V${wire(gate.b) + 15}`} />
          <text className="cxlabel" x={x} y={minWire - 9}>CX {gate.a} {gate.b}</text>
          {cls && <text className="cxtag" x={x + 20} y={(minWire + maxWire) / 2 + 4}>{cls === 'INDIRECT' ? 'needs route' : cls === 'UNREACHABLE' ? 'no path' : 'direct'}</text>}
        </g>
      }
      return <g key={index} className="gate"><rect x={x - 22} y={wire(gate.a) - 17} width="44" height="34" rx="3" /><text x={x} y={wire(gate.a) + (gate.angle ? -2 : 5)}>{gate.name}</text>{gate.angle && <text className="angle" x={x} y={wire(gate.a) + 11}>{String(gate.angle).slice(0, 7)}</text>}</g>
    })}
  </svg></figure>
}

function TopologyDiagram({ text, routes = [] }) {
  const lines = (text ?? '').split('\n').map(line => line.trim()).filter(line => line && !line.startsWith('#'))
  const n = Number(lines[0])
  if (!Number.isInteger(n) || n < 1 || n > 12) return <div className="visual-empty">Hardware graph appears when the topology text is valid.</div>
  const edges = lines.slice(1).map(line => line.split(/\s+/).map(Number)).filter(([a, b]) => Number.isInteger(a) && Number.isInteger(b) && a < n && b < n)
  const pos = i => ({ x: 160 + 82 * Math.cos(-Math.PI / 2 - (2 * Math.PI * i) / n), y: 105 + 72 * Math.sin(-Math.PI / 2 - (2 * Math.PI * i) / n) })
  const onPath = new Set()
  routes.filter(route => route.classification === 'INDIRECT' && route.path).forEach(route => { const v = route.path.split('->').map(Number); for (let i = 0; i + 1 < v.length; i++) onPath.add([v[i], v[i + 1]].sort().join('-')) })
  return <figure className="visual-card"><figcaption>Hardware graph {onPath.size > 0 ? '(BFS route highlighted)' : ''}</figcaption><svg className="graph-svg" viewBox="0 0 320 210" role="img" aria-label="Rendered hardware topology">
    {edges.map(([a, b], i) => { const p = pos(a), q = pos(b); return <line key={i} className={onPath.has([a, b].sort().join('-')) ? 'edge hot' : 'edge'} x1={p.x} y1={p.y} x2={q.x} y2={q.y} /> })}
    {Array.from({ length: n }, (_, i) => { const p = pos(i); return <g key={i} className="node"><circle cx={p.x} cy={p.y} r="17" /><text x={p.x} y={p.y + 5}>{i}</text></g> })}
  </svg></figure>
}

function parseAnalysis(output) {
  return output.split('\n').filter(line => line.startsWith('CX ')).map(line => Object.fromEntries(line.slice(3).split(' ').filter(part => part.includes('=')).map(part => {
    const index = part.indexOf('=')
    return [part.slice(0, index), part.slice(index + 1)]
  })))
}

function DemoLab() {
  const [library, setLibrary] = useState({ demos: [], topologies: [] })
  const [selectedId, setSelectedId] = useState('x-bit-flip')
  const [category, setCategory] = useState('All')
  const [circuitText, setCircuitText] = useState('')
  const [topologyId, setTopologyId] = useState('line4')
  const [topologyText, setTopologyText] = useState('')
  const [output, setOutput] = useState(null)
  const [status, setStatus] = useState('connecting')
  const [source, setSource] = useState(null)

  useEffect(() => {
    fetch('/api/demos').then(response => {
      if (!response.ok) throw new Error('Engine bridge is unavailable')
      return response.json()
    }).then(data => { setLibrary(data); setStatus('ready') })
      .catch(error => { setStatus('offline'); setOutput({ stderr: error.message, exitCode: null }) })
  }, [])

  const demo = library.demos.find(item => item.id === selectedId) ?? library.demos[0]
  const categories = useMemo(() => ['All', ...new Set(library.demos.map(item => item.category))], [library])
  const filtered = category === 'All' ? library.demos : library.demos.filter(item => item.category === category)

  useEffect(() => {
    if (!demo) return
    const initialText = demo.circuitText || (demo.mode === 'qml' ? getQmlCircuitText(demo.qmlMode) : '')
    setCircuitText(initialText)
    const nextTopology = demo.topology ?? topologyId
    setTopologyId(nextTopology)
    setTopologyText(library.topologies.find(item => item.id === nextTopology)?.text ?? '')
    setOutput(null)
    setSource(null)
  }, [demo?.id, library.topologies])

  function selectTopology(id) {
    setTopologyId(id)
    setTopologyText(library.topologies.find(item => item.id === id)?.text ?? '')
  }

  function resetDemo() {
    if (!demo) return
    const initialText = demo.circuitText || (demo.mode === 'qml' ? getQmlCircuitText(demo.qmlMode) : '')
    setCircuitText(initialText)
    selectTopology(demo.topology ?? topologyId)
    setOutput(null)
  }

  async function runDemo() {
    if (!demo || status === 'running') return
    setStatus('running'); setOutput(null)
    try {
      const response = await fetch('/api/run', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ operation: demo.mode, qmlMode: demo.qmlMode, circuitText, topologyText }) })
      const result = await response.json()
      if (!response.ok) throw new Error(result.error || 'Bridge request failed')
      setOutput(result)
      setStatus(result.launchError ? 'offline' : result.exitCode === 0 ? (result.stderr ? 'warning' : 'success') : 'error')
    } catch (error) {
      setOutput({ stderr: error.message, exitCode: null }); setStatus('offline')
    }
  }

  async function openSource(path) {
    const response = await fetch(`/api/source?path=${encodeURIComponent(path)}`)
    setSource(await response.json())
  }

  const routes = output?.stdout ? parseAnalysis(output.stdout) : []
  return <div className="lab-shell">
    <aside className="library-panel"><div className="brand"><span>PQC-GRAPH-C</span><strong>Workbench</strong></div><label>Category<select value={category} onChange={event => setCategory(event.target.value)}>{categories.map(item => <option key={item}>{item}</option>)}</select></label><div className="demo-list">{filtered.map(item => <button className={item.id === demo?.id ? 'active' : ''} key={item.id} onClick={() => setSelectedId(item.id)}><b>{item.title}</b><span>{item.difficulty} · {item.mode}</span></button>)}</div></aside>
    <main className="workspace">{!demo && <section className="offline-panel"><span className="status offline">offline</span><h1>Engine bridge unavailable</h1><p>{output?.stderr || 'The example library could not be loaded.'}</p><code>cd presentation<br/>npm run bridge</code></section>}{demo && <>
      <header className="lab-header"><div><span className={`status ${status}`}>{status}</span><h1>{demo.title}</h1><p>{demo.description}</p></div><div className="run-actions"><button className="secondary" onClick={resetDemo}>Reset</button><button className="run" onClick={runDemo} disabled={status === 'running'}>{status === 'running' ? 'Running…' : `Run ${demo.mode}`}</button></div></header>
      <section className={`editor-grid ${demo.mode === 'analyze' ? '' : 'single'}`}>
        <article className="editor-card">
          <div className="card-title"><b>Circuit Lines</b><code>{demo.circuit ?? `qml.c --qml ${demo.qmlMode}`}</code></div>
          <textarea value={circuitText} onChange={event => setCircuitText(event.target.value)} spellCheck="false" />
        </article>
        {demo.mode === 'analyze' && <article className="editor-card">
          <div className="card-title"><b>Topology</b><select value={topologyId} onChange={event => selectTopology(event.target.value)}>{library.topologies.map(item => <option value={item.id} key={item.id}>{item.title}</option>)}</select></div>
          <textarea value={topologyText} onChange={event => setTopologyText(event.target.value)} spellCheck="false" />
        </article>}
      </section>
      <section className="visuals"><CircuitDiagram circuit={demo.mode === 'qml' ? pqcCircuit : parseCircuit(circuitText)} routes={routes} caption={demo.mode === 'qml' ? 'Parameterized quantum circuit (XOR classifier): encode → trainable RY → CX → trainable RY' : 'Circuit (gate order, left to right)'} />{demo.mode === 'analyze' && <TopologyDiagram text={topologyText} routes={routes} />}</section>
      {demo.mode === 'qml' && <QmlLogic output={output} />}
      <section className="explanation"><div><b>Expected behavior</b><p>{demo.expected}</p></div><div><b>Production modules</b><div className="module-links">{demo.modules.map(module => <button key={module} onClick={() => openSource(module)}>{module}</button>)}</div></div></section>
      {routes.length > 0 && <section className="route-table"><h2>CX routing records</h2><table><thead><tr><th>Line</th><th>Control → target</th><th>Class</th><th>Path</th><th>Distance</th><th>SWAPs</th><th>Total CX</th></tr></thead><tbody>{routes.map((row, index) => <tr key={index}><td>{row.source_line}</td><td>{row.control} → {row.target}</td><td>{row.classification}</td><td>{row.path}</td><td>{row.distance}</td><td>{row.estimated_swaps}</td><td>{row.total_cx_equivalents}</td></tr>)}</tbody></table></section>}
      <section className={`console ${output ? (output.exitCode === 0 ? (output.stderr ? 'warning' : 'success') : 'error') : ''}`}><div><b>Compiled C output</b>{output && <span>exit {output.exitCode ?? 'NA'} · {output.durationMs ?? 0} ms</span>}</div><pre>{output ? `${output.stdout ?? ''}${output.stderr ? `\n[stderr]\n${output.stderr}` : ''}${output.launchError ? `\n${output.launchError}` : ''}` : 'Choose an example, edit its input if needed, then run the production C application.'}</pre></section>
      {source && <section className="source-panel"><div><b>{source.path}</b><a href={source.githubUrl} target="_blank" rel="noreferrer">Open on GitHub</a></div><pre>{source.text}</pre></section>}
    </>}</main>
    <aside className="learn-panel"><h2>Quick guide</h2>{gateGuide.map(([gate, name, text]) => <details key={gate}><summary><b>{gate}</b> {name}</summary><p>{text}</p></details>)}<details><summary><b>BFS</b> graph search</summary><p>A queue explores vertices by distance. Visited prevents repeats; parent reconstructs the path.</p></details><details><summary><b>Cost</b> RESTORE-3CX-v1</summary><p>At distance d: SWAPs = 2(d-1), extra CX = 6(d-1). It is a simplified estimate.</p></details><details><summary><b>Ideal vs hardware</b></summary><p>Simulation evolves amplitudes. Analysis checks CX connectivity. Neither predicts noise or fidelity.</p></details></aside>
  </div>
}

function SlideDeck() {
  const [index, setIndex] = useState(0)
  const [showNotes, setShowNotes] = useState(false)
  const move = delta => setIndex(value => Math.max(0, Math.min(slides.length - 1, value + delta)))
  useEffect(() => {
    const onKey = event => {
      if (['ArrowRight', 'ArrowDown', 'PageDown', ' '].includes(event.key)) move(1)
      if (['ArrowLeft', 'ArrowUp', 'PageUp'].includes(event.key)) move(-1)
      if (event.key === 'Home') setIndex(0)
      if (event.key === 'End') setIndex(slides.length - 1)
      if (event.key.toLowerCase() === 'n') setShowNotes(value => !value)
    }
    window.addEventListener('keydown', onKey)
    return () => window.removeEventListener('keydown', onKey)
  }, [])
  const slide = slides[index]
  return <main className="deck"><section className={`slide ${slide.className || ''}`}><header><span>{slide.eyebrow}</span><span>{String(index + 1).padStart(2, '0')} / {slides.length}</span></header><h1>{slide.title}</h1><div className="slide-body">{slide.body}</div><footer><span>PQC-GRAPH-C</span><div className="progress"><i style={{ width: `${((index + 1) / slides.length) * 100}%` }} /></div><nav><button onClick={() => setShowNotes(value => !value)} className={showNotes ? 'active' : ''}>Notes</button><button onClick={() => move(-1)} disabled={index === 0}>←</button><button onClick={() => move(1)} disabled={index === slides.length - 1}>→</button></nav></footer>{showNotes && <aside className="speaker-notes"><b>Presenter notes</b><p>{slide.notes}</p></aside>}</section></main>
}

function App() {
  const [view, setView] = useState(location.hash === '#slides' ? 'slides' : 'lab')
  useEffect(() => { const update = () => setView(location.hash === '#slides' ? 'slides' : 'lab'); window.addEventListener('hashchange', update); return () => window.removeEventListener('hashchange', update) }, [])
  return <><div className="view-switch"><a href="#lab" className={view === 'lab' ? 'active' : ''}>Workbench</a><a href="#slides" className={view === 'slides' ? 'active' : ''}>Presentation</a></div>{view === 'slides' ? <SlideDeck /> : <DemoLab />}</>
}

createRoot(document.getElementById('root')).render(<App />)
