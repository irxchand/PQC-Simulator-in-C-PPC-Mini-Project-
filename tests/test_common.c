#include "common.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int failures = 0;

#define CHECK(condition) do { \
    if (!(condition)) { \
        printf("FAIL test_common:%d: %s\n", __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static void test_statuses_and_diagnostics(void)
{
    Diagnostic d;

    pqc_diag_clear(&d);
    CHECK(d.code == PQC_OK);
    CHECK(d.file == NULL);
    CHECK(d.line == 0);
    CHECK(d.message[0] == '\0');

    pqc_diag_set(&d, PQC_E_FORMAT, "input.circuit", 7,
                 "bad token %d", 4);
    CHECK(d.code == PQC_E_FORMAT);
    CHECK(strcmp(d.file, "input.circuit") == 0);
    CHECK(d.line == 7);
    CHECK(strcmp(d.message, "bad token 4") == 0);
    CHECK(strcmp(pqc_status_name(PQC_E_FORMAT), "E_FORMAT") == 0);
    CHECK(pqc_exit_code(PQC_OK) == 0);
    CHECK(pqc_exit_code(PQC_E_USAGE) == 2);
    CHECK(pqc_exit_code(PQC_E_FORMAT) == 2);
    CHECK(pqc_exit_code(PQC_E_RANGE) == 2);
    CHECK(pqc_exit_code(PQC_E_MAPPING) == 2);
    CHECK(pqc_exit_code(PQC_E_IO) == 3);
    CHECK(pqc_exit_code(PQC_E_LIMIT) == 4);
    CHECK(pqc_exit_code(PQC_E_ALLOC) == 4);
    CHECK(pqc_exit_code(PQC_E_NUMERIC) == 5);
    CHECK(pqc_exit_code(PQC_E_INTERNAL) == 5);
}

static void test_checked_arithmetic(void)
{
    size_t result = 123;

    CHECK(pqc_size_add(20, 22, &result));
    CHECK(result == 42);
    result = 123;
    CHECK(!pqc_size_add(SIZE_MAX, 1, &result));
    CHECK(result == 123);
    CHECK(pqc_size_mul(6, 7, &result));
    CHECK(result == 42);
    result = 123;
    CHECK(!pqc_size_mul(SIZE_MAX, 2, &result));
    CHECK(result == 123);
}

static void test_allocation_accounting(void)
{
    unsigned char *bytes;
    unsigned char *zeros;
    unsigned char *grown;
    size_t start = pqc_test_live_blocks();
    size_t i;

    pqc_test_fail_alloc_on(0);
    CHECK(pqc_malloc(0) == NULL);
    CHECK(pqc_calloc(0, 4) == NULL);
    CHECK(pqc_calloc(SIZE_MAX, 2) == NULL);
    CHECK(pqc_realloc(NULL, 0) == NULL);
    CHECK(pqc_test_alloc_attempts() == 0);
    CHECK(pqc_test_live_blocks() == start);

    pqc_test_fail_alloc_on(1);
    CHECK(pqc_malloc(8) == NULL);
    CHECK(pqc_test_alloc_attempts() == 1);
    CHECK(pqc_test_live_blocks() == start);

    pqc_test_fail_alloc_on(0);
    zeros = pqc_calloc(4, sizeof(*zeros));
    CHECK(zeros != NULL);
    CHECK(pqc_test_alloc_attempts() == 1);
    CHECK(pqc_test_live_blocks() == start + 1);
    if (zeros != NULL) {
        for (i = 0; i < 4; i++) {
            CHECK(zeros[i] == 0);
        }
    }
    pqc_free(zeros);

    pqc_test_fail_alloc_on(1);
    CHECK(pqc_calloc(4, sizeof(*zeros)) == NULL);
    CHECK(pqc_test_alloc_attempts() == 1);
    CHECK(pqc_test_live_blocks() == start);

    pqc_test_fail_alloc_on(0);
    bytes = pqc_malloc(8);
    CHECK(bytes != NULL);
    CHECK(pqc_test_alloc_attempts() == 1);
    CHECK(pqc_test_live_blocks() == start + 1);
    if (bytes == NULL) {
        return;
    }
    for (i = 0; i < 8; i++) {
        bytes[i] = (unsigned char)(i + 10);
    }

    pqc_test_fail_alloc_on(1);
    grown = pqc_realloc(bytes, 32);
    CHECK(grown == NULL);
    CHECK(pqc_test_alloc_attempts() == 1);
    CHECK(pqc_test_live_blocks() == start + 1);
    for (i = 0; i < 8; i++) {
        CHECK(bytes[i] == (unsigned char)(i + 10));
    }

    pqc_test_fail_alloc_on(0);
    grown = pqc_realloc(bytes, 32);
    CHECK(grown != NULL);
    CHECK(pqc_test_alloc_attempts() == 1);
    CHECK(pqc_test_live_blocks() == start + 1);
    if (grown != NULL) {
        bytes = grown;
        for (i = 0; i < 8; i++) {
            CHECK(bytes[i] == (unsigned char)(i + 10));
        }
    }

    CHECK(pqc_realloc(bytes, 0) == NULL);
    CHECK(pqc_test_alloc_attempts() == 1);
    CHECK(pqc_test_live_blocks() == start + 1);
    pqc_free(bytes);
    pqc_free(NULL);
    CHECK(pqc_test_live_blocks() == start);

    pqc_test_fail_alloc_on(0);
    bytes = pqc_realloc(NULL, 12);
    CHECK(bytes != NULL);
    CHECK(pqc_test_alloc_attempts() == 1);
    CHECK(pqc_test_live_blocks() == start + 1);
    pqc_free(bytes);

    pqc_test_fail_alloc_on(1);
    CHECK(pqc_realloc(NULL, 12) == NULL);
    CHECK(pqc_test_alloc_attempts() == 1);
    CHECK(pqc_test_live_blocks() == start);
    pqc_test_fail_alloc_on(0);
}

int test_common(void)
{
    failures = 0;
    test_statuses_and_diagnostics();
    test_checked_arithmetic();
    test_allocation_accounting();

    if (failures == 0) {
        printf("test_common: PASS\n");
    }
    return failures;
}
