#ifndef PQC_SIM_H
#define PQC_SIM_H

#include "common.h"
#include "circuit.h"

typedef struct {
    double r;
    double i;
} Complex;

typedef struct {
    size_t qubits;
    size_t size;
    Complex *amplitude;
    bool usable;
} SimState;

PqcStatus sim_create(size_t qubits, SimState *out, Diagnostic *d);
void sim_destroy(SimState *state);
PqcStatus sim_apply_gate(SimState *state, const Gate *gate, Diagnostic *d);
PqcStatus sim_odd_probability(const SimState *state, double *out,
                              Diagnostic *d);
PqcStatus sim_write_gate(FILE *stream, const Gate *gate, Diagnostic *d);
PqcStatus sim_write_probabilities(FILE *stream, const SimState *state,
                                  Diagnostic *d);
PqcStatus sim_execute(const Circuit *circuit, FILE *trace,
                      SimState *out, Diagnostic *d);

#endif
