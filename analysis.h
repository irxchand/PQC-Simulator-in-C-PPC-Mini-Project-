#ifndef PQC_ANALYSIS_H
#define PQC_ANALYSIS_H

#include "common.h"
#include "circuit.h"
#include "graph.h"

typedef struct {
    uint64_t swaps;
    uint64_t extra_cx;
    uint64_t total_cx;
} RoutingCost;

typedef struct {
    bool available;
    RoutingCost value;
} OptionalCost;

typedef enum {
    CX_DIRECT,
    CX_INDIRECT,
    CX_UNREACHABLE
} CxClass;

typedef struct {
    size_t cx_index;
    size_t gate_index;
    size_t source_line;
    size_t control;
    size_t target;
    size_t physical_control;
    size_t physical_target;
    CxClass classification;
    GraphPath path;
    OptionalCost cost;
} CxAnalysis;

typedef struct {
    size_t count;
    size_t direct_count;
    size_t indirect_count;
    size_t unreachable_count;
    CxAnalysis *rows;
    RoutingCost reachable_sum;
    OptionalCost full_cost;
} Analysis;

PqcStatus routing_cost_restore3cx(size_t distance, RoutingCost *out,
                                 Diagnostic *d);
PqcStatus analysis_build(const Circuit *circuit, const Graph *graph,
                         Analysis *out, Diagnostic *d);
void analysis_destroy(Analysis *analysis);

#endif
