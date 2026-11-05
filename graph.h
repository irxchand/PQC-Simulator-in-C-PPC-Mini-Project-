#ifndef PQC_GRAPH_H
#define PQC_GRAPH_H

#include "common.h"

typedef struct Graph Graph;

typedef struct {
    bool reachable;
    size_t length;
    size_t vertex[PQC_MAX_VERTICES];
} GraphPath;

PqcStatus graph_create(size_t vertices, Graph **out, Diagnostic *d);
void graph_destroy(Graph **graph);
size_t graph_vertex_count(const Graph *graph);
size_t graph_edge_count(const Graph *graph);
PqcStatus graph_add_edge(Graph *graph, size_t u, size_t v,
                         bool *inserted, Diagnostic *d);
PqcStatus graph_adjacent(const Graph *graph, size_t u, size_t v,
                         bool *adjacent, Diagnostic *d);
PqcStatus graph_shortest_path(const Graph *graph, size_t source,
                              size_t target, GraphPath *out,
                              Diagnostic *d);

#endif
