# Quantum Circuit Demo Lab Extension

## Scope addition

This companion layer was added after the approved DOC-01 through DOC-07 implementation. It does not change their core algorithms or redefine the original scope. The C modules remain the reference implementation for parsing, statevector gates, BFS, routing cost and XOR training.

The extension adds:

- Deterministic `--simulate` and `--qml` CLI entry points that call existing production modules.
- A tracked library of readable circuit, topology and negative-test files.
- A localhost Node bridge with fixed executable and source allowlists, bounded requests, a 15-second process timeout, safe temporary files and no shell execution.
- A Vite + React Demo Lab for editing inputs, running the C application, viewing stdout/stderr/exit status and reading actual C source.
- Updated presentation slides with presenter notes stored in `presentation/src/main.jsx`.

## Verified execution

On 22 October 2026, `python tests/test_demos.py` executed all 28 manifest entries through `qmlc.exe`. Every valid demo returned success and every intentional error returned a nonzero status with stderr. The same-circuit topology checks reproduced:

| Topology | SWAPs | Extra CX | Total CX |
|---|---:|---:|---:|
| Line | 4 | 12 | 15 |
| Line plus shortcut | 0 | 0 | 3 |
| Disconnected | NA | NA | NA |

The browser integration test selected a topology demo, ran the line and shortcut cases through the bridge, checked the returned totals, opened `graph.c`, verified the DS Lab title label and displayed presenter notes.

## Security boundary

The bridge is a local teaching tool, not a remote service. It binds to `127.0.0.1`, accepts only simulation, analysis and five documented QML modes, and exposes only an explicit list of source files. Browser input never enters a shell command. Temporary circuit and graph files are removed after each run.

## Model limitations

The Demo Lab shows ideal simulation and a simplified routing estimate. It does not provide GPU execution, noise simulation, hardware timing, fidelity prediction, optimal placement or optimal routing. The XOR controls explicitly show that binary RY encoding plus odd-parity readout already represents the four labels; the training run does not establish generalization.
