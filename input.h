#ifndef PQC_INPUT_H
#define PQC_INPUT_H

#include "common.h"
#include "circuit.h"
#include "graph.h"

PqcStatus input_load_circuit(const char *filename, Circuit *out,
                             Diagnostic *d);
PqcStatus input_load_topology(const char *filename, Graph **out,
                              FILE *warnings, Diagnostic *d);
PqcStatus input_parse_gate(const char *line, size_t length, size_t qubits,
                           Gate *out, bool *present, Diagnostic *d);
PqcStatus input_parse_count(const char *line, size_t length,
                            size_t maximum, size_t *out, Diagnostic *d);
PqcStatus input_terminal_line(FILE *stream,
                              char out[PQC_MAX_LINE_BYTES + 1u],
                              size_t *length, bool *eof, Diagnostic *d);

#ifdef PQC_TESTING
void input_test_fail_read_on(size_t attempt);
#endif

#endif
