#include "graph.h"

#include <string.h>

typedef struct AdjNode {
    size_t vertex;
    struct AdjNode *next;
} AdjNode;

struct Graph {
    size_t vertex_count;
    size_t edge_count;
    AdjNode **head;
};

static bool graph_is_valid(const Graph *graph)
{
    return graph != NULL && graph->head != NULL &&
           graph->vertex_count > 0 &&
           graph->vertex_count <= PQC_MAX_VERTICES;
}

static AdjNode **find_position(AdjNode **head, size_t vertex)
{
    AdjNode **position = head;

    while (*position != NULL && (*position)->vertex < vertex) {
        position = &(*position)->next;
    }
    return position;
}

PqcStatus graph_create(size_t vertices, Graph **out, Diagnostic *d)
{
    Graph *graph;

    if (out == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "graph output is missing");
        return PQC_E_INTERNAL;
    }
    if (*out != NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "graph output must be empty");
        return PQC_E_INTERNAL;
    }
    if (vertices == 0) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "physical vertex count must be positive");
        return PQC_E_RANGE;
    }
    if (vertices > PQC_MAX_VERTICES) {
        pqc_diag_set(d, PQC_E_LIMIT, NULL, 0,
                     "physical vertex count exceeds %u", PQC_MAX_VERTICES);
        return PQC_E_LIMIT;
    }

    graph = pqc_malloc(sizeof(*graph));
    if (graph == NULL) {
        pqc_diag_set(d, PQC_E_ALLOC, NULL, 0,
                     "could not allocate graph");
        return PQC_E_ALLOC;
    }
    graph->vertex_count = vertices;
    graph->edge_count = 0;
    graph->head = pqc_calloc(vertices, sizeof(*graph->head));
    if (graph->head == NULL) {
        pqc_free(graph);
        pqc_diag_set(d, PQC_E_ALLOC, NULL, 0,
                     "could not allocate adjacency lists");
        return PQC_E_ALLOC;
    }

    *out = graph;
    return PQC_OK;
}

void graph_destroy(Graph **graph)
{
    size_t i;

    if (graph == NULL || *graph == NULL) {
        return;
    }

    for (i = 0; i < (*graph)->vertex_count; i++) {
        AdjNode *current = (*graph)->head[i];

        while (current != NULL) {
            AdjNode *next = current->next;
            pqc_free(current);
            current = next;
        }
    }
    pqc_free((*graph)->head);
    pqc_free(*graph);
    *graph = NULL;
}

size_t graph_vertex_count(const Graph *graph)
{
    if (!graph_is_valid(graph)) {
        return 0;
    }
    return graph->vertex_count;
}

size_t graph_edge_count(const Graph *graph)
{
    if (!graph_is_valid(graph)) {
        return 0;
    }
    return graph->edge_count;
}

PqcStatus graph_add_edge(Graph *graph, size_t u, size_t v,
                         bool *inserted, Diagnostic *d)
{
    AdjNode **u_position;
    AdjNode **v_position;
    AdjNode *u_node;
    AdjNode *v_node;

    if (inserted == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "edge result is missing");
        return PQC_E_INTERNAL;
    }
    *inserted = false;
    if (!graph_is_valid(graph)) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "graph is not initialized");
        return PQC_E_INTERNAL;
    }
    if (u >= graph->vertex_count || v >= graph->vertex_count) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "edge endpoint is out of range");
        return PQC_E_RANGE;
    }
    if (u == v) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "self-loops are not supported");
        return PQC_E_RANGE;
    }

    u_position = find_position(&graph->head[u], v);
    if (*u_position != NULL && (*u_position)->vertex == v) {
        return PQC_OK;
    }
    v_position = find_position(&graph->head[v], u);

    u_node = pqc_malloc(sizeof(*u_node));
    if (u_node == NULL) {
        pqc_diag_set(d, PQC_E_ALLOC, NULL, 0,
                     "could not allocate adjacency node");
        return PQC_E_ALLOC;
    }
    v_node = pqc_malloc(sizeof(*v_node));
    if (v_node == NULL) {
        pqc_free(u_node);
        pqc_diag_set(d, PQC_E_ALLOC, NULL, 0,
                     "could not allocate reverse adjacency node");
        return PQC_E_ALLOC;
    }

    u_node->vertex = v;
    u_node->next = *u_position;
    v_node->vertex = u;
    v_node->next = *v_position;
    *u_position = u_node;
    *v_position = v_node;
    graph->edge_count++;
    *inserted = true;
    return PQC_OK;
}

PqcStatus graph_adjacent(const Graph *graph, size_t u, size_t v,
                         bool *adjacent, Diagnostic *d)
{
    AdjNode *current;

    if (adjacent == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "adjacency result is missing");
        return PQC_E_INTERNAL;
    }
    *adjacent = false;
    if (!graph_is_valid(graph)) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "graph is not initialized");
        return PQC_E_INTERNAL;
    }
    if (u >= graph->vertex_count || v >= graph->vertex_count) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "adjacency endpoint is out of range");
        return PQC_E_RANGE;
    }

    current = graph->head[u];
    while (current != NULL && current->vertex < v) {
        current = current->next;
    }
    *adjacent = current != NULL && current->vertex == v;
    return PQC_OK;
}

PqcStatus graph_shortest_path(const Graph *graph, size_t source,
                              size_t target, GraphPath *out,
                              Diagnostic *d)
{
    bool visited[PQC_MAX_VERTICES] = {false};
    size_t queue[PQC_MAX_VERTICES];
    size_t parent[PQC_MAX_VERTICES];
    size_t reverse[PQC_MAX_VERTICES];
    size_t queue_start = 0;
    size_t queue_end = 0;
    size_t reverse_length = 0;
    size_t current;

    if (out == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "path output is missing");
        return PQC_E_INTERNAL;
    }
    memset(out, 0, sizeof(*out));
    if (!graph_is_valid(graph)) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "graph is not initialized");
        return PQC_E_INTERNAL;
    }
    if (source >= graph->vertex_count || target >= graph->vertex_count) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "path endpoint is out of range");
        return PQC_E_RANGE;
    }
    if (source == target) {
        out->reachable = true;
        out->length = 1;
        out->vertex[0] = source;
        return PQC_OK;
    }

    visited[source] = true;
    queue[queue_end++] = source;

    while (queue_start < queue_end && !visited[target]) {
        AdjNode *neighbor;

        current = queue[queue_start++];
        neighbor = graph->head[current];
        while (neighbor != NULL) {
            size_t next = neighbor->vertex;

            if (next >= graph->vertex_count) {
                pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                             "adjacency list contains an invalid vertex");
                return PQC_E_INTERNAL;
            }
            if (!visited[next]) {
                if (queue_end >= graph->vertex_count) {
                    pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                                 "BFS queue overflow");
                    return PQC_E_INTERNAL;
                }
                visited[next] = true;
                parent[next] = current;
                queue[queue_end++] = next;
            }
            neighbor = neighbor->next;
        }
    }

    if (!visited[target]) {
        return PQC_OK;
    }

    current = target;
    while (current != source) {
        if (reverse_length >= graph->vertex_count || !visited[current]) {
            pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                         "invalid BFS parent chain");
            return PQC_E_INTERNAL;
        }
        reverse[reverse_length++] = current;
        current = parent[current];
    }
    reverse[reverse_length++] = source;

    out->reachable = true;
    out->length = reverse_length;
    for (current = 0; current < reverse_length; current++) {
        out->vertex[current] = reverse[reverse_length - current - 1];
    }
    return PQC_OK;
}
