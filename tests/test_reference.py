import cmath
import math
import os
import subprocess


probe = os.environ.get("PQC_TEST_PROBE", "build/reference/reference_probe.exe")
completed = subprocess.run([probe], text=True, capture_output=True, timeout=120)
assert completed.returncode == 0, completed.stderr

bell = {}
qml = {}
train = None
pred = {}
for line in completed.stdout.splitlines():
    fields = line.split()
    if fields[0] == "BELL":
        bell[int(fields[1])] = complex(float(fields[2]), float(fields[3]))
    elif fields[0] == "QML":
        qml[int(fields[1])] = tuple(map(float, fields[2:]))
    elif fields[0] == "TRAIN":
        train = (float(fields[1]), float(fields[2]), int(fields[3]),
                 int(fields[4]), float(fields[5]))
    elif fields[0] == "PRED":
        pred[int(fields[1])] = float(fields[2])

root_two = math.sqrt(0.5)
expected_bell = {0: root_two + 0j, 1: 0j, 2: 0j, 3: root_two + 0j}
assert set(bell) == set(expected_bell)
assert max(abs(bell[i] - expected_bell[i]) for i in bell) <= 1e-12

theta = [2.639, 1.239, 2.461, 2.508]
features = [(0.0, 0.0), (0.0, 1.0), (1.0, 0.0), (1.0, 1.0)]
labels = [0.0, 1.0, 1.0, 0.0]


def ry(vector, target, angle):
    result = vector[:]
    step = 1 << target
    cosine = math.cos(angle / 2.0)
    sine = math.sin(angle / 2.0)
    for base in range(0, len(vector), 2 * step):
        for offset in range(step):
            low = vector[base + offset]
            high = vector[base + offset + step]
            result[base + offset] = low * cosine - high * sine
            result[base + offset + step] = low * sine + high * cosine
    return result


def cx(vector, control, target):
    result = vector[:]
    for index in range(len(vector)):
        if ((index >> control) & 1) and not ((index >> target) & 1):
            other = index | (1 << target)
            result[index], result[other] = result[other], result[index]
    return result


def forward(x, parameters, full=True):
    vector = [1 + 0j, 0j, 0j, 0j]
    for qubit in range(2):
        vector = ry(vector, qubit, x[qubit] * math.pi)
    if full:
        for qubit in range(2):
            vector = ry(vector, qubit, parameters[qubit])
        vector = cx(vector, 0, 1)
        for qubit in range(2):
            vector = ry(vector, qubit, parameters[2 + qubit])
    return math.fsum(abs(vector[index]) ** 2 for index in (1, 2))


assert set(qml) == set(range(4))
for index, x in enumerate(features):
    encoding, initial, zero_full = qml[index]
    assert abs(encoding - forward(x, theta, full=False)) <= 1e-12
    assert abs(initial - forward(x, theta)) <= 1e-10
    assert abs(zero_full - forward(x, [0.0] * 4)) <= 1e-10
    assert abs(encoding - labels[index]) <= 1e-12

assert train is not None and set(pred) == set(range(4))
initial_mse, final_mse, initial_correct, final_correct, epoch_200 = train
assert math.isfinite(epoch_200)
assert final_mse < initial_mse
assert final_correct == 4
assert all((pred[i] >= 0.5) == bool(labels[i]) for i in range(4))
print("Independent simulator/QML reference checks passed.")
