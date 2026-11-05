#include "qml.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static int failures = 0;

#define CHECK(condition) do { \
    if (!(condition)) { \
        printf("FAIL test_qml:%d: %s\n", __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static void test_controls_and_training(void)
{
    static const double features[4][2] = {
        {0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}
    };
    static const double expected[4] = {0.0, 1.0, 1.0, 0.0};
    double initial[4] = {2.639, 1.239, 2.461, 2.508};
    double initial_copy[4];
    double zero[4] = {0.0, 0.0, 0.0, 0.0};
    QmlTrainResult result;
    Diagnostic d = {0};
    size_t k;

    memcpy(initial_copy, initial, sizeof(initial));
    for (k = 0; k < 4; k++) {
        double encoding;
        double full;
        double zero_full;
        CHECK(qml_encoding_only(features[k], &encoding, &d) == PQC_OK);
        CHECK(fabs(encoding - expected[k]) <= 1e-12);
        CHECK(qml_forward(features[k], initial, 2, &full, &d) == PQC_OK);
        CHECK(isfinite(full));
        CHECK(qml_forward(features[k], zero, 2, &zero_full, &d) == PQC_OK);
        CHECK(isfinite(zero_full));
    }
    CHECK(qml_train_xor(initial, NULL, &result, &d) == PQC_OK);
    CHECK(memcmp(initial, initial_copy, sizeof(initial)) == 0);
    CHECK(result.final_correct == 4);
    CHECK(result.final_mse < result.initial_mse);
    CHECK(isfinite(result.epoch200_preupdate_mse));
}

static void test_failure_propagation(void)
{
    double features[2] = {0.0, 1.0};
    double theta[4] = {0.1, 0.2, 0.3, 0.4};
    double probability = -1.0;
    QmlTrainResult result;
    Diagnostic d = {0};
    size_t start = pqc_test_live_blocks();

    pqc_test_fail_alloc_on(1);
    CHECK(qml_forward(features, theta, 2, &probability, &d) == PQC_E_ALLOC);
    CHECK(probability == -1.0);
    CHECK(pqc_test_live_blocks() == start);
    memset(&result, 0x5a, sizeof(result));
    pqc_test_fail_alloc_on(1);
    CHECK(qml_train_xor(theta, NULL, &result, &d) == PQC_E_ALLOC);
    CHECK(result.initial_mse == 0.0 && result.final_mse == 0.0);
    CHECK(pqc_test_live_blocks() == start);
    pqc_test_fail_alloc_on(0);
}

int test_qml(void)
{
    failures = 0;
    pqc_test_fail_alloc_on(0);
    test_controls_and_training();
    test_failure_propagation();
    pqc_test_fail_alloc_on(0);
    CHECK(pqc_test_live_blocks() == 0);
    if (failures == 0) {
        printf("test_qml: PASS\n");
    }
    return failures;
}
