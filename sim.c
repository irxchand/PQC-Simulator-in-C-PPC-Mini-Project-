#include "sim.h"

#include <inttypes.h>
#include <math.h>
#include <string.h>

static bool state_owner_is_empty(const SimState *state)
{
    return state != NULL && state->qubits == 0 && state->size == 0 &&
           state->amplitude == NULL && !state->usable;
}

static PqcStatus state_shape(const SimState *state, Diagnostic *d)
{
    size_t expected;

    if (state == NULL || !state->usable || state->amplitude == NULL ||
        state->qubits == 0 || state->qubits > PQC_MAX_SIM_QUBITS) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "simulation state is not usable");
        return PQC_E_INTERNAL;
    }
    expected = ((size_t)1) << state->qubits;
    if (state->size != expected) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "simulation state size is inconsistent");
        return PQC_E_INTERNAL;
    }
    return PQC_OK;
}

static PqcStatus squared_norm(const SimState *state, double *out,
                              Diagnostic *d)
{
    double sum = 0.0;
    double correction = 0.0;
    size_t i;

    for (i = 0; i < state->size; i++) {
        double real = state->amplitude[i].r;
        double imag = state->amplitude[i].i;
        double term;
        double adjusted;
        double next;

        if (!isfinite(real) || !isfinite(imag)) {
            pqc_diag_set(d, PQC_E_NUMERIC, NULL, 0,
                         "state contains a nonfinite amplitude");
            return PQC_E_NUMERIC;
        }
        term = real * real + imag * imag;
        if (!isfinite(term)) {
            pqc_diag_set(d, PQC_E_NUMERIC, NULL, 0,
                         "state norm contains a nonfinite term");
            return PQC_E_NUMERIC;
        }
        adjusted = term - correction;
        next = sum + adjusted;
        correction = (next - sum) - adjusted;
        sum = next;
    }
    if (!isfinite(sum)) {
        pqc_diag_set(d, PQC_E_NUMERIC, NULL, 0,
                     "state norm is nonfinite");
        return PQC_E_NUMERIC;
    }
    *out = sum;
    return PQC_OK;
}

static PqcStatus require_unit_norm(SimState *state, double *norm,
                                   Diagnostic *d)
{
    PqcStatus status = squared_norm(state, norm, d);

    if (status != PQC_OK || *norm <= 0.0 ||
        fabs(*norm - 1.0) > PQC_NUMERIC_TOL) {
        state->usable = false;
        if (status == PQC_OK) {
            pqc_diag_set(d, PQC_E_NUMERIC, NULL, 0,
                         "state squared norm is outside tolerance");
        }
        return PQC_E_NUMERIC;
    }
    return PQC_OK;
}

PqcStatus sim_create(size_t qubits, SimState *out, Diagnostic *d)
{
    SimState temporary = {0};
    size_t bytes;

    if (!state_owner_is_empty(out)) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "simulation output must be empty");
        return PQC_E_INTERNAL;
    }
    if (qubits == 0) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "simulation qubit count must be positive");
        return PQC_E_RANGE;
    }
    if (qubits > PQC_MAX_SIM_QUBITS) {
        pqc_diag_set(d, PQC_E_LIMIT, NULL, 0,
                     "simulation supports at most %u qubits",
                     PQC_MAX_SIM_QUBITS);
        return PQC_E_LIMIT;
    }

    temporary.qubits = qubits;
    temporary.size = ((size_t)1) << qubits;
    if (!pqc_size_mul(temporary.size, sizeof(Complex), &bytes)) {
        pqc_diag_set(d, PQC_E_LIMIT, NULL, 0,
                     "statevector allocation size overflow");
        return PQC_E_LIMIT;
    }
    temporary.amplitude = pqc_calloc(temporary.size, sizeof(Complex));
    if (temporary.amplitude == NULL) {
        pqc_diag_set(d, PQC_E_ALLOC, NULL, 0,
                     "could not allocate statevector");
        return PQC_E_ALLOC;
    }
    (void)bytes;
    temporary.amplitude[0].r = 1.0;
    temporary.usable = true;
    *out = temporary;
    return PQC_OK;
}

