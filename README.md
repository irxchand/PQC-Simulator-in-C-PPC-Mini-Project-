# Quantum Circuit Simulator & PQC Classifier in C

## Overview

This project implements a low-level quantum circuit simulator using C, combined with a Parameterized Quantum Circuit (PQC) for machine learning tasks.

It provides a fully transparent environment to simulate quantum gates, visualize state evolution, and train a quantum-inspired classifier.

---

## Motivation

Quantum circuits are often treated as black boxes in high-level frameworks. This project focuses on:

- Understanding quantum state transformations at a low level
- Providing step-by-step visibility of gate operations
- Bridging quantum computing and machine learning concepts

---

## System Architecture
Input (CLI / File) to Gate Parser to Statevector Engine (2^n amplitudes) to Gate Application (X, H, RX, RY, RZ, CX) to State Update + Normalization to Measurement (Probability Output) to PQC Classifier (Optional)


---

## Core Features

### 1. Statevector Simulation

- Represents quantum state as complex amplitude vector
- Supports up to 30 qubits (limited by memory)
- Normalization ensures valid quantum states

---

### 2. Quantum Gate Engine

Implemented gates:

- X (Pauli-X)
- H (Hadamard)
- RX, RY, RZ (rotation gates)
- CX (controlled NOT)

Applies transformations directly on statevector.

---

### 3. Circuit Execution

- Interactive mode (manual gate input)
- File-based execution (circuit definition)

Example input:
2
RY 0 1.5708
H 1
CX 0 1
RZ 1 3.1416


---

### 4. Explainability Layer

- Describes each gate in plain language
- Shows probability distribution after each step
- Enables step-by-step understanding of quantum behavior

---

### 5. Parameterized Quantum Circuit (PQC)

Implements a simple quantum machine learning model:

- Data encoding via RY rotations
- Trainable parameters
- Entanglement via CX gates
- Measurement-based output

---

### 6. Training (XOR Problem)

- Uses gradient descent with finite differences
- Optimizes parameters to solve XOR
- Demonstrates non-linear decision boundary

---

## How to Run

### Compile
gcc a.c -o qmlc -lm

### Run
./qmlc


---

## Modes

1. Interactive Mode  
   - Enter gates manually  

2. File Mode  
   - Load circuit from file  

3. Training Mode  
   - Train PQC on XOR problem  

---

## Example Output

- State probabilities after each gate
- Final quantum state distribution
- Classification predictions

---

## Limitations

- Exponential memory growth (2^n)
- Limited to small qubit systems
- Finite-difference gradients (inefficient)
- No GPU acceleration

---

## Future Improvements

- Matrix-based gate optimization
- Variational quantum circuits
- Integration with real quantum frameworks
- Hybrid classical-quantum pipelines
- GPU acceleration

---

## Key Takeaway

This project demonstrates how quantum circuits can be simulated and understood at a low level, while also showcasing their application in machine learning through PQCs.

---

## Author

Ishaan Chand
