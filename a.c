#include "analysis.h"
#include "circuit.h"
#include "common.h"
#include "graph.h"
#include "input.h"
#include "qml.h"
#include "sim.h"

#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

static int usage_error(const char *message);

static void print_diagnostic(const Diagnostic *diagnostic)
{
    (void)pqc_diag_print(stderr, diagnostic);
}

static bool is_done_line(const char *line, size_t length)
{
    size_t start = 0;
    size_t end = length;

    while (start < end && (line[start] == ' ' || line[start] == '\t')) {
        start++;
    }
    while (end > start && (line[end - 1] == ' ' || line[end - 1] == '\t')) {
        end--;
    }
    return (end - start == 4 && strncmp(line + start, "DONE", 4) == 0) ||
           (end - start == 4 && strncmp(line + start, "done", 4) == 0);
}

static PqcStatus run_interactive_action(void)
{
    char line[PQC_MAX_LINE_BYTES + 1u];
    size_t length;
    size_t qubits;
    size_t applied = 0;
    size_t input_errors = 0;
    bool eof;
    SimState state = {0};
    Diagnostic diagnostic = {0};
    PqcStatus status = PQC_OK;

    printf("Enter number of qubits: ");
    status = input_terminal_line(stdin, line, &length, &eof, &diagnostic);
    if (status != PQC_OK) {
        print_diagnostic(&diagnostic);
        return status;
    }
    if (eof) {
        return PQC_OK;
    }
    status = input_parse_count(line, length, PQC_MAX_INTERACTIVE_QUBITS,
                               &qubits, &diagnostic);
    if (status != PQC_OK) {
        print_diagnostic(&diagnostic);
        return status;
    }
    status = sim_create(qubits, &state, &diagnostic);
    if (status != PQC_OK) {
        print_diagnostic(&diagnostic);
        return status;
    }
    printf("Initialized %u qubits.\n", (unsigned)qubits);
    status = sim_write_probabilities(stdout, &state, &diagnostic);
    if (status != PQC_OK) {
        print_diagnostic(&diagnostic);
        sim_destroy(&state);
        return status;
    }
    printf("Enter gate (e.g. RY 0 1.57). DONE to exit.\n");

    while (true) {
        Gate gate;
        bool present;

        printf("> ");
        status = input_terminal_line(stdin, line, &length, &eof, &diagnostic);
        if (status != PQC_OK) {
            print_diagnostic(&diagnostic);
            input_errors++;
            if (status == PQC_E_IO) {
                break;
            }
            continue;
        }
        if (eof || is_done_line(line, length)) {
            break;
        }
        status = input_parse_gate(line, length, qubits, &gate, &present,
                                  &diagnostic);
        if (status != PQC_OK) {
            print_diagnostic(&diagnostic);
            input_errors++;
            continue;
        }
        if (!present) {
            continue;
        }
        status = sim_apply_gate(&state, &gate, &diagnostic);
        if (status != PQC_OK) {
            print_diagnostic(&diagnostic);
            input_errors++;
            if (!state.usable) {
                break;
            }
            continue;
        }
        status = sim_write_gate(stdout, &gate, &diagnostic);
        if (status == PQC_OK) {
            status = sim_write_probabilities(stdout, &state, &diagnostic);
        }
        if (status != PQC_OK) {
            print_diagnostic(&diagnostic);
            break;
        }
        applied++;
    }

    if (input_errors == 0) {
        printf("Gates applied: %u\n", (unsigned)applied);
    } else {
        printf("Gates applied: %u; input errors: %u\n",
               (unsigned)applied, (unsigned)input_errors);
    }
    sim_destroy(&state);
    return status == PQC_E_IO ? status : PQC_OK;
}

