#include "analysis.h"

#include <stdint.h>
#include <string.h>

static bool analysis_is_empty(const Analysis *analysis)
{
    return analysis != NULL && analysis->count == 0 &&
           analysis->direct_count == 0 && analysis->indirect_count == 0 &&
           analysis->unreachable_count == 0 && analysis->rows == NULL &&
           analysis->reachable_sum.swaps == 0 &&
           analysis->reachable_sum.extra_cx == 0 &&
           analysis->reachable_sum.total_cx == 0 &&
           !analysis->full_cost.available &&
           analysis->full_cost.value.swaps == 0 &&
           analysis->full_cost.value.extra_cx == 0 &&
           analysis->full_cost.value.total_cx == 0;
}

static bool add_u64(uint64_t a, uint64_t b, uint64_t *out)
{
    if (UINT64_MAX - a < b) {
        return false;
    }
    *out = a + b;
    return true;
}

static PqcStatus add_cost(RoutingCost *sum, const RoutingCost *value,
                          Diagnostic *d)
{
    RoutingCost result;

    if (!add_u64(sum->swaps, value->swaps, &result.swaps) ||
        !add_u64(sum->extra_cx, value->extra_cx, &result.extra_cx) ||
        !add_u64(sum->total_cx, value->total_cx, &result.total_cx)) {
        pqc_diag_set(d, PQC_E_LIMIT, NULL, 0,
                     "routing-cost total overflow");
        return PQC_E_LIMIT;
    }
    *sum = result;
    return PQC_OK;
}

PqcStatus routing_cost_restore3cx(size_t distance, RoutingCost *out,
                                 Diagnostic *d)
{
    uint64_t steps;

    if (out == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "routing-cost output is missing");
        return PQC_E_INTERNAL;
    }
    if (distance == 0 || distance >= PQC_MAX_VERTICES) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "routing distance must be between 1 and 63");
        return PQC_E_RANGE;
    }

    steps = (uint64_t)(distance - 1);
    out->swaps = 2u * steps;
    out->extra_cx = 6u * steps;
    out->total_cx = 1u + out->extra_cx;
    return PQC_OK;
}

PqcStatus analysis_build(const Circuit *circuit, const Graph *graph,
                         Analysis *out, Diagnostic *d)
{
    Analysis temporary = {0};
    size_t vertices;
    size_t i;
    size_t row_index = 0;
    PqcStatus status;

    if (!analysis_is_empty(out)) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "analysis output must be empty");
        return PQC_E_INTERNAL;
    }
    status = circuit_validate(circuit, d);
    if (status != PQC_OK) {
        return status;
    }
    vertices = graph_vertex_count(graph);
    if (vertices == 0) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "graph is not initialized");
        return PQC_E_INTERNAL;
    }
    if (vertices < circuit->logical_qubits) {
        pqc_diag_set(d, PQC_E_MAPPING, NULL, 0,
                     "identity mapping requires at least as many physical vertices as logical qubits");
        return PQC_E_MAPPING;
    }

    for (i = 0; i < circuit->count; i++) {
        if (circuit->operations[i].gate.kind == GATE_CX) {
            temporary.count++;
        }
    }
    if (temporary.count > 0) {
        size_t bytes;

        if (!pqc_size_mul(temporary.count, sizeof(*temporary.rows), &bytes)) {
            pqc_diag_set(d, PQC_E_LIMIT, NULL, 0,
                         "analysis row allocation size overflow");
            return PQC_E_LIMIT;
        }
        temporary.rows = pqc_calloc(temporary.count,
                                    sizeof(*temporary.rows));
        if (temporary.rows == NULL) {
            pqc_diag_set(d, PQC_E_ALLOC, NULL, 0,
                         "could not allocate analysis rows");
            return PQC_E_ALLOC;
        }
        (void)bytes;
    }

    for (i = 0; i < circuit->count; i++) {
        const Operation *operation = &circuit->operations[i];
        CxAnalysis *row;
        bool adjacent;
        size_t distance;

        if (operation->gate.kind != GATE_CX) {
            continue;
        }
        row = &temporary.rows[row_index];
        row->cx_index = row_index + 1;
        row->gate_index = i + 1;
        row->source_line = operation->source_line;
        row->control = (size_t)operation->gate.control;
        row->target = (size_t)operation->gate.target;
        row->physical_control = row->control;
        row->physical_target = row->target;

        status = graph_adjacent(graph, row->physical_control,
                                row->physical_target, &adjacent, d);
        if (status != PQC_OK) {
            goto fail;
        }
        status = graph_shortest_path(graph, row->physical_control,
                                     row->physical_target, &row->path, d);
        if (status != PQC_OK) {
            goto fail;
        }
        if (!row->path.reachable) {
            if (adjacent) {
                pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                             "adjacency and BFS results disagree");
                status = PQC_E_INTERNAL;
                goto fail;
            }
            row->classification = CX_UNREACHABLE;
            temporary.unreachable_count++;
            row_index++;
            continue;
        }
        if (row->path.length < 2) {
            pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                         "CX path has invalid length");
            status = PQC_E_INTERNAL;
            goto fail;
        }
        distance = row->path.length - 1;
        if ((adjacent && distance != 1) || (!adjacent && distance == 1)) {
            pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                         "adjacency and shortest path disagree");
            status = PQC_E_INTERNAL;
            goto fail;
        }
        status = routing_cost_restore3cx(distance, &row->cost.value, d);
        if (status != PQC_OK) {
            goto fail;
        }
        row->cost.available = true;
        if (adjacent) {
            row->classification = CX_DIRECT;
            temporary.direct_count++;
        } else {
            row->classification = CX_INDIRECT;
            temporary.indirect_count++;
        }
        status = add_cost(&temporary.reachable_sum, &row->cost.value, d);
        if (status != PQC_OK) {
            goto fail;
        }
        row_index++;
    }

    if (temporary.count != temporary.direct_count +
                           temporary.indirect_count +
                           temporary.unreachable_count) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "analysis counts are inconsistent");
        status = PQC_E_INTERNAL;
        goto fail;
    }
    if (temporary.unreachable_count == 0) {
        temporary.full_cost.available = true;
        temporary.full_cost.value = temporary.reachable_sum;
    }
    *out = temporary;
    return PQC_OK;

fail:
    analysis_destroy(&temporary);
    return status;
}

void analysis_destroy(Analysis *analysis)
{
    if (analysis == NULL) {
        return;
    }
    pqc_free(analysis->rows);
    memset(analysis, 0, sizeof(*analysis));
}
