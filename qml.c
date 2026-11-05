#include "qml.h"

#include "circuit.h"
#include "sim.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

static const double xor_features[4][2] = {
    {0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}
};
static const double xor_targets[4] = {0.0, 1.0, 1.0, 0.0};

static PqcStatus apply_encoding(SimState *state, const double *features,
                                size_t count, Diagnostic *d)
{
    size_t i;
    for (i = 0; i < count; i++) {
        Gate gate = {
            .kind = GATE_RY,
            .target = (int)i,
            .control = -1,
            .angle = features[i] * PQC_PI
        };
        PqcStatus status = sim_apply_gate(state, &gate, d);
        if (status != PQC_OK) {
            return status;
        }
    }
    return PQC_OK;
}

void qml_legacy_initial_theta(double out[4])
{
    size_t i;
    if (out == NULL) {
        return;
    }
    for (i = 0; i < 4; i++) {
        out[i] = ((double)rand() / RAND_MAX) * PQC_PI;
    }
}

PqcStatus qml_forward(const double *features, const double *theta,
                      size_t feature_count, double *probability,
                      Diagnostic *d)
{
    SimState state = {0};
    PqcStatus status;
    size_t i;

    if (features == NULL || theta == NULL || probability == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "QML forward argument is missing");
        return PQC_E_INTERNAL;
    }
    if (feature_count == 0) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "feature count must be positive");
        return PQC_E_RANGE;
    }
    if (feature_count > PQC_MAX_SIM_QUBITS) {
        pqc_diag_set(d, PQC_E_LIMIT, NULL, 0,
                     "feature count exceeds simulation limit");
        return PQC_E_LIMIT;
    }
    for (i = 0; i < feature_count; i++) {
        if (!isfinite(features[i]) || !isfinite(theta[i]) ||
            !isfinite(theta[feature_count + i])) {
            pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                         "QML inputs and parameters must be finite");
            return PQC_E_RANGE;
        }
    }
    status = sim_create(feature_count, &state, d);
    if (status != PQC_OK) {
        return status;
    }
    status = apply_encoding(&state, features, feature_count, d);
    for (i = 0; status == PQC_OK && i < feature_count; i++) {
        Gate gate = {GATE_RY, (int)i, -1, theta[i]};
        status = sim_apply_gate(&state, &gate, d);
    }
    for (i = 0; status == PQC_OK && i + 1 < feature_count; i++) {
        Gate gate = {GATE_CX, (int)(i + 1), (int)i, 0.0};
        status = sim_apply_gate(&state, &gate, d);
    }
    for (i = 0; status == PQC_OK && i < feature_count; i++) {
        Gate gate = {GATE_RY, (int)i, -1, theta[feature_count + i]};
        status = sim_apply_gate(&state, &gate, d);
    }
    if (status == PQC_OK) {
        status = sim_odd_probability(&state, probability, d);
    }
    sim_destroy(&state);
    return status;
}

PqcStatus qml_encoding_only(const double features[2], double *probability,
                            Diagnostic *d)
{
    SimState state = {0};
    PqcStatus status;

    if (features == NULL || probability == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "encoding-only argument is missing");
        return PQC_E_INTERNAL;
    }
    if (!isfinite(features[0]) || !isfinite(features[1])) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "features must be finite");
        return PQC_E_RANGE;
    }
    status = sim_create(2, &state, d);
    if (status == PQC_OK) {
        status = apply_encoding(&state, features, 2, d);
    }
    if (status == PQC_OK) {
        status = sim_odd_probability(&state, probability, d);
    }
    sim_destroy(&state);
    return status;
}

static PqcStatus observe(const double theta[4], double probability[4],
                         double *mse, unsigned *correct, Diagnostic *d)
{
    size_t k;
    double error = 0.0;
    unsigned matches = 0;

    for (k = 0; k < 4; k++) {
        double difference;
        PqcStatus status = qml_forward(xor_features[k], theta, 2,
                                       &probability[k], d);
        if (status != PQC_OK) {
            return status;
        }
        difference = probability[k] - xor_targets[k];
        error += difference * difference;
        if ((probability[k] >= 0.5) == (xor_targets[k] >= 0.5)) {
            matches++;
        }
    }
    error /= 4.0;
    if (!isfinite(error)) {
        pqc_diag_set(d, PQC_E_NUMERIC, NULL, 0,
                     "QML metric is nonfinite");
        return PQC_E_NUMERIC;
    }
    *mse = error;
    *correct = matches;
    return PQC_OK;
}

