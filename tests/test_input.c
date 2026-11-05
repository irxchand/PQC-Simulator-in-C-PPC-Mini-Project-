#include "input.h"

#include <stdio.h>
#include <string.h>

static int failures = 0;

#define CHECK(condition) do { \
    if (!(condition)) { \
        printf("FAIL test_input:%d: %s\n", __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static bool write_file(const char *path, const unsigned char *data,
                       size_t length)
{
    FILE *file = fopen(path, "wb");
    bool ok;

    if (file == NULL) {
        return false;
    }
    ok = fwrite(data, 1, length, file) == length;
    if (fclose(file) != 0) {
        ok = false;
    }
    return ok;
}

static void test_parsers(void)
{
    Diagnostic d = {0};
    size_t count = 0;
    Gate gate = {0};
    bool present = true;

    CHECK(input_parse_count(" +4 ", 4, 30, &count, &d) == PQC_OK);
    CHECK(count == 4);
    CHECK(input_parse_count("0", 1, 30, &count, &d) == PQC_E_RANGE);
    CHECK(input_parse_count("31", 2, 30, &count, &d) == PQC_E_LIMIT);
    CHECK(input_parse_count("4 junk", 6, 30, &count, &d) == PQC_E_FORMAT);
    CHECK(input_parse_count("1.0", 3, 30, &count, &d) == PQC_E_FORMAT);

    CHECK(input_parse_gate(" \t ", 3, 4, &gate, &present, &d) == PQC_OK);
    CHECK(!present);
    CHECK(input_parse_gate("RX 2 -1.25e+2", 14, 4,
                           &gate, &present, &d) == PQC_OK);
    CHECK(present && gate.kind == GATE_RX && gate.target == 2 &&
          gate.control == -1 && gate.angle == -125.0);
    CHECK(input_parse_gate("CX 3 1", 6, 4,
                           &gate, &present, &d) == PQC_OK);
    CHECK(gate.kind == GATE_CX && gate.control == 3 && gate.target == 1);
    CHECK(input_parse_gate("cx 0 1", 6, 4,
                           &gate, &present, &d) == PQC_E_FORMAT);
    CHECK(input_parse_gate("X 0 extra", 9, 4,
                           &gate, &present, &d) == PQC_E_FORMAT);
    CHECK(input_parse_gate("CX 1 1", 6, 4,
                           &gate, &present, &d) == PQC_E_RANGE);
    CHECK(input_parse_gate("RY 0 nan", 8, 4,
                           &gate, &present, &d) == PQC_E_RANGE);
    CHECK(input_parse_gate("RY 0 1e9999", 11, 4,
                           &gate, &present, &d) == PQC_E_RANGE);
    CHECK(input_parse_gate("RY 0 0x1p2", 11, 4,
                           &gate, &present, &d) == PQC_E_FORMAT);
}

static void test_valid_circuits(void)
{
    static const unsigned char fixture[] =
        "4\r\n"
        "RY 0 0.3\r\n"
        "RY 1 -0.2\r\n"
        "RY 2 0.5\r\n"
        "RY 3 0.7\r\n"
        "CX 0 1\r\n"
        "CX 0 3\r\n"
        "CX 2 3\r\n"
        "RZ 3 0.4";
    Circuit circuit = {0};
    Diagnostic d = {0};

    CHECK(input_load_circuit("circuit.txt", &circuit, &d) == PQC_OK);
    CHECK(circuit.logical_qubits == 2 && circuit.count == 4);
    circuit_destroy(&circuit);

    CHECK(write_file("build/test/pqc4-valid.circuit", fixture,
                     sizeof(fixture) - 1));
    CHECK(input_load_circuit("build/test/pqc4-valid.circuit",
                             &circuit, &d) == PQC_OK);
    CHECK(circuit.logical_qubits == 4 && circuit.count == 8);
    CHECK(circuit.operations[4].gate.kind == GATE_CX);
    CHECK(circuit.operations[4].gate.control == 0);
    CHECK(circuit.operations[4].gate.target == 1);
    CHECK(circuit.operations[4].source_line == 6);
    CHECK(circuit.operations[5].source_line == 7);
    CHECK(circuit.operations[6].source_line == 8);
    circuit_destroy(&circuit);
}

static void test_atomic_invalid_files(void)
{
    static const unsigned char late_bad[] =
        "2\nX 0\nH 1\nBOGUS 0\n";
    static const unsigned char with_bom[] = {0xef, 0xbb, 0xbf, '2', '\n'};
    static const unsigned char with_nul[] = {'2', '\n', 'X', 0, '0', '\n'};
    static const unsigned char bare_cr[] = {'2', '\r', 'X', ' ', '0'};
    Circuit circuit = {0};
    Diagnostic d = {0};

    CHECK(write_file("build/test/late-bad.circuit", late_bad,
                     sizeof(late_bad) - 1));
    CHECK(input_load_circuit("build/test/late-bad.circuit",
                             &circuit, &d) == PQC_E_FORMAT);
    CHECK(circuit.logical_qubits == 0 && circuit.operations == NULL);
    CHECK(d.line == 4);

    CHECK(write_file("build/test/bom.circuit", with_bom,
                     sizeof(with_bom)));
    CHECK(input_load_circuit("build/test/bom.circuit",
                             &circuit, &d) == PQC_E_FORMAT);
    CHECK(circuit.logical_qubits == 0);

    CHECK(write_file("build/test/nul.circuit", with_nul,
                     sizeof(with_nul)));
    CHECK(input_load_circuit("build/test/nul.circuit",
                             &circuit, &d) == PQC_E_FORMAT);
    CHECK(circuit.logical_qubits == 0);

    CHECK(write_file("build/test/bare-cr.circuit", bare_cr,
                     sizeof(bare_cr)));
    CHECK(input_load_circuit("build/test/bare-cr.circuit",
                             &circuit, &d) == PQC_E_FORMAT);
    CHECK(circuit.logical_qubits == 0);
}

static void test_topology_and_duplicates(void)
{
    static const unsigned char topology[] =
        "4\n0 1\n1 0\n1 2\n2 3\n";
    Graph *graph = NULL;
    Diagnostic d = {0};
    FILE *warnings;
    char text[256] = {0};
    size_t length;

    CHECK(write_file("build/test/duplicate.graph", topology,
                     sizeof(topology) - 1));
    warnings = tmpfile();
    CHECK(warnings != NULL);
    if (warnings == NULL) {
        return;
    }
    CHECK(input_load_topology("build/test/duplicate.graph", &graph,
                              warnings, &d) == PQC_OK);
    CHECK(graph_vertex_count(graph) == 4);
    CHECK(graph_edge_count(graph) == 3);
    rewind(warnings);
    length = fread(text, 1, sizeof(text) - 1, warnings);
    text[length] = '\0';
    CHECK(strstr(text, "W_DUPLICATE_EDGE") != NULL);
    CHECK(strstr(text, "line=3") != NULL);
    CHECK(fclose(warnings) == 0);
    graph_destroy(&graph);
}

static void test_loader_failures(void)
{
    static const unsigned char circuit_text[] = "2\nX 0\n";
    Circuit circuit = {0};
    Diagnostic d = {0};
    size_t start = pqc_test_live_blocks();

    CHECK(input_load_circuit("build/test/does-not-exist.circuit",
                             &circuit, &d) == PQC_E_IO);
    CHECK(circuit.logical_qubits == 0);

    CHECK(write_file("build/test/read-fail.circuit", circuit_text,
                     sizeof(circuit_text) - 1));
    input_test_fail_read_on(1);
    CHECK(input_load_circuit("build/test/read-fail.circuit",
                             &circuit, &d) == PQC_E_IO);
    CHECK(circuit.logical_qubits == 0);
    input_test_fail_read_on(0);
    CHECK(pqc_test_live_blocks() == start);

    pqc_test_fail_alloc_on(1);
    CHECK(input_load_circuit("build/test/read-fail.circuit",
                             &circuit, &d) == PQC_E_ALLOC);
    CHECK(circuit.logical_qubits == 0);
    pqc_test_fail_alloc_on(2);
    CHECK(input_load_circuit("build/test/read-fail.circuit",
                             &circuit, &d) == PQC_E_ALLOC);
    CHECK(circuit.logical_qubits == 0);
    pqc_test_fail_alloc_on(0);
    CHECK(pqc_test_live_blocks() == start);
}

static void test_terminal_reader(void)
{
    FILE *file = tmpfile();
    Diagnostic d = {0};
    char output[PQC_MAX_LINE_BYTES + 1u];
    size_t length = 0;
    bool eof = false;
    size_t i;

    CHECK(file != NULL);
    if (file == NULL) {
        return;
    }
    for (i = 0; i < PQC_MAX_LINE_BYTES + 5u; i++) {
        fputc('A', file);
    }
    fputs("\nNEXT\n", file);
    rewind(file);
    input_test_fail_read_on(0);
    CHECK(input_terminal_line(file, output, &length, &eof, &d) ==
          PQC_E_FORMAT);
    CHECK(input_terminal_line(file, output, &length, &eof, &d) == PQC_OK);
    CHECK(length == 4 && strcmp(output, "NEXT") == 0 && !eof);
    CHECK(input_terminal_line(file, output, &length, &eof, &d) == PQC_OK);
    CHECK(eof && length == 0);
    CHECK(fclose(file) == 0);
}

int test_input(void)
{
    failures = 0;
    pqc_test_fail_alloc_on(0);
    input_test_fail_read_on(0);
    test_parsers();
    test_valid_circuits();
    test_atomic_invalid_files();
    test_topology_and_duplicates();
    test_loader_failures();
    test_terminal_reader();
    pqc_test_fail_alloc_on(0);
    input_test_fail_read_on(0);

    CHECK(pqc_test_live_blocks() == 0);
    if (failures == 0) {
        printf("test_input: PASS\n");
    }
    return failures;
}