void sim_destroy(SimState *state)
{
    if (state == NULL) {
        return;
    }
    pqc_free(state->amplitude);
    memset(state, 0, sizeof(*state));
}

PqcStatus sim_apply_gate(SimState *state, const Gate *gate, Diagnostic *d)
{
    PqcStatus status;
    double norm;
    size_t step;
    size_t i;

    status = state_shape(state, d);
    if (status != PQC_OK) {
        return status;
    }
    status = gate_validate(gate, state->qubits, d);
    if (status != PQC_OK) {
        return status;
    }
    status = require_unit_norm(state, &norm, d);
    if (status != PQC_OK) {
        return status;
    }

    step = ((size_t)1) << (size_t)gate->target;
    if (gate->kind == GATE_X) {
        for (i = 0; i < state->size; i += 2 * step) {
            size_t j;
            for (j = 0; j < step; j++) {
                Complex temporary = state->amplitude[i + j];
                state->amplitude[i + j] = state->amplitude[i + j + step];
                state->amplitude[i + j + step] = temporary;
            }
        }
    } else if (gate->kind == GATE_H) {
        double value = 1.0 / sqrt(2.0);
        for (i = 0; i < state->size; i += 2 * step) {
            size_t j;
            for (j = 0; j < step; j++) {
                Complex a = state->amplitude[i + j];
                Complex b = state->amplitude[i + j + step];
                state->amplitude[i + j].r = (a.r + b.r) * value;
                state->amplitude[i + j].i = (a.i + b.i) * value;
                state->amplitude[i + j + step].r = (a.r - b.r) * value;
                state->amplitude[i + j + step].i = (a.i - b.i) * value;
            }
        }
    } else if (gate->kind == GATE_RX) {
        double cosine = cos(gate->angle / 2.0);
        double sine = sin(gate->angle / 2.0);
        for (i = 0; i < state->size; i += 2 * step) {
            size_t j;
            for (j = 0; j < step; j++) {
                Complex a = state->amplitude[i + j];
                Complex b = state->amplitude[i + j + step];
                state->amplitude[i + j].r = a.r * cosine + b.i * sine;
                state->amplitude[i + j].i = a.i * cosine - b.r * sine;
                state->amplitude[i + j + step].r = b.r * cosine + a.i * sine;
                state->amplitude[i + j + step].i = b.i * cosine - a.r * sine;
            }
        }
    } else if (gate->kind == GATE_RY) {
        double cosine = cos(gate->angle / 2.0);
        double sine = sin(gate->angle / 2.0);
        for (i = 0; i < state->size; i += 2 * step) {
            size_t j;
            for (j = 0; j < step; j++) {
                Complex a = state->amplitude[i + j];
                Complex b = state->amplitude[i + j + step];
                state->amplitude[i + j].r = a.r * cosine - b.r * sine;
                state->amplitude[i + j].i = a.i * cosine - b.i * sine;
                state->amplitude[i + j + step].r = a.r * sine + b.r * cosine;
                state->amplitude[i + j + step].i = a.i * sine + b.i * cosine;
            }
        }
    } else if (gate->kind == GATE_RZ) {
        double low_cosine = cos(-gate->angle / 2.0);
        double low_sine = sin(-gate->angle / 2.0);
        double high_cosine = cos(gate->angle / 2.0);
        double high_sine = sin(gate->angle / 2.0);
        for (i = 0; i < state->size; i += 2 * step) {
            size_t j;
            for (j = 0; j < step; j++) {
                Complex a = state->amplitude[i + j];
                Complex b = state->amplitude[i + j + step];
                state->amplitude[i + j].r = a.r * low_cosine - a.i * low_sine;
                state->amplitude[i + j].i = a.r * low_sine + a.i * low_cosine;
                state->amplitude[i + j + step].r = b.r * high_cosine - b.i * high_sine;
                state->amplitude[i + j + step].i = b.r * high_sine + b.i * high_cosine;
            }
        }
    } else {
        for (i = 0; i < state->size; i++) {
            bool control = ((i >> (size_t)gate->control) & 1u) != 0;
            bool target = ((i >> (size_t)gate->target) & 1u) != 0;
            if (control && !target) {
                size_t flipped = i | (((size_t)1) << (size_t)gate->target);
                Complex temporary = state->amplitude[i];
                state->amplitude[i] = state->amplitude[flipped];
                state->amplitude[flipped] = temporary;
            }
        }
    }

    status = require_unit_norm(state, &norm, d);
    if (status != PQC_OK) {
        return status;
    }
    norm = sqrt(norm);
    for (i = 0; i < state->size; i++) {
        state->amplitude[i].r /= norm;
        state->amplitude[i].i /= norm;
    }
    return require_unit_norm(state, &norm, d);
}

