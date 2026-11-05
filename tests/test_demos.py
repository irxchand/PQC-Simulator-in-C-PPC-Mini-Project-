import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
APP = ROOT / ("qmlc.exe" if sys.platform.startswith("win") else "qmlc")
MANIFEST = json.loads((ROOT / "demos" / "manifest.json").read_text(encoding="utf-8"))
TOPOLOGIES = {item["id"]: ROOT / item["file"] for item in MANIFEST["topologies"]}


def run(arguments):
    return subprocess.run(
        [str(APP), *map(str, arguments)],
        cwd=ROOT,
        text=True,
        capture_output=True,
        timeout=20,
        check=False,
    )


for demo in MANIFEST["demos"]:
    if demo["mode"] == "simulate":
        result = run(["--simulate", ROOT / demo["circuit"]])
    elif demo["mode"] == "analyze":
        result = run([
            "--analyze", ROOT / demo["circuit"],
            "--topology", TOPOLOGIES[demo["topology"]],
        ])
    elif demo["mode"] == "qml":
        result = run(["--qml", demo["qmlMode"]])
    else:
        raise AssertionError(f"unknown demo mode: {demo['mode']}")

    if demo["negative"]:
        assert result.returncode != 0, (demo["id"], result.stdout, result.stderr)
        assert result.stderr, demo["id"]
    else:
        assert result.returncode == 0, (demo["id"], result.stdout, result.stderr)


expected_summaries = {
    "compare-line": (
        "estimated_swaps_total=4",
        "extra_cx_equivalents_total=12",
        "total_cx_equivalents=15",
    ),
    "compare-shortcut": (
        "estimated_swaps_total=0",
        "extra_cx_equivalents_total=0",
        "total_cx_equivalents=3",
    ),
    "compare-disconnected": (
        "cost_status=INCOMPLETE_UNREACHABLE",
        "estimated_swaps_total=NA",
        "total_cx_equivalents=NA",
    ),
}

for demo_id, fragments in expected_summaries.items():
    demo = next(item for item in MANIFEST["demos"] if item["id"] == demo_id)
    result = run([
        "--analyze", ROOT / demo["circuit"],
        "--topology", TOPOLOGIES[demo["topology"]],
    ])
    assert result.returncode == 0
    for fragment in fragments:
        assert fragment in result.stdout, (demo_id, fragment, result.stdout)


phase = run(["--simulate", ROOT / "demos/circuits/phase-interference.circuit"])
assert "|1> : 1.0000" in phase.stdout, phase.stdout

bell = run(["--simulate", ROOT / "demos/circuits/bell-state.circuit"])
assert "|00> : 0.5000" in bell.stdout and "|11> : 0.5000" in bell.stdout

print(f"Demo library checks passed: {len(MANIFEST['demos'])} demos.")
