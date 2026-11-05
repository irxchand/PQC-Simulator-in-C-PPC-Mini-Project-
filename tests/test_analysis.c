#include "analysis.h"
#include "input.h"

#include <stdio.h>

static int failures = 0;

#define CHECK(condition) do { \
    if (!(condition)) { \
        printf("FAIL test_analysis:%d: %s\n", __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static Graph *make_graph(size_t vertices, const size_t edges[][2],
                         size_t edge_count)
{
    Graph *graph = NULL;
    Diagnostic d = {0};
    size_t i;

    CHECK(graph_create(vertices, &graph, &d) == PQC_OK);
    for (i = 0; graph != NULL && i < edge_count; i++) {
        bool inserted = false;
        CHECK(graph_add_edge(graph, edges[i][0], edges[i][1],
                             &inserted, &d) == PQC_OK);
        CHECK(inserted);
    }
    return graph;
}

static void append_cx(Circuit *circuit, int control, int target,
                      size_t line)
{
    Diagnostic d = {0};
    Gate gate = {
        .kind = GATE_CX,
        .target = target,
        .control = control,
        .angle = 0.0
    };

    CHECK(circuit_append(circuit, gate, line, &d) == PQC_OK);
}

static void test_cost_function(void)
{
    RoutingCost cost = {0};
    Diagnostic d = {0};

    CHECK(routing_cost_restore3cx(1, &cost, &d) == PQC_OK);
    CHECK(cost.swaps == 0 && cost.extra_cx == 0 && cost.total_cx == 1);
    CHECK(routing_cost_restore3cx(3, &cost, &d) == PQC_OK);
    CHECK(cost.swaps == 4 && cost.extra_cx == 12 && cost.total_cx == 13);
    CHECK(routing_cost_restore3cx(0, &cost, &d) == PQC_E_RANGE);
    CHECK(routing_cost_restore3cx(64, &cost, &d) == PQC_E_RANGE);
}

static void test_required_topologies(void)
{
    static const size_t line_edges[][2] = {{0, 1}, {1, 2}, {2, 3}};
    static const size_t disconnected_edges[][2] = {{0, 1}, {2, 3}};
    Circuit circuit = {0};
    Graph *line;
    Graph *disconnected;
    Analysis analysis = {0};
    Diagnostic d = {0};

    CHECK(circuit_init(&circuit, 4, &d) == PQC_OK);
    append_cx(&circuit, 0, 1, 6);
    append_cx(&circuit, 0, 3, 7);
    append_cx(&circuit, 2, 3, 8);
    line = make_graph(4, line_edges, 3);
    disconnected = make_graph(4, disconnected_edges, 2);

    CHECK(analysis_build(&circuit, line, &analysis, &d) == PQC_OK);
    CHECK(analysis.count == 3 && analysis.direct_count == 2 &&
          analysis.indirect_count == 1 && analysis.unreachable_count == 0);
    CHECK(analysis.rows[0].gate_index == 1 &&
          analysis.rows[0].source_line == 6);
    CHECK(analysis.rows[0].control == 0 && analysis.rows[0].target == 1);
    CHECK(analysis.rows[1].path.length == 4);
    CHECK(analysis.rows[1].path.vertex[0] == 0 &&
          analysis.rows[1].path.vertex[3] == 3);
    CHECK(analysis.full_cost.available);
    CHECK(analysis.full_cost.value.swaps == 4);
    CHECK(analysis.full_cost.value.extra_cx == 12);
    CHECK(analysis.full_cost.value.total_cx == 15);
    analysis_destroy(&analysis);

    CHECK(analysis_build(&circuit, disconnected, &analysis, &d) == PQC_OK);
    CHECK(analysis.direct_count == 2 && analysis.unreachable_count == 1);
    CHECK(analysis.rows[1].classification == CX_UNREACHABLE);
    CHECK(!analysis.rows[1].cost.available);
    CHECK(!analysis.full_cost.available);
    analysis_destroy(&analysis);

    graph_destroy(&line);
    graph_destroy(&disconnected);
    circuit_destroy(&circuit);
}

static void test_repetition_zero_and_mapping(void)
{
    static const size_t line_edges[][2] = {{0, 1}, {1, 2}, {2, 3}};
    Circuit circuit = {0};
    Circuit zero = {0};
    Graph *line = make_graph(4, line_edges, 3);
    Graph *too_small = NULL;
    Analysis analysis = {0};
    Diagnostic d = {0};

    CHECK(circuit_init(&circuit, 4, &d) == PQC_OK);
    append_cx(&circuit, 0, 1, 2);
    append_cx(&circuit, 0, 3, 3);
    append_cx(&circuit, 0, 3, 4);
    CHECK(analysis_build(&circuit, line, &analysis, &d) == PQC_OK);
    CHECK(analysis.count == 3);
    CHECK(analysis.full_cost.available);
    CHECK(analysis.full_cost.value.swaps == 8);
    CHECK(analysis.full_cost.value.extra_cx == 24);
    CHECK(analysis.full_cost.value.total_cx == 27);
    analysis_destroy(&analysis);

    CHECK(circuit_init(&zero, 4, &d) == PQC_OK);
    CHECK(analysis_build(&zero, line, &analysis, &d) == PQC_OK);
    CHECK(analysis.count == 0 && analysis.rows == NULL);
    CHECK(analysis.full_cost.available);
    CHECK(analysis.full_cost.value.total_cx == 0);
    analysis_destroy(&analysis);

    CHECK(graph_create(3, &too_small, &d) == PQC_OK);
    CHECK(analysis_build(&zero, too_small, &analysis, &d) == PQC_E_MAPPING);
    CHECK(analysis.rows == NULL && analysis.count == 0);

    graph_destroy(&too_small);
    graph_destroy(&line);
    circuit_destroy(&zero);
    circuit_destroy(&circuit);
}

static void test_row_allocation_failure(void)
{
    static const size_t edges[][2] = {{0, 1}};
    Circuit circuit = {0};
    Graph *graph;
    Analysis analysis = {0};
    Diagnostic d = {0};
    size_t owned;

    CHECK(circuit_init(&circuit, 2, &d) == PQC_OK);
    append_cx(&circuit, 0, 1, 2);
    graph = make_graph(2, edges, 1);
    owned = pqc_test_live_blocks();
    pqc_test_fail_alloc_on(1);
    CHECK(analysis_build(&circuit, graph, &analysis, &d) == PQC_E_ALLOC);
    CHECK(analysis.rows == NULL && analysis.count == 0);
    CHECK(pqc_test_live_blocks() == owned);
    pqc_test_fail_alloc_on(0);

    graph_destroy(&graph);
    circuit_destroy(&circuit);
}

int test_analysis(void)
{
    failures = 0;
    pqc_test_fail_alloc_on(0);
    test_cost_function();
    test_required_topologies();
    test_repetition_zero_and_mapping();
    test_row_allocation_failure();
    pqc_test_fail_alloc_on(0);

    CHECK(pqc_test_live_blocks() == 0);
    if (failures == 0) {
        printf("test_analysis: PASS\n");
    }
    return failures;
}
