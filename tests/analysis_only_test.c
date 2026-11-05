#include "analysis.h"

#include <stdio.h>

int main(void)
{
    Circuit circuit = {0};
    Graph *graph = NULL;
    Analysis analysis = {0};
    Diagnostic d = {0};
    PqcStatus status;
    size_t i;

    status = circuit_init(&circuit, 30, &d);
    if (status != PQC_OK) {
        return 1;
    }
    status = graph_create(30, &graph, &d);
    if (status != PQC_OK) {
        circuit_destroy(&circuit);
        return 1;
    }
    for (i = 0; i + 1 < 30; i++) {
        bool inserted = false;
        status = graph_add_edge(graph, i, i + 1, &inserted, &d);
        if (status != PQC_OK || !inserted) {
            graph_destroy(&graph);
            circuit_destroy(&circuit);
            return 1;
        }
    }
    status = analysis_build(&circuit, graph, &analysis, &d);
    if (status != PQC_OK || analysis.count != 0 ||
        !analysis.full_cost.available) {
        analysis_destroy(&analysis);
        graph_destroy(&graph);
        circuit_destroy(&circuit);
        return 1;
    }

    analysis_destroy(&analysis);
    graph_destroy(&graph);
    circuit_destroy(&circuit);
    printf("30-qubit analysis-only test passed.\n");
    return 0;
}