static PqcStatus run_file_action(void)
{
    char filename[PQC_MAX_LINE_BYTES + 1u];
    size_t length;
    bool eof;
    Circuit circuit = {0};
    SimState state = {0};
    Diagnostic diagnostic = {0};
    PqcStatus status;

    printf("Enter filename: ");
    status = input_terminal_line(stdin, filename, &length, &eof, &diagnostic);
    if (status != PQC_OK) {
        print_diagnostic(&diagnostic);
        return status;
    }
    if (eof) {
        return PQC_OK;
    }
    filename[length] = '\0';
    status = input_load_circuit(filename, &circuit, &diagnostic);
    if (status != PQC_OK) {
        print_diagnostic(&diagnostic);
        return status;
    }
    status = sim_execute(&circuit, stdout, &state, &diagnostic);
    if (status == PQC_OK) {
        printf("Gates applied: %u\n", (unsigned)circuit.count);
    } else {
        if (diagnostic.file == NULL) {
            diagnostic.file = filename;
        }
        print_diagnostic(&diagnostic);
    }
    sim_destroy(&state);
    circuit_destroy(&circuit);
    return status;
}

static PqcStatus run_train_action(void)
{
    double initial[4];
    QmlTrainResult result;
    Diagnostic diagnostic = {0};
    PqcStatus status;

    qml_legacy_initial_theta(initial);
    status = qml_train_xor(initial, stdout, &result, &diagnostic);
    if (status != PQC_OK) {
        print_diagnostic(&diagnostic);
    }
    return status;
}

static int run_simulate_command(const char *circuit_file)
{
    Circuit circuit = {0};
    SimState state = {0};
    Diagnostic diagnostic = {0};
    PqcStatus status;

    status = input_load_circuit(circuit_file, &circuit, &diagnostic);
    if (status == PQC_OK) {
        status = sim_execute(&circuit, stdout, &state, &diagnostic);
    }
    if (status == PQC_OK) {
        printf("SIMULATION status=COMPLETE logical_qubits=%" PRIuMAX
               " gates=%" PRIuMAX "\n",
               (uintmax_t)circuit.logical_qubits,
               (uintmax_t)circuit.count);
    } else {
        if (diagnostic.file == NULL) {
            diagnostic.file = circuit_file;
        }
        print_diagnostic(&diagnostic);
    }
    sim_destroy(&state);
    circuit_destroy(&circuit);
    return pqc_exit_code(status);
}

static PqcStatus write_qml_probabilities(const char *mode,
                                         const double probability[4],
                                         Diagnostic *d)
{
    static const int features[4][2] = {
        {0, 0}, {0, 1}, {1, 0}, {1, 1}
    };
    static const int expected[4] = {0, 1, 1, 0};
    size_t i;

    if (printf("QML mode=%s examples=4 readout=ODD_PARITY\n", mode) < 0) {
        pqc_diag_set(d, PQC_E_IO, NULL, 0, "could not write QML output");
        return PQC_E_IO;
    }
    for (i = 0; i < 4; i++) {
        if (printf("PREDICTION input=%d%d probability=%.10f class=%d expected=%d\n",
                   features[i][0], features[i][1], probability[i],
                   probability[i] >= 0.5 ? 1 : 0, expected[i]) < 0) {
            pqc_diag_set(d, PQC_E_IO, NULL, 0,
                         "could not write QML prediction");
            return PQC_E_IO;
        }
    }
    return PQC_OK;
}

