#ifndef PQC_CIRCUIT_H
#define PQC_CIRCUIT_H

#include "common.h"

typedef enum {
    GATE_X = 0,
    GATE_H = 1,
    GATE_RX = 2,
    GATE_RY = 3,
    GATE_RZ = 4,
    GATE_CX = 5
} GateKind;

typedef struct {
    GateKind kind;
    int target;
    int control;
    double angle;
} Gate;

typedef struct {
    Gate gate;
    size_t source_line;
} Operation;

typedef struct {
    size_t logical_qubits;
    size_t count;
    size_t capacity;
    Operation *operations;
} Circuit;

PqcStatus gate_validate(const Gate *gate, size_t qubits, Diagnostic *d);
PqcStatus circuit_init(Circuit *out, size_t qubits, Diagnostic *d);
PqcStatus circuit_append(Circuit *circuit, Gate gate,
                         size_t source_line, Diagnostic *d);
PqcStatus circuit_validate(const Circuit *circuit, Diagnostic *d);
void circuit_destroy(Circuit *circuit);

#endif
