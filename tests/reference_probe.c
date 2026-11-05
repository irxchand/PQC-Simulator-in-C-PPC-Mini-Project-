#include "qml.h"
#include "sim.h"

#include <stdio.h>

int main(void)
{
    static const double features[4][2] = {
        {0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}
    };
    double theta[4] = {2.639, 1.239, 2.461, 2.508};
    double zero[4] = {0.0, 0.0, 0.0, 0.0};
    SimState state = {0};
    Diagnostic d = {0};
    QmlTrainResult training;
    Gate h = {GATE_H, 0, -1, 0.0};
    Gate cx = {GATE_CX, 1, 0, 0.0};
    size_t i;

    if (sim_create(2, &state, &d) != PQC_OK ||
        sim_apply_gate(&state, &h, &d) != PQC_OK ||
        sim_apply_gate(&state, &cx, &d) != PQC_OK) {
        return 1;
    }
    for (i = 0; i < state.size; i++) {
        printf("BELL %u %.17g %.17g\n", (unsigned)i,
               state.amplitude[i].r, state.amplitude[i].i);
    }
    sim_destroy(&state);

    for (i = 0; i < 4; i++) {
        double encoding;
        double initial;
        double zero_full;
        if (qml_encoding_only(features[i], &encoding, &d) != PQC_OK ||
            qml_forward(features[i], theta, 2, &initial, &d) != PQC_OK ||
            qml_forward(features[i], zero, 2, &zero_full, &d) != PQC_OK) {
            return 1;
        }
        printf("QML %u %.17g %.17g %.17g\n", (unsigned)i,
               encoding, initial, zero_full);
    }
    if (qml_train_xor(theta, NULL, &training, &d) != PQC_OK) {
        return 1;
    }
    printf("TRAIN %.17g %.17g %u %u %.17g\n",
           training.initial_mse, training.final_mse,
           training.initial_correct, training.final_correct,
           training.epoch200_preupdate_mse);
    for (i = 0; i < 4; i++) {
        printf("PRED %u %.17g\n", (unsigned)i,
               training.final_probability[i]);
    }
    return 0;
}