static int run_qml_command(const char *mode)
{
    static const double features[4][2] = {
        {0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}
    };
    const double initial[4] = {0.2, 0.4, 0.6, 0.8};
    const double zero[4] = {0.0, 0.0, 0.0, 0.0};
    double probability[4] = {0.0, 0.0, 0.0, 0.0};
    QmlTrainResult training = {0};
    Diagnostic diagnostic = {0};
    PqcStatus status = PQC_OK;
    size_t i;

    if (strcmp(mode, "train") == 0) {
        status = qml_train_xor(initial, stdout, &training, &diagnostic);
        if (status == PQC_OK) {
            status = write_qml_probabilities("TRAINED_FULL_PQC",
                                             training.final_probability,
                                             &diagnostic);
        }
    } else if (strcmp(mode, "trained") == 0) {
        status = qml_train_xor(initial, NULL, &training, &diagnostic);
        if (status == PQC_OK) {
            status = write_qml_probabilities("TRAINED_FULL_PQC",
                                             training.final_probability,
                                             &diagnostic);
        }
    } else {
        for (i = 0; status == PQC_OK && i < 4; i++) {
            if (strcmp(mode, "encoding-only") == 0) {
                status = qml_encoding_only(features[i], &probability[i],
                                           &diagnostic);
            } else if (strcmp(mode, "initialized") == 0) {
                status = qml_forward(features[i], initial, 2,
                                     &probability[i], &diagnostic);
            } else if (strcmp(mode, "zero") == 0) {
                status = qml_forward(features[i], zero, 2,
                                     &probability[i], &diagnostic);
            } else {
                return usage_error("unknown QML mode");
            }
        }
        if (status == PQC_OK) {
            const char *label = strcmp(mode, "encoding-only") == 0
                              ? "ENCODING_ONLY"
                              : strcmp(mode, "initialized") == 0
                              ? "INITIALIZED_FULL_PQC"
                              : "ZERO_PARAMETER_FULL_PQC";
            status = write_qml_probabilities(label, probability,
                                             &diagnostic);
        }
    }
    if (status != PQC_OK) {
        print_diagnostic(&diagnostic);
    }
    return pqc_exit_code(status);
}

static int run_menu(void)
{
    char line[PQC_MAX_LINE_BYTES + 1u];
    size_t length;
    bool eof;

    while (true) {
        size_t choice;
        Diagnostic diagnostic = {0};
        PqcStatus status;

        printf("1. Interactive\n");
        printf("2. File\n");
        printf("3. Train\n");
        printf("4. Exit\n");
        printf("Choice: ");
        status = input_terminal_line(stdin, line, &length, &eof, &diagnostic);
        if (status != PQC_OK) {
            print_diagnostic(&diagnostic);
            if (status == PQC_E_IO) {
                return pqc_exit_code(status);
            }
            continue;
        }
        if (eof) {
            return 0;
        }
        status = input_parse_count(line, length, 4, &choice, &diagnostic);
        if (status != PQC_OK) {
            fprintf(stderr, "Invalid Choice\n");
            continue;
        }
        if (choice == 1) {
            (void)run_interactive_action();
        } else if (choice == 2) {
            (void)run_file_action();
        } else if (choice == 3) {
            (void)run_train_action();
        } else {
            return 0;
        }
    }
}

static PqcStatus write_quoted(FILE *stream, const char *text)
{
    const unsigned char *current = (const unsigned char *)text;

    if (fputc('"', stream) == EOF) {
        return PQC_E_IO;
    }
    while (*current != '\0') {
        if (*current == '\\' || *current == '"') {
            if (fputc('\\', stream) == EOF || fputc(*current, stream) == EOF) {
                return PQC_E_IO;
            }
        } else if (*current < 32 || *current == 127) {
            if (fprintf(stream, "\\x%02X", (unsigned)*current) < 0) {
                return PQC_E_IO;
            }
        } else if (fputc(*current, stream) == EOF) {
            return PQC_E_IO;
        }
        current++;
    }
    return fputc('"', stream) == EOF ? PQC_E_IO : PQC_OK;
}

static const char *class_name(CxClass classification)
{
    if (classification == CX_DIRECT) {
        return "DIRECT";
    }
    if (classification == CX_INDIRECT) {
        return "INDIRECT";
    }
    return "UNREACHABLE";
}

