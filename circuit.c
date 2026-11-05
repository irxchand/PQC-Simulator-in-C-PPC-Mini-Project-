#include "circuit.h"

#include <inttypes.h>
#include <math.h>

static bool circuit_is_empty_owner(const Circuit *circuit)
{
    return circuit != NULL && circuit->logical_qubits == 0 &&
           circuit->count == 0 && circuit->capacity == 0 &&
           circuit->operations == NULL;
}

PqcStatus gate_validate(const Gate *gate, size_t qubits, Diagnostic *d)
{
    if (gate == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0, "gate is missing");
        return PQC_E_INTERNAL;
    }
    if (qubits == 0) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "logical qubit count must be positive");
        return PQC_E_RANGE;
    }
    if (qubits > PQC_MAX_LOGICAL_QUBITS) {
        pqc_diag_set(d, PQC_E_LIMIT, NULL, 0,
                     "logical qubit count exceeds %u",
                     PQC_MAX_LOGICAL_QUBITS);
        return PQC_E_LIMIT;
    }
    if (gate->kind < GATE_X || gate->kind > GATE_CX) {
        pqc_diag_set(d, PQC_E_FORMAT, NULL, 0, "unsupported gate kind");
        return PQC_E_FORMAT;
    }
    if (gate->target < 0 || (size_t)gate->target >= qubits) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0, "target qubit is out of range");
        return PQC_E_RANGE;
    }

    if (gate->kind == GATE_CX) {
        if (gate->control < 0 || (size_t)gate->control >= qubits) {
            pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                         "control qubit is out of range");
            return PQC_E_RANGE;
        }
        if (gate->control == gate->target) {
            pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                         "CX control and target must differ");
            return PQC_E_RANGE;
        }
        if (gate->angle != 0.0) {
            pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                         "CX gate must not contain an angle");
            return PQC_E_FORMAT;
        }
        return PQC_OK;
    }

    if (gate->control != -1) {
        pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                     "single-qubit gate must not contain a control");
        return PQC_E_FORMAT;
    }

    if (gate->kind == GATE_X || gate->kind == GATE_H) {
        if (gate->angle != 0.0) {
            pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                         "X and H gates must not contain an angle");
            return PQC_E_FORMAT;
        }
    } else if (!isfinite(gate->angle)) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "rotation angle must be finite");
        return PQC_E_RANGE;
    }

    return PQC_OK;
}

PqcStatus circuit_init(Circuit *out, size_t qubits, Diagnostic *d)
{
    if (out == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "circuit output is missing");
        return PQC_E_INTERNAL;
    }
    if (!circuit_is_empty_owner(out)) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "circuit output must be empty");
        return PQC_E_INTERNAL;
    }
    if (qubits == 0) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "logical qubit count must be positive");
        return PQC_E_RANGE;
    }
    if (qubits > PQC_MAX_LOGICAL_QUBITS) {
        pqc_diag_set(d, PQC_E_LIMIT, NULL, 0,
                     "logical qubit count exceeds %u",
                     PQC_MAX_LOGICAL_QUBITS);
        return PQC_E_LIMIT;
    }

    out->logical_qubits = qubits;
    return PQC_OK;
}

PqcStatus circuit_append(Circuit *circuit, Gate gate,
                         size_t source_line, Diagnostic *d)
{
    PqcStatus status;

    if (circuit == NULL || circuit->logical_qubits == 0) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, source_line,
                     "circuit is not initialized");
        return PQC_E_INTERNAL;
    }
    if (source_line == 0) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "operation source line must be positive");
        return PQC_E_INTERNAL;
    }

    status = circuit_validate(circuit, d);
    if (status != PQC_OK) {
        return status;
    }
    status = gate_validate(&gate, circuit->logical_qubits, d);
    if (status != PQC_OK) {
        return status;
    }
    if (circuit->count == PQC_MAX_GATES) {
        pqc_diag_set(d, PQC_E_LIMIT, NULL, source_line,
                     "gate count exceeds %u", PQC_MAX_GATES);
        return PQC_E_LIMIT;
    }

    if (circuit->count == circuit->capacity) {
        size_t new_capacity;
        size_t bytes;
        Operation *new_operations;

        if (circuit->capacity == 0) {
            new_capacity = 16;
        } else if (!pqc_size_mul(circuit->capacity, 2, &new_capacity)) {
            pqc_diag_set(d, PQC_E_LIMIT, NULL, source_line,
                         "circuit capacity overflow");
            return PQC_E_LIMIT;
        }
        if (new_capacity > PQC_MAX_GATES) {
            new_capacity = PQC_MAX_GATES;
        }
        if (!pqc_size_mul(new_capacity, sizeof(Operation), &bytes)) {
            pqc_diag_set(d, PQC_E_LIMIT, NULL, source_line,
                         "circuit allocation size overflow");
            return PQC_E_LIMIT;
        }

        new_operations = pqc_realloc(circuit->operations, bytes);
        if (new_operations == NULL) {
            pqc_diag_set(d, PQC_E_ALLOC, NULL, source_line,
                         "could not grow circuit storage");
            return PQC_E_ALLOC;
        }
        circuit->operations = new_operations;
        circuit->capacity = new_capacity;
    }

    circuit->operations[circuit->count].gate = gate;
    circuit->operations[circuit->count].source_line = source_line;
    circuit->count++;
    return PQC_OK;
}

PqcStatus circuit_validate(const Circuit *circuit, Diagnostic *d)
{
    size_t i;

    if (circuit == NULL || circuit->logical_qubits == 0) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "circuit is not initialized");
        return PQC_E_INTERNAL;
    }
    if (circuit->logical_qubits > PQC_MAX_LOGICAL_QUBITS ||
        circuit->count > circuit->capacity ||
        circuit->capacity > PQC_MAX_GATES) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "circuit metadata is invalid");
        return PQC_E_INTERNAL;
    }
    if (circuit->count == 0) {
        if (circuit->capacity != 0 || circuit->operations != NULL) {
            pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                         "empty circuit owns unexpected storage");
            return PQC_E_INTERNAL;
        }
        return PQC_OK;
    }
    if (circuit->operations == NULL || circuit->capacity == 0) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "populated circuit has no storage");
        return PQC_E_INTERNAL;
    }

    for (i = 0; i < circuit->count; i++) {
        PqcStatus status;

        if (circuit->operations[i].source_line == 0) {
            pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                         "operation %" PRIuMAX " has no source line",
                         (uintmax_t)(i + 1));
            return PQC_E_INTERNAL;
        }
        status = gate_validate(&circuit->operations[i].gate,
                               circuit->logical_qubits, d);
        if (status != PQC_OK) {
            return status;
        }
    }
    return PQC_OK;
}

void circuit_destroy(Circuit *circuit)
{
    if (circuit == NULL) {
        return;
    }

    pqc_free(circuit->operations);
    circuit->logical_qubits = 0;
    circuit->count = 0;
    circuit->capacity = 0;
    circuit->operations = NULL;
}
