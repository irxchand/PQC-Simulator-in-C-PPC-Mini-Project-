# PQC-GRAPH-C Quantum Circuit Demo Lab

A student-readable C application for ideal parameterized-quantum-circuit simulation and graph-based hardware-connectivity analysis. The companion Demo Lab lets students edit tracked examples, execute the compiled C engine, inspect routing records and open the real implementation source.

The estimator is not an optimal router and does not predict device runtime, noise or fidelity.

## Build

Linux/macOS with GCC or Clang:

```bash
make all
```

Windows with the supplied MinGW toolchain:

```powershell
C:\MinGW\bin\mingw32-make.exe all
```

This produces `qmlc` on POSIX or `qmlc.exe` on Windows.

## Run

Open the original four-choice menu:

```bash
./qmlc
```

Show CLI help:

```bash
./qmlc --help
```

Run a circuit file without the interactive menu:

```bash
./qmlc --simulate demos/circuits/bell-state.circuit
```

Analyze the same circuit against the three supplied topologies:

```bash
./qmlc --analyze examples/pqc4.circuit --topology examples/line4.graph
./qmlc --analyze examples/pqc4.circuit --topology examples/shortcut4.graph
./qmlc --analyze examples/pqc4.circuit --topology examples/disconnected4.graph
```

Run deterministic production QML controls:

```bash
./qmlc --qml encoding-only
./qmlc --qml initialized
./qmlc --qml trained
./qmlc --qml zero
./qmlc --qml train
```

On PowerShell, use `.\qmlc.exe` and Windows path separators if preferred.

Observed full-cost summaries on 22 October 2026:

| Topology | Distances | SWAPs | Extra CX | Total CX |
|---|---|---:|---:|---:|
| Line | `1,3,1` | 4 | 12 | 15 |
| Shortcut | `1,1,1` | 0 | 0 | 3 |
| Disconnected | `1,NA,1` | NA | NA | NA |

## Input formats

Circuit:

```text
4
RY 0 0.3
CX 0 3
```

Supported gates are `X`, `H`, `RX`, `RY`, `RZ`, and `CX`. The first nonblank line is the logical-qubit count.

Topology:

```text
4
0 1
1 2
2 3
```

The first nonblank line is the physical-vertex count. Every later row is one undirected edge. Reversed/duplicate edges are stored once and generate a warning.

Important bounds: 30 logical qubits for analysis, 20 for file simulation, 10 for interactive simulation, 64 graph vertices, 10,000 gates/edge rows, 1 MiB per file, and 255 content bytes per line.

## Tests

Run the complete suite:

```bash
make test
```

Windows:

```powershell
C:\MinGW\bin\mingw32-make.exe test
```

Individual targets are `test-unit`, `test-analysis-only`, and `test-reference`. The CLI test can also be run directly:

```bash
python3 tests/test_cli.py --stage all
```

The sanitizer target is:

```bash
make sanitize
```

On the recorded Windows environment it cannot link because the installed GCC/Clang distributions do not include ASan/UBSan runtimes; this is documented as unavailable, not passing. Run the same target on a GCC/Clang environment with those runtimes installed.

## Demo library

The tracked [`demos/`](demos/) directory contains 28 examples described by [`demos/manifest.json`](demos/manifest.json). Categories cover fundamental gates, Bell/GHZ circuits, CX routing, the three-topology comparison, production XOR/QML controls and intentional parser failures.

Circuit files use the production input format. Topology files remain separate graph inputs. Invalid examples under `demos/invalid/` are negative tests and should return a nonzero exit status with a diagnostic.

Routing classifications mean:

- `DIRECT`: control and target share an edge.
- `INDIRECT`: BFS found a path longer than one edge.
- `UNREACHABLE`: no graph path exists, so the complete workload cost is `NA`.

## Demo Lab and presentation

First compile the C application. In one PowerShell window, start the localhost bridge:

```powershell
cd presentation
npm install
npm run bridge
```

In a second window, start Vite:

```bash
cd presentation
npm run dev
```

Open the displayed local URL. The default view is the Demo Lab; choose **Presentation** to open the slides. The bridge listens only on `127.0.0.1:8787`, accepts only documented operations, limits inputs, uses temporary files and invokes `qmlc` without a shell.

Use arrow keys, Page Up/Page Down, Home/End or the slide controls to navigate. Press `N` or choose **Notes** for presenter explanations. Build the frontend with:

```bash
npm run build
```

With the Vite server running and a Playwright Chromium browser installed, export the slides with:

```powershell
npx playwright install chromium
$env:PQC_PRESENTATION_URL='http://127.0.0.1:5173/#slides'
npm run export:pdf
```

The CLI remains fully independent of the bridge and frontend.

## Documentation and evidence

- [Final case-study report](docs/final-report.md)
- [Demo Lab extension note](docs/demo-lab-extension.md)
- Final 10-slide faculty briefing: `final-presentation.pdf` (also available in `presentation/final-presentation.pdf`)
- Presenter notes for every slide: `presentation/speaker-notes.md`
- DOC-01 through DOC-07 and the immutable baseline archive are retained under `docs/`.
- New transcripts are generated under ignored `build/evidence/`.
- `python tests/render_evidence.py` rebuilds the report screenshots from those transcripts.

The required DSA style-reference file `DSA_Journal_Experiments_6_to_13_Simple_Style.md` was not present in the supplied workspace or upstream repository. The implementation therefore follows the explicit simple-C rules in the master prompt and the fixed DOC-05 interfaces; it does not claim to have read the missing file.