static PqcStatus write_analysis(FILE *stream, const char *circuit_file,
                                const char *topology_file,
                                const Circuit *circuit, const Graph *graph,
                                const Analysis *analysis, Diagnostic *d)
{
    size_t i;

    if (fprintf(stream, "ANALYSIS circuit_file=") < 0 ||
        write_quoted(stream, circuit_file) != PQC_OK ||
        fprintf(stream, " topology_file=") < 0 ||
        write_quoted(stream, topology_file) != PQC_OK ||
        fprintf(stream,
                " logical_qubits=%" PRIuMAX
                " physical_vertices=%" PRIuMAX
                " unique_edges=%" PRIuMAX
                " mapping=IDENTITY model=RESTORE-3CX-v1\n",
                (uintmax_t)circuit->logical_qubits,
                (uintmax_t)graph_vertex_count(graph),
                (uintmax_t)graph_edge_count(graph)) < 0 ||
        fprintf(stream,
                "NOTE Idealized RESTORE-3CX-v1 gate-count estimate; undirected equal-cost edges, identity mapping, mapping restored after every interaction, three CX equivalents per SWAP. Not optimal routing, hardware runtime, or fidelity.\n") < 0) {
        pqc_diag_set(d, PQC_E_IO, NULL, 0,
                     "could not write analysis header");
        return PQC_E_IO;
    }

    for (i = 0; i < analysis->count; i++) {
        const CxAnalysis *row = &analysis->rows[i];
        bool adjacent = row->classification == CX_DIRECT;

        if (fprintf(stream,
                    "CX cx_index=%" PRIuMAX
                    " gate_index=%" PRIuMAX
                    " source_line=%" PRIuMAX
                    " control=%" PRIuMAX
                    " target=%" PRIuMAX
                    " physical_control=%" PRIuMAX
                    " physical_target=%" PRIuMAX
                    " classification=%s adjacent=%s path=",
                    (uintmax_t)row->cx_index, (uintmax_t)row->gate_index,
                    (uintmax_t)row->source_line, (uintmax_t)row->control,
                    (uintmax_t)row->target,
                    (uintmax_t)row->physical_control,
                    (uintmax_t)row->physical_target,
                    class_name(row->classification),
                    adjacent ? "YES" : "NO") < 0) {
            goto write_error;
        }
        if (!row->path.reachable) {
            if (fprintf(stream,
                        "NA distance=NA estimated_swaps=NA extra_cx_equivalents=NA total_cx_equivalents=NA\n") < 0) {
                goto write_error;
            }
        } else {
            size_t path_index;
            for (path_index = 0; path_index < row->path.length; path_index++) {
                if (fprintf(stream, "%s%" PRIuMAX,
                            path_index == 0 ? "" : "->",
                            (uintmax_t)row->path.vertex[path_index]) < 0) {
                    goto write_error;
                }
            }
            if (fprintf(stream,
                        " distance=%" PRIuMAX
                        " estimated_swaps=%" PRIu64
                        " extra_cx_equivalents=%" PRIu64
                        " total_cx_equivalents=%" PRIu64 "\n",
                        (uintmax_t)(row->path.length - 1),
                        row->cost.value.swaps, row->cost.value.extra_cx,
                        row->cost.value.total_cx) < 0) {
                goto write_error;
            }
        }
    }

    if (fprintf(stream,
                "SUMMARY cx_total=%" PRIuMAX
                " cx_direct=%" PRIuMAX
                " cx_indirect=%" PRIuMAX
                " cx_unreachable=%" PRIuMAX,
                (uintmax_t)analysis->count,
                (uintmax_t)analysis->direct_count,
                (uintmax_t)analysis->indirect_count,
                (uintmax_t)analysis->unreachable_count) < 0) {
        goto write_error;
    }
    if (analysis->full_cost.available) {
        if (fprintf(stream,
                    " cost_status=COMPLETE estimated_swaps_total=%" PRIu64
                    " extra_cx_equivalents_total=%" PRIu64
                    " total_cx_equivalents=%" PRIu64 "\n",
                    analysis->full_cost.value.swaps,
                    analysis->full_cost.value.extra_cx,
                    analysis->full_cost.value.total_cx) < 0) {
            goto write_error;
        }
    } else if (fprintf(stream,
                       " cost_status=INCOMPLETE_UNREACHABLE estimated_swaps_total=NA extra_cx_equivalents_total=NA total_cx_equivalents=NA\n") < 0) {
        goto write_error;
    }
    if (fflush(stream) != 0) {
        goto write_error;
    }
    return PQC_OK;

write_error:
    pqc_diag_set(d, PQC_E_IO, NULL, 0,
                 "could not write complete analysis report");
    return PQC_E_IO;
}

