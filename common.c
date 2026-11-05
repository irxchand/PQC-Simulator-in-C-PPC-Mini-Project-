#include "common.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef PQC_TESTING
static size_t allocation_attempts = 0;
static size_t fail_on_attempt = 0;
static size_t live_blocks = 0;

static bool allocation_should_fail(void)
{
    allocation_attempts++;
    return fail_on_attempt > 0 && allocation_attempts == fail_on_attempt;
}
#endif

void pqc_diag_clear(Diagnostic *d)
{
    if (d == NULL) {
        return;
    }

    d->code = PQC_OK;
    d->file = NULL;
    d->line = 0;
    d->message[0] = '\0';
}

void pqc_diag_set(Diagnostic *d, PqcStatus code, const char *file,
                  size_t line, const char *format, ...)
{
    va_list arguments;

    if (d == NULL) {
        return;
    }

    d->code = code;
    d->file = file;
    d->line = line;
    d->message[0] = '\0';

    if (format == NULL) {
        return;
    }

    va_start(arguments, format);
    (void)vsnprintf(d->message, sizeof(d->message), format, arguments);
    va_end(arguments);
    d->message[sizeof(d->message) - 1] = '\0';
}

const char *pqc_status_name(PqcStatus code)
{
    switch (code) {
        case PQC_OK: return "OK";
        case PQC_E_USAGE: return "E_USAGE";
        case PQC_E_FORMAT: return "E_FORMAT";
        case PQC_E_RANGE: return "E_RANGE";
        case PQC_E_MAPPING: return "E_MAPPING";
        case PQC_E_IO: return "E_IO";
        case PQC_E_LIMIT: return "E_LIMIT";
        case PQC_E_ALLOC: return "E_ALLOC";
        case PQC_E_NUMERIC: return "E_NUMERIC";
        case PQC_E_INTERNAL: return "E_INTERNAL";
        default: return "E_INTERNAL";
    }
}

int pqc_exit_code(PqcStatus code)
{
    switch (code) {
        case PQC_OK:
            return 0;
        case PQC_E_USAGE:
        case PQC_E_FORMAT:
        case PQC_E_RANGE:
        case PQC_E_MAPPING:
            return 2;
        case PQC_E_IO:
            return 3;
        case PQC_E_LIMIT:
        case PQC_E_ALLOC:
            return 4;
        case PQC_E_NUMERIC:
        case PQC_E_INTERNAL:
        default:
            return 5;
    }
}

PqcStatus pqc_diag_print(FILE *stream, const Diagnostic *d)
{
    int result;

    if (stream == NULL || d == NULL) {
        return PQC_E_IO;
    }

    if (d->file != NULL && d->line > 0) {
        result = fprintf(stream, "%s: %s:%" PRIuMAX ": %s\n",
                         pqc_status_name(d->code), d->file,
                         (uintmax_t)d->line, d->message);
    } else if (d->file != NULL) {
        result = fprintf(stream, "%s: %s: %s\n",
                         pqc_status_name(d->code), d->file, d->message);
    } else {
        result = fprintf(stream, "%s: %s\n",
                         pqc_status_name(d->code), d->message);
    }

    if (result < 0) {
        return PQC_E_IO;
    }
    return PQC_OK;
}

bool pqc_size_add(size_t a, size_t b, size_t *out)
{
    if (out == NULL || a > SIZE_MAX - b) {
        return false;
    }

    *out = a + b;
    return true;
}

bool pqc_size_mul(size_t a, size_t b, size_t *out)
{
    if (out == NULL || (a != 0 && b > SIZE_MAX / a)) {
        return false;
    }

    *out = a * b;
    return true;
}

void *pqc_malloc(size_t bytes)
{
    void *pointer;

    if (bytes == 0) {
        return NULL;
    }

#ifdef PQC_TESTING
    if (allocation_should_fail()) {
        return NULL;
    }
#endif

    pointer = malloc(bytes);
#ifdef PQC_TESTING
    if (pointer != NULL) {
        live_blocks++;
    }
#endif
    return pointer;
}

void *pqc_calloc(size_t count, size_t element_size)
{
    void *pointer;
    size_t bytes;

    if (count == 0 || element_size == 0 ||
        !pqc_size_mul(count, element_size, &bytes)) {
        return NULL;
    }

#ifdef PQC_TESTING
    if (allocation_should_fail()) {
        return NULL;
    }
#endif

    pointer = calloc(count, element_size);
#ifdef PQC_TESTING
    if (pointer != NULL) {
        live_blocks++;
    }
#endif
    return pointer;
}

void *pqc_realloc(void *pointer, size_t bytes)
{
    void *new_pointer;
    bool was_null;

    if (bytes == 0) {
        return NULL;
    }

    was_null = pointer == NULL;
#ifdef PQC_TESTING
    if (allocation_should_fail()) {
        return NULL;
    }
#endif

    new_pointer = realloc(pointer, bytes);
#ifdef PQC_TESTING
    if (new_pointer != NULL && was_null) {
        live_blocks++;
    }
#else
    (void)was_null;
#endif
    return new_pointer;
}

void pqc_free(void *pointer)
{
    if (pointer == NULL) {
        return;
    }

    free(pointer);
#ifdef PQC_TESTING
    if (live_blocks > 0) {
        live_blocks--;
    }
#endif
}

#ifdef PQC_TESTING
void pqc_test_fail_alloc_on(size_t attempt)
{
    fail_on_attempt = attempt;
    allocation_attempts = 0;
}

size_t pqc_test_alloc_attempts(void)
{
    return allocation_attempts;
}

size_t pqc_test_live_blocks(void)
{
    return live_blocks;
}
#endif