PqcStatus sim_odd_probability(const SimState *state, double *out,
                              Diagnostic *d)
{
    double norm;
    double sum = 0.0;
    double correction = 0.0;
    size_t i;
    PqcStatus status;

    if (out == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "probability output is missing");
        return PQC_E_INTERNAL;
    }
    status = state_shape(state, d);
    if (status != PQC_OK) {
        return status;
    }
    status = squared_norm(state, &norm, d);
    if (status != PQC_OK || norm <= 0.0 ||
        fabs(norm - 1.0) > PQC_NUMERIC_TOL) {
        if (status == PQC_OK) {
            pqc_diag_set(d, PQC_E_NUMERIC, NULL, 0,
                         "state squared norm is outside tolerance");
        }
        return PQC_E_NUMERIC;
    }

    for (i = 0; i < state->size; i++) {
        size_t bits = i;
        unsigned parity = 0;
        double term;
        double adjusted;
        double next;

        while (bits > 0) {
            parity ^= (unsigned)(bits & 1u);
            bits >>= 1;
        }
        if (parity == 0) {
            continue;
        }
        term = state->amplitude[i].r * state->amplitude[i].r +
               state->amplitude[i].i * state->amplitude[i].i;
        adjusted = term - correction;
        next = sum + adjusted;
        correction = (next - sum) - adjusted;
        sum = next;
    }
    if (!isfinite(sum) || sum < -PQC_NUMERIC_TOL ||
        sum > 1.0 + PQC_NUMERIC_TOL) {
        pqc_diag_set(d, PQC_E_NUMERIC, NULL, 0,
                     "odd-parity probability is invalid");
        return PQC_E_NUMERIC;
    }
    if (sum < 0.0) {
        sum = 0.0;
    } else if (sum > 1.0) {
        sum = 1.0;
    }
    *out = sum;
    return PQC_OK;
}

PqcStatus sim_write_gate(FILE *stream, const Gate *gate, Diagnostic *d)
{
    int result;

    if (stream == NULL || gate == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "gate display argument is missing");
        return PQC_E_INTERNAL;
    }
    if (gate->kind == GATE_X) {
        result = fprintf(stream, "[X Gate] Applied to qubit %d. Flips the state probability between |0> and |1>.\n", gate->target);
    } else if (gate->kind == GATE_H) {
        result = fprintf(stream, "[H Gate] Applied to qubit %d. Creates superposition, mixing |0> and |1> evenly.\n", gate->target);
    } else if (gate->kind == GATE_RX) {
        result = fprintf(stream, "[RX Gate] Target %d, theta %.4f. Rotates state probability amplitudes around the X-axis of the Bloch sphere.\n", gate->target, gate->angle);
    } else if (gate->kind == GATE_RY) {
        result = fprintf(stream, "[RY Gate] Target %d, theta %.4f. Rotates state probability amplitudes around the Y-axis.\n", gate->target, gate->angle);
    } else if (gate->kind == GATE_RZ) {
        result = fprintf(stream, "[RZ Gate] Target %d, theta %.4f. Applies a relative phase shift around the Z-axis (does not change probability magnitudes).\n", gate->target, gate->angle);
    } else {
        result = fprintf(stream, "[CX Gate] Control %d, Target %d. Flips target if control is |1>. Used to create entanglement.\n", gate->control, gate->target);
    }
    if (result < 0) {
        pqc_diag_set(d, PQC_E_IO, NULL, 0, "could not write gate trace");
        return PQC_E_IO;
    }
    return PQC_OK;
}

