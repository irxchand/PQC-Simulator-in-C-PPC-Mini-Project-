#include "circuit.h"

#include <math.h>
#include <stdio.h>

static int failures = 0;

#define CHECK(condition) do { \
    if (!(condition)) { \
        printf("FAIL test_circuit:%d: %s\n", __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static Gate make_x(int target)
{
    Gate gate = {
        .kind = GATE_X,
        .target = target,
        .control = -1,
        .angle = 0.0
    };
    return gate;
}

static void test_gate_validation(void)
{
    Diagnostic d = {0};
    Gate gate = make_x(1);

    CHECK(gate_validate(&gate, 2, &d) == PQC_OK);
    gate.angle = 1.0;
    CHECK(gate_validate(&gate, 2, &d) == PQC_E_FORMAT);

    gate = (Gate){
        .kind = GATE_RY,
        .target = 0,
        .control = -1,
        .angle = NAN
    };
    CHECK(gate_validate(&gate, 2, &d) == PQC_E_RANGE);

    gate = (Gate){
        .kind = GATE_CX,
        .target = 1,
        .control = 0,
        .angle = 0.0
    };
    CHECK(gate_validate(&gate, 2, &d) == PQC_OK);
    gate.control = 1;
    CHECK(gate_validate(&gate, 2, &d) == PQC_E_RANGE);
}

static void test_order_and_ownership(void)
{
    Circuit circuit = {0};
    Diagnostic d = {0};
    Gate x = make_x(0);
    Gate cx = {
        .kind = GATE_CX,
        .target = 2,
        .control = 0,
        .angle = 0.0
    };
    size_t start = pqc_test_live_blocks();

    CHECK(circuit_init(&circuit, 3, &d) == PQC_OK);
    CHECK(circuit_validate(&circuit, &d) == PQC_OK);
    CHECK(circuit.count == 0);
    CHECK(circuit.operations == NULL);

    CHECK(circuit_append(&circuit, x, 2, &d) == PQC_OK);
    CHECK(circuit_append(&circuit, cx, 5, &d) == PQC_OK);
    CHECK(circuit.count == 2);
    CHECK(circuit.capacity == 16);
    CHECK(circuit.operations[0].gate.kind == GATE_X);
    CHECK(circuit.operations[0].source_line == 2);
    CHECK(circuit.operations[1].gate.kind == GATE_CX);
    CHECK(circuit.operations[1].gate.control == 0);
    CHECK(circuit.operations[1].gate.target == 2);
    CHECK(circuit.operations[1].source_line == 5);
    CHECK(circuit_validate(&circuit, &d) == PQC_OK);

    circuit_destroy(&circuit);
    CHECK(circuit.logical_qubits == 0);
    CHECK(circuit.operations == NULL);
    CHECK(pqc_test_live_blocks() == start);
    circuit_destroy(&circuit);
    CHECK(pqc_test_live_blocks() == start);
}

static void test_failed_initial_allocation(void)
{
    Circuit circuit = {0};
    Diagnostic d = {0};
    Gate x = make_x(0);
    size_t start = pqc_test_live_blocks();

    CHECK(circuit_init(&circuit, 1, &d) == PQC_OK);
    pqc_test_fail_alloc_on(1);
    CHECK(circuit_append(&circuit, x, 2, &d) == PQC_E_ALLOC);
    CHECK(circuit.count == 0);
    CHECK(circuit.capacity == 0);
    CHECK(circuit.operations == NULL);
    CHECK(pqc_test_live_blocks() == start);
    pqc_test_fail_alloc_on(0);
    circuit_destroy(&circuit);
}

static void test_failed_growth_keeps_operations(void)
{
    Circuit circuit = {0};
    Diagnostic d = {0};
    Gate x = make_x(0);
    Operation *old_operations;
    size_t start = pqc_test_live_blocks();
    size_t i;

    CHECK(circuit_init(&circuit, 1, &d) == PQC_OK);
    pqc_test_fail_alloc_on(0);
    for (i = 0; i < 16; i++) {
        CHECK(circuit_append(&circuit, x, i + 2, &d) == PQC_OK);
    }
    old_operations = circuit.operations;
    pqc_test_fail_alloc_on(1);
    CHECK(circuit_append(&circuit, x, 18, &d) == PQC_E_ALLOC);
    CHECK(circuit.count == 16);
    CHECK(circuit.capacity == 16);
    CHECK(circuit.operations == old_operations);
    for (i = 0; i < 16; i++) {
        CHECK(circuit.operations[i].source_line == i + 2);
        CHECK(circuit.operations[i].gate.kind == GATE_X);
    }

    pqc_test_fail_alloc_on(0);
    circuit_destroy(&circuit);
    CHECK(pqc_test_live_blocks() == start);
}

static void test_invalid_init_and_append(void)
{
    Circuit circuit = {0};
    Diagnostic d = {0};
    Gate bad = make_x(2);

    CHECK(circuit_init(&circuit, 0, &d) == PQC_E_RANGE);
    CHECK(circuit_init(&circuit, PQC_MAX_LOGICAL_QUBITS + 1, &d) ==
          PQC_E_LIMIT);
    CHECK(circuit_init(&circuit, 2, &d) == PQC_OK);
    CHECK(circuit_init(&circuit, 2, &d) == PQC_E_INTERNAL);
    CHECK(circuit_append(&circuit, bad, 3, &d) == PQC_E_RANGE);
    CHECK(circuit.count == 0);
    CHECK(circuit_append(&circuit, make_x(0), 0, &d) == PQC_E_INTERNAL);
    CHECK(circuit.count == 0);
    circuit_destroy(&circuit);
}

int test_circuit(void)
{
    failures = 0;
    pqc_test_fail_alloc_on(0);
    test_gate_validation();
    test_order_and_ownership();
    test_failed_initial_allocation();
    test_failed_growth_keeps_operations();
    test_invalid_init_and_append();
    pqc_test_fail_alloc_on(0);

    if (failures == 0) {
        printf("test_circuit: PASS\n");
    }
    return failures;
}