static int run_analysis_command(const char *circuit_file,
                                const char *topology_file)
{
    Circuit circuit = {0};
    Graph *graph = NULL;
    Analysis analysis = {0};
    Diagnostic diagnostic = {0};
    PqcStatus status;

    status = input_load_circuit(circuit_file, &circuit, &diagnostic);
    if (status == PQC_OK) {
        status = input_load_topology(topology_file, &graph, stderr,
                                     &diagnostic);
    }
    if (status == PQC_OK) {
        status = analysis_build(&circuit, graph, &analysis, &diagnostic);
    }
    if (status == PQC_OK) {
        status = write_analysis(stdout, circuit_file, topology_file,
                                &circuit, graph, &analysis, &diagnostic);
    }
    if (status != PQC_OK) {
        print_diagnostic(&diagnostic);
    }
    analysis_destroy(&analysis);
    graph_destroy(&graph);
    circuit_destroy(&circuit);
    return pqc_exit_code(status);
}

static int usage_error(const char *message)
{
    Diagnostic diagnostic = {0};
    pqc_diag_set(&diagnostic, PQC_E_USAGE, NULL, 0, "%s", message);
    print_diagnostic(&diagnostic);
    fprintf(stderr,
            "Usage: qmlc [--help | --simulate CIRCUIT | "
            "--analyze CIRCUIT --topology GRAPH | --qml MODE]\n");
    return pqc_exit_code(PQC_E_USAGE);
}

static int write_help(void)
{
    if (printf(
        "PQC-GRAPH-C: ideal circuit simulator and connectivity analyzer\n"
        "Usage:\n"
        "  qmlc\n"
        "  qmlc --help\n"
        "  qmlc --simulate CIRCUIT\n"
        "  qmlc --analyze CIRCUIT --topology GRAPH\n"
        "  qmlc --qml encoding-only|initialized|trained|zero|train\n"
        "The two analysis options may appear in either order.\n"
        "Circuit bounds: 1-30 logical qubits, 10000 gates, 255 bytes per line.\n"
        "Topology bounds: 1-64 vertices, 10000 edge rows.\n"
        "Ideal simulation supports up to 20 qubits (interactive: 10).\n"
        "Cost model: RESTORE-3CX-v1, an idealized gate-count estimate.\n") < 0 ||
        fflush(stdout) != 0) {
        return pqc_exit_code(PQC_E_IO);
    }
    return 0;
}

int main(int argc, char **argv)
{
    const char *circuit_file = NULL;
    const char *topology_file = NULL;
    int i;

#ifdef SIGPIPE
    (void)signal(SIGPIPE, SIG_IGN);
#endif
    if (argc == 1) {
        return run_menu();
    }
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        return write_help();
    }
    if (argc == 3 && strcmp(argv[1], "--simulate") == 0) {
        return run_simulate_command(argv[2]);
    }
    if (argc == 3 && strcmp(argv[1], "--qml") == 0) {
        return run_qml_command(argv[2]);
    }
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--analyze") == 0) {
            if (circuit_file != NULL || i + 1 >= argc) {
                return usage_error("--analyze must occur once with a value");
            }
            circuit_file = argv[++i];
        } else if (strcmp(argv[i], "--topology") == 0) {
            if (topology_file != NULL || i + 1 >= argc) {
                return usage_error("--topology must occur once with a value");
            }
            topology_file = argv[++i];
        } else {
            return usage_error("unknown or extra argument");
        }
    }
    if (circuit_file == NULL || topology_file == NULL) {
        return usage_error("both --analyze and --topology are required");
    }
    return run_analysis_command(circuit_file, topology_file);
}
