#ifndef PQC_COMMON_H
#define PQC_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define PQC_PI 3.14159265358979323846
#define PQC_MAX_LOGICAL_QUBITS 30u
#define PQC_MAX_SIM_QUBITS 20u
#define PQC_MAX_INTERACTIVE_QUBITS 10u
#define PQC_MAX_VERTICES 64u
#define PQC_MAX_GATES 10000u
#define PQC_MAX_EDGE_ROWS 10000u
#define PQC_MAX_FILE_BYTES 1048576u
#define PQC_MAX_LINE_BYTES 255u
#define PQC_NUMERIC_TOL 1e-10

typedef enum {
    PQC_OK = 0,
    PQC_E_USAGE,
    PQC_E_FORMAT,
    PQC_E_RANGE,
    PQC_E_MAPPING,
    PQC_E_IO,
    PQC_E_LIMIT,
    PQC_E_ALLOC,
    PQC_E_NUMERIC,
    PQC_E_INTERNAL
} PqcStatus;

typedef struct {
    PqcStatus code;
    const char *file;
    size_t line;
    char message[192];
} Diagnostic;

void pqc_diag_clear(Diagnostic *d);
void pqc_diag_set(Diagnostic *d, PqcStatus code, const char *file,
                  size_t line, const char *format, ...);
const char *pqc_status_name(PqcStatus code);
int pqc_exit_code(PqcStatus code);
PqcStatus pqc_diag_print(FILE *stream, const Diagnostic *d);

bool pqc_size_add(size_t a, size_t b, size_t *out);
bool pqc_size_mul(size_t a, size_t b, size_t *out);

void *pqc_malloc(size_t bytes);
void *pqc_calloc(size_t count, size_t element_size);
void *pqc_realloc(void *pointer, size_t bytes);
void pqc_free(void *pointer);

#ifdef PQC_TESTING
void pqc_test_fail_alloc_on(size_t attempt);
size_t pqc_test_alloc_attempts(void);
size_t pqc_test_live_blocks(void);
#endif

#endif
