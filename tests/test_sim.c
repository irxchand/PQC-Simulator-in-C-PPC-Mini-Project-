#include "sim.h"

#include <math.h>
#include <stdio.h>

static int failures = 0;

#define CHECK(condition) do { \
    if (!(condition)) { \
        printf("FAIL test_sim:%d: %s\n", __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static bool close_to(double a, double b, double tolerance)
{
    return fabs(a - b) <= tolerance;
}

static PqcStatus apply(SimState *state, GateKind kind, int target,
                       int control, double angle)
{
    Gate gate = {kind, target, control, angle};
    Diagnostic d = {0};
    return sim_apply_gate(state, &gate, &d);
}

static void test_bell_and_phases(void)
{
    SimState state = {0};
    Diagnostic d = {0};
    double odd = -1.0;

    CHECK(sim_create(2, &state, &d) == PQC_OK);
    CHECK(apply(&state, GATE_H, 0, -1, 0.0) == PQC_OK);
    CHECK(apply(&state, GATE_CX, 1, 0, 0.0) == PQC_OK);
    CHECK(close_to(state.amplitude[0].r, 1.0 / sqrt(2.0), 1e-12));
    CHECK(close_to(state.amplitude[3].r, 1.0 / sqrt(2.0), 1e-12));
    CHECK(close_to(state.amplitude[1].r, 0.0, 1e-12));
    CHECK(sim_odd_probability(&state, &odd, &d) == PQC_OK);
    CHECK(close_to(odd, 0.0, 1e-12));
    CHECK(apply(&state, GATE_RZ, 1, -1, PQC_PI) == PQC_OK);
    CHECK(isfinite(state.amplitude[0].r) && isfinite(state.amplitude[3].i));
    sim_destroy(&state);
}

static void test_rejection_and_allocation(void)
{
    SimState state = {0};
    Diagnostic d = {0};
    Gate invalid = {GATE_X, 3, -1, 0.0};
    size_t start = pqc_test_live_blocks();

    CHECK(sim_create(0, &state, &d) == PQC_E_RANGE);
    CHECK(sim_create(21, &state, &d) == PQC_E_LIMIT);
    pqc_test_fail_alloc_on(1);
    CHECK(sim_create(2, &state, &d) == PQC_E_ALLOC);
    CHECK(state.amplitude == NULL && pqc_test_live_blocks() == start);
    pqc_test_fail_alloc_on(0);
    CHECK(sim_create(2, &state, &d) == PQC_OK);
    CHECK(sim_apply_gate(&state, &invalid, &d) == PQC_E_RANGE);
    CHECK(state.usable && state.amplitude[0].r == 1.0);
    sim_destroy(&state);
    CHECK(pqc_test_live_blocks() == start);
}

static void test_numeric_failures(void)
{
    SimState state = {0};
    Diagnostic d = {0};

    CHECK(sim_create(2, &state, &d) == PQC_OK);
    state.amplitude[0].r = NAN;
    CHECK(apply(&state, GATE_X, 0, -1, 0.0) == PQC_E_NUMERIC);
    CHECK(!state.usable);
    sim_destroy(&state);

    CHECK(sim_create(2, &state, &d) == PQC_OK);
    state.amplitude[0].r = sqrt(1.0 + 1e-6);
    CHECK(apply(&state, GATE_X, 0, -1, 0.0) == PQC_E_NUMERIC);
    CHECK(!state.usable);
    sim_destroy(&state);
}

static void test_dense_and_skewed_20_qubits(void)
{
    SimState state = {0};
    Diagnostic d = {0};
    double odd;
    size_t i;
    double small = ldexp(1.0, -27);
    double p = ldexp(1.0, -54);
    double tail;

    CHECK(sim_create(20, &state, &d) == PQC_OK);
    for (i = 0; i < state.size; i++) {
        state.amplitude[i].r = ldexp(1.0, -10);
    }
    CHECK(apply(&state, GATE_RX, 0, -1, 0.0) == PQC_OK);
    CHECK(sim_odd_probability(&state, &odd, &d) == PQC_OK);
    CHECK(close_to(odd, 0.5, 1e-10));
    sim_destroy(&state);

    CHECK(sim_create(20, &state, &d) == PQC_OK);
    tail = (double)(state.size - 2) * p;
    for (i = 0; i < state.size; i++) {
        state.amplitude[i].r = small;
    }
    state.amplitude[0].r = sqrt(0.6 * (1.0 - tail));
    state.amplitude[state.size - 1].r = sqrt(0.4 * (1.0 - tail));
    CHECK(apply(&state, GATE_RX, 0, -1, 0.0) == PQC_OK);
    CHECK(sim_odd_probability(&state, &odd, &d) == PQC_OK);
    CHECK(isfinite(odd));
    sim_destroy(&state);
}

static void test_generated_20_qubit_circuit(void)
{
    SimState state = {0};
    Diagnostic d = {0};
    double odd;
    int i;

    CHECK(sim_create(20, &state, &d) == PQC_OK);
    for (i = 0; i < 20; i++) {
        CHECK(apply(&state, GATE_RY, i, -1, 0.01 * (i + 1)) == PQC_OK);
    }
    for (i = 0; i < 19; i++) {
        CHECK(apply(&state, GATE_CX, i + 1, i, 0.0) == PQC_OK);
    }
    CHECK(sim_odd_probability(&state, &odd, &d) == PQC_OK);
    CHECK(isfinite(odd) && odd >= 0.0 && odd <= 1.0);
    sim_destroy(&state);
}

int test_sim(void)
{
    failures = 0;
    pqc_test_fail_alloc_on(0);
    test_bell_and_phases();
    test_rejection_and_allocation();
    test_numeric_failures();
    test_dense_and_skewed_20_qubits();
    test_generated_20_qubit_circuit();
    pqc_test_fail_alloc_on(0);
    CHECK(pqc_test_live_blocks() == 0);
    if (failures == 0) {
        printf("test_sim: PASS\n");
    }
    return failures;
}