PqcStatus sim_write_probabilities(FILE *stream, const SimState *state,
                                  Diagnostic *d)
{
    size_t i;
    PqcStatus status = state_shape(state, d);

    if (stream == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "probability stream is missing");
        return PQC_E_INTERNAL;
    }
    if (status != PQC_OK) {
        return status;
    }
    for (i = 0; i < state->size; i++) {
        double probability = state->amplitude[i].r * state->amplitude[i].r +
                             state->amplitude[i].i * state->amplitude[i].i;
        if (probability > 1e-4) {
            size_t bit;
            if (fprintf(stream, " |") < 0) {
                goto write_error;
            }
            for (bit = state->qubits; bit > 0; bit--) {
                if (fprintf(stream, "%u",
                            (unsigned)((i >> (bit - 1)) & 1u)) < 0) {
                    goto write_error;
                }
            }
            if (fprintf(stream, "> : %.4f\n", probability) < 0) {
                goto write_error;
            }
        }
    }
    if (fprintf(stream, "\n") < 0) {
        goto write_error;
    }
    return PQC_OK;

write_error:
    pqc_diag_set(d, PQC_E_IO, NULL, 0,
                 "could not write probability trace");
    return PQC_E_IO;
}

PqcStatus sim_execute(const Circuit *circuit, FILE *trace,
                      SimState *out, Diagnostic *d)
{
    SimState temporary = {0};
    PqcStatus status;
    size_t i;

    if (!state_owner_is_empty(out)) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "simulation output must be empty");
        return PQC_E_INTERNAL;
    }
    status = circuit_validate(circuit, d);
    if (status != PQC_OK) {
        return status;
    }
    status = sim_create(circuit->logical_qubits, &temporary, d);
    if (status != PQC_OK) {
        return status;
    }
    if (trace != NULL) {
        if (fprintf(trace, "IDEAL LOGICAL SIMULATION (no routing or noise)\nInitialized %" PRIuMAX " qubits.\n",
                    (uintmax_t)circuit->logical_qubits) < 0) {
            pqc_diag_set(d, PQC_E_IO, NULL, 0,
                         "could not write simulation trace");
            status = PQC_E_IO;
            goto fail;
        }
        status = sim_write_probabilities(trace, &temporary, d);
        if (status != PQC_OK) {
            goto fail;
        }
    }
    for (i = 0; i < circuit->count; i++) {
        status = sim_apply_gate(&temporary, &circuit->operations[i].gate, d);
        if (status != PQC_OK) {
            if (d != NULL) {
                d->line = circuit->operations[i].source_line;
            }
            goto fail;
        }
        if (trace != NULL) {
            status = sim_write_gate(trace, &circuit->operations[i].gate, d);
            if (status == PQC_OK) {
                status = sim_write_probabilities(trace, &temporary, d);
            }
            if (status != PQC_OK) {
                if (d != NULL) {
                    d->line = circuit->operations[i].source_line;
                }
                goto fail;
            }
        }
    }
    *out = temporary;
    return PQC_OK;

fail:
    sim_destroy(&temporary);
    return status;
}
