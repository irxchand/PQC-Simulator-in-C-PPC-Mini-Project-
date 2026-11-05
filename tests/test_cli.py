import argparse
import hashlib
import os
import pathlib
import subprocess
import tempfile


parser = argparse.ArgumentParser()
parser.add_argument("--stage", choices=("menu", "all"), default="all")
args = parser.parse_args()
app = os.environ.get("PQC_TEST_APP", "qmlc.exe" if os.name == "nt" else "./qmlc")
root = pathlib.Path(__file__).resolve().parents[1]


def run(arguments=(), stdin=""):
    return subprocess.run([app, *arguments], input=stdin, text=True,
                          capture_output=True, timeout=120, cwd=root)


result = run(stdin="4\n")
assert result.returncode == 0
assert "1. Interactive" in result.stdout and "4. Exit" in result.stdout

result = run(stdin="1\n2\nH 0\nCX 0 1\nDONE\n4\n")
assert result.returncode == 0
assert "|00> : 0.5000" in result.stdout and "|11> : 0.5000" in result.stdout
assert "Gates applied: 2" in result.stdout

result = run(stdin="1\n0\n4\n")
assert "Initialized 2 qubits" not in result.stdout
assert "E_RANGE" in result.stderr

result = run(stdin=f"2\n{root / 'circuit.txt'}\n4\n")
assert result.returncode == 0 and "Gates applied: 4" in result.stdout
assert "IDEAL LOGICAL SIMULATION" in result.stdout

with tempfile.TemporaryDirectory() as directory:
    temporary = pathlib.Path(directory)
    bad = temporary / "bad.circuit"
    bad.write_text("2\nX 0\nH 1\nBOGUS 0\n", encoding="ascii")
    result = run(stdin=f"2\n{bad}\n4\n")
    assert "IDEAL LOGICAL SIMULATION" not in result.stdout
    assert "Gates applied:" not in result.stdout
    assert "E_FORMAT" in result.stderr

    if args.stage == "all":
        line_too_long = temporary / "line-too-long.circuit"
        line_too_long.write_text("2\n" + "X" * 256 + "\n", encoding="ascii")
        result = run(["--analyze", str(line_too_long), "--topology",
                      str(root / "examples" / "line4.graph")])
        assert result.returncode == 2 and "E_FORMAT" in result.stderr

        file_too_large = temporary / "too-large.circuit"
        file_too_large.write_bytes(b"2\n" + b" " * 1_048_575)
        result = run(["--analyze", str(file_too_large), "--topology",
                      str(root / "examples" / "line4.graph")])
        assert result.returncode == 4 and "E_LIMIT" in result.stderr

        too_many_gates = temporary / "too-many-gates.circuit"
        too_many_gates.write_text("1\n" + "X 0\n" * 10_001,
                                   encoding="ascii")
        result = run(["--analyze", str(too_many_gates), "--topology",
                      str(root / "examples" / "line4.graph")])
        assert result.returncode == 4 and "E_LIMIT" in result.stderr

        too_many_vertices = temporary / "too-many-vertices.graph"
        too_many_vertices.write_text("65\n", encoding="ascii")
        result = run(["--analyze", str(root / "examples" / "pqc4.circuit"),
                      "--topology", str(too_many_vertices)])
        assert result.returncode == 4 and "E_LIMIT" in result.stderr

        too_small = temporary / "too-small.graph"
        too_small.write_text("3\n0 1\n1 2\n", encoding="ascii")
        result = run(["--analyze", str(root / "examples" / "pqc4.circuit"),
                      "--topology", str(too_small)])
        assert result.returncode == 2 and "E_MAPPING" in result.stderr

        twenty_one = temporary / "twenty-one.circuit"
        twenty_one.write_text("21\n", encoding="ascii")
        result = run(stdin=f"2\n{twenty_one}\n4\n")
        assert "IDEAL LOGICAL SIMULATION" not in result.stdout
        assert "E_LIMIT" in result.stderr

if args.stage == "menu":
    print("Menu CLI checks passed.")
    raise SystemExit(0)

result = run(["--help"])
assert result.returncode == 0 and "RESTORE-3CX-v1" in result.stdout

circuit = root / "examples" / "pqc4.circuit"
before = hashlib.sha256(circuit.read_bytes()).hexdigest()
expectations = {
    "line4.graph": ("distance=1", "distance=3", "estimated_swaps_total=4", "total_cx_equivalents=15"),
    "shortcut4.graph": ("cx_direct=3", "estimated_swaps_total=0", "total_cx_equivalents=3"),
    "disconnected4.graph": ("distance=NA", "cost_status=INCOMPLETE_UNREACHABLE", "estimated_swaps_total=NA"),
}
for graph_name, fields in expectations.items():
    graph = root / "examples" / graph_name
    result = run(["--analyze", str(circuit), "--topology", str(graph)])
    assert result.returncode == 0, result.stderr
    assert result.stdout.startswith("ANALYSIS ")
    for field in fields:
        assert field in result.stdout, (graph_name, field, result.stdout)

result = run(["--topology", str(root / "examples" / "line4.graph"),
              "--analyze", str(circuit)])
assert result.returncode == 0 and "total_cx_equivalents=15" in result.stdout
assert hashlib.sha256(circuit.read_bytes()).hexdigest() == before

for arguments in (["--analyze"], ["--unknown"],
                  ["--analyze", str(circuit), "--analyze", str(circuit),
                   "--topology", "x"]):
    result = run(arguments)
    assert result.returncode == 2
    assert "ANALYSIS " not in result.stdout

result = run(["--analyze", "missing.circuit", "--topology", "missing.graph"])
assert result.returncode == 3 and "ANALYSIS " not in result.stdout
assert "E_IO" in result.stderr
print("Complete CLI checks passed.")
