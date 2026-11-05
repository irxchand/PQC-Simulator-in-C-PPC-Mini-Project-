#include "graph.h"

#include <stdio.h>

static int failures = 0;

#define CHECK(condition) do { \
    if (!(condition)) { \
        printf("FAIL test_graph:%d: %s\n", __LINE__, #condition); \
        failures++; \
    } \
} while (0)

static void add_edge(Graph *graph, size_t u, size_t v)
{
    Diagnostic d = {0};
    bool inserted = false;

    CHECK(graph_add_edge(graph, u, v, &inserted, &d) == PQC_OK);
    CHECK(inserted);
}

static void test_create_and_edges(void)
{
    Graph *graph = NULL;
    Diagnostic d = {0};
    bool inserted = false;
    bool adjacent = false;
    size_t start = pqc_test_live_blocks();

    CHECK(graph_create(1, &graph, &d) == PQC_OK);
    CHECK(graph_vertex_count(graph) == 1);
    CHECK(graph_edge_count(graph) == 0);
    CHECK(graph_add_edge(graph, 0, 0, &inserted, &d) == PQC_E_RANGE);
    graph_destroy(&graph);
    CHECK(graph == NULL);
    CHECK(pqc_test_live_blocks() == start);

    CHECK(graph_create(4, &graph, &d) == PQC_OK);
    add_edge(graph, 2, 3);
    add_edge(graph, 0, 2);
    add_edge(graph, 0, 1);
    CHECK(graph_edge_count(graph) == 3);
    CHECK(graph_adjacent(graph, 0, 1, &adjacent, &d) == PQC_OK);
    CHECK(adjacent);
    CHECK(graph_adjacent(graph, 1, 0, &adjacent, &d) == PQC_OK);
    CHECK(adjacent);
    CHECK(graph_adjacent(graph, 1, 3, &adjacent, &d) == PQC_OK);
    CHECK(!adjacent);

    pqc_test_fail_alloc_on(1);
    CHECK(graph_add_edge(graph, 1, 0, &inserted, &d) == PQC_OK);
    CHECK(!inserted);
    CHECK(pqc_test_alloc_attempts() == 0);
    CHECK(graph_edge_count(graph) == 3);
    pqc_test_fail_alloc_on(0);

    CHECK(graph_add_edge(graph, 0, 4, &inserted, &d) == PQC_E_RANGE);
    CHECK(graph_add_edge(graph, 2, 2, &inserted, &d) == PQC_E_RANGE);
    graph_destroy(&graph);
    graph_destroy(&graph);
    CHECK(pqc_test_live_blocks() == start);
}

static void test_allocation_failures(void)
{
    Graph *graph = NULL;
    Diagnostic d = {0};
    bool inserted = false;
    bool adjacent = true;
    size_t start = pqc_test_live_blocks();
    size_t owned;

    pqc_test_fail_alloc_on(1);
    CHECK(graph_create(4, &graph, &d) == PQC_E_ALLOC);
    CHECK(graph == NULL);
    CHECK(pqc_test_live_blocks() == start);

    pqc_test_fail_alloc_on(2);
    CHECK(graph_create(4, &graph, &d) == PQC_E_ALLOC);
    CHECK(graph == NULL);
    CHECK(pqc_test_live_blocks() == start);

    pqc_test_fail_alloc_on(0);
    CHECK(graph_create(4, &graph, &d) == PQC_OK);
    owned = pqc_test_live_blocks();

    pqc_test_fail_alloc_on(1);
    CHECK(graph_add_edge(graph, 0, 1, &inserted, &d) == PQC_E_ALLOC);
    CHECK(!inserted);
    CHECK(graph_edge_count(graph) == 0);
    CHECK(pqc_test_live_blocks() == owned);

    pqc_test_fail_alloc_on(2);
    CHECK(graph_add_edge(graph, 0, 1, &inserted, &d) == PQC_E_ALLOC);
    CHECK(!inserted);
    CHECK(graph_edge_count(graph) == 0);
    CHECK(pqc_test_live_blocks() == owned);
    CHECK(graph_adjacent(graph, 0, 1, &adjacent, &d) == PQC_OK);
    CHECK(!adjacent);

    pqc_test_fail_alloc_on(0);
    graph_destroy(&graph);
    CHECK(pqc_test_live_blocks() == start);
}