static PqcStatus write_progress_intro(FILE *stream, Diagnostic *d)
{
    if (stream == NULL) {
        return PQC_OK;
    }
    if (fprintf(stream,
        "\n[PQC Explanation]\n"
        "A Parameterized Quantum Circuit (PQC) acts as a supporting quantum machine learning demonstration.\n"
        "1. Data Encoding: Input features are mapped to quantum states via RY rotations.\n"
        "2. Parameter Layers: Trainable RY rotations apply continuous weights.\n"
        "3. Entanglement: CX gates correlate qubits to capture feature interactions.\n"
        "4. Measurement: Odd-parity probability is the classification output.\n"
        "Parameters are optimized using mean squared error and gradient descent.\n\n"
        "Training XOR problem using 2 features.\n") < 0) {
        pqc_diag_set(d, PQC_E_IO, NULL, 0,
                     "could not write QML progress");
        return PQC_E_IO;
    }
    return PQC_OK;
}

PqcStatus qml_train_xor(const double initial_theta[4], FILE *progress,
                        QmlTrainResult *out, Diagnostic *d)
{
    QmlTrainResult result = {0};
    double theta[4];
    const double learning_rate = 0.5;
    const double epsilon = 1e-4;
    size_t i;
    int epoch;
    PqcStatus status;

    if (initial_theta == NULL || out == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "QML training argument is missing");
        return PQC_E_INTERNAL;
    }
    memset(out, 0, sizeof(*out));
    for (i = 0; i < 4; i++) {
        if (!isfinite(initial_theta[i])) {
            pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                         "initial parameters must be finite");
            return PQC_E_RANGE;
        }
        theta[i] = initial_theta[i];
        result.initial_theta[i] = initial_theta[i];
    }
    status = write_progress_intro(progress, d);
    if (status != PQC_OK) {
        return status;
    }
    status = observe(theta, result.initial_probability, &result.initial_mse,
                     &result.initial_correct, d);
    if (status != PQC_OK) {
        return status;
    }

    for (epoch = 1; epoch <= 200; epoch++) {
        double error = 0.0;
        double gradient[4] = {0.0, 0.0, 0.0, 0.0};
        size_t k;

        for (k = 0; k < 4; k++) {
            double probability;
            double difference;

            status = qml_forward(xor_features[k], theta, 2,
                                 &probability, d);
            if (status != PQC_OK) {
                return status;
            }
            difference = probability - xor_targets[k];
            error += difference * difference;
            for (i = 0; i < 4; i++) {
                double saved = theta[i];
                double plus;
                double minus;
                double derivative;

                theta[i] = saved + epsilon;
                status = qml_forward(xor_features[k], theta, 2, &plus, d);
                if (status != PQC_OK) {
                    theta[i] = saved;
                    return status;
                }
                theta[i] = saved - epsilon;
                status = qml_forward(xor_features[k], theta, 2, &minus, d);
                theta[i] = saved;
                if (status != PQC_OK) {
                    return status;
                }
                derivative = (plus - minus) / (2.0 * epsilon);
                gradient[i] += 2.0 * difference * derivative;
            }
        }
        error /= 4.0;
        if (!isfinite(error)) {
            pqc_diag_set(d, PQC_E_NUMERIC, NULL, 0,
                         "training loss is nonfinite");
            return PQC_E_NUMERIC;
        }
        if (epoch == 200) {
            result.epoch200_preupdate_mse = error;
        }
        for (i = 0; i < 4; i++) {
            double update = learning_rate * (gradient[i] / 4.0);
            if (!isfinite(gradient[i]) || !isfinite(update) ||
                !isfinite(theta[i] - update)) {
                pqc_diag_set(d, PQC_E_NUMERIC, NULL, 0,
                             "training update is nonfinite");
                return PQC_E_NUMERIC;
            }
            theta[i] -= update;
        }
        if (progress != NULL && (epoch == 1 || epoch % 50 == 0)) {
            if (fprintf(progress, "Epoch %03d, Error: %.6f\n",
                        epoch, error) < 0) {
                pqc_diag_set(d, PQC_E_IO, NULL, 0,
                             "could not write QML progress");
                return PQC_E_IO;
            }
        }
    }

    for (i = 0; i < 4; i++) {
        result.final_theta[i] = theta[i];
    }
    status = observe(theta, result.final_probability, &result.final_mse,
                     &result.final_correct, d);
    if (status != PQC_OK) {
        return status;
    }
    if (progress != NULL) {
        if (fprintf(progress, "\nPredictions:\n") < 0) {
            pqc_diag_set(d, PQC_E_IO, NULL, 0,
                         "could not write QML predictions");
            return PQC_E_IO;
        }
        for (i = 0; i < 4; i++) {
            if (fprintf(progress,
                        "Input [ %.1f, %.1f ] -> Pred: %.4f (Expected: %.1f)\n",
                        xor_features[i][0], xor_features[i][1],
                        result.final_probability[i], xor_targets[i]) < 0) {
                pqc_diag_set(d, PQC_E_IO, NULL, 0,
                             "could not write QML predictions");
                return PQC_E_IO;
            }
        }
        if (fprintf(progress, "\n") < 0) {
            pqc_diag_set(d, PQC_E_IO, NULL, 0,
                         "could not finish QML progress");
            return PQC_E_IO;
        }
    }
    *out = result;
    return PQC_OK;
}