static void test_paths(void)
{
    Graph *line = NULL;
    Graph *diamond_a = NULL;
    Graph *diamond_b = NULL;
    Diagnostic d = {0};
    GraphPath path = {0};
    size_t i;

    CHECK(graph_create(5, &line, &d) == PQC_OK);
    add_edge(line, 0, 1);
    add_edge(line, 1, 2);
    add_edge(line, 2, 3);
    CHECK(graph_shortest_path(line, 0, 3, &path, &d) == PQC_OK);
    CHECK(path.reachable && path.length == 4);
    for (i = 0; i < 4; i++) {
        CHECK(path.vertex[i] == i);
    }
    CHECK(graph_shortest_path(line, 1, 2, &path, &d) == PQC_OK);
    CHECK(path.reachable && path.length == 2);
    CHECK(graph_shortest_path(line, 4, 0, &path, &d) == PQC_OK);
    CHECK(!path.reachable && path.length == 0);
    CHECK(graph_shortest_path(line, 4, 4, &path, &d) == PQC_OK);
    CHECK(path.reachable && path.length == 1 && path.vertex[0] == 4);

    CHECK(graph_create(4, &diamond_a, &d) == PQC_OK);
    add_edge(diamond_a, 0, 2);
    add_edge(diamond_a, 2, 3);
    add_edge(diamond_a, 0, 1);
    add_edge(diamond_a, 1, 3);
    CHECK(graph_create(4, &diamond_b, &d) == PQC_OK);
    add_edge(diamond_b, 1, 3);
    add_edge(diamond_b, 0, 1);
    add_edge(diamond_b, 2, 3);
    add_edge(diamond_b, 0, 2);
    CHECK(graph_shortest_path(diamond_a, 0, 3, &path, &d) == PQC_OK);
    CHECK(path.length == 3 && path.vertex[0] == 0 &&
          path.vertex[1] == 1 && path.vertex[2] == 3);
    CHECK(graph_shortest_path(diamond_b, 0, 3, &path, &d) == PQC_OK);
    CHECK(path.length == 3 && path.vertex[0] == 0 &&
          path.vertex[1] == 1 && path.vertex[2] == 3);

    graph_destroy(&line);
    graph_destroy(&diamond_a);
    graph_destroy(&diamond_b);
}

static void test_maximum_path(void)
{
    Graph *graph = NULL;
    Diagnostic d = {0};
    GraphPath path = {0};
    size_t i;

    CHECK(graph_create(PQC_MAX_VERTICES, &graph, &d) == PQC_OK);
    for (i = 0; i + 1 < PQC_MAX_VERTICES; i++) {
        add_edge(graph, i, i + 1);
    }
    CHECK(graph_shortest_path(graph, 0, PQC_MAX_VERTICES - 1,
                              &path, &d) == PQC_OK);
    CHECK(path.reachable && path.length == PQC_MAX_VERTICES);
    for (i = 0; i < path.length; i++) {
        CHECK(path.vertex[i] == i);
    }
    graph_destroy(&graph);
}

int test_graph(void)
{
    failures = 0;
    pqc_test_fail_alloc_on(0);
    test_create_and_edges();
    test_allocation_failures();
    test_paths();
    test_maximum_path();
    pqc_test_fail_alloc_on(0);

    CHECK(pqc_test_live_blocks() == 0);
    if (failures == 0) {
        printf("test_graph: PASS\n");
    }
    return failures;
}
