#ifndef PQC_QML_H
#define PQC_QML_H

#include "common.h"

typedef struct {
    double initial_theta[4];
    double final_theta[4];
    double initial_probability[4];
    double final_probability[4];
    double initial_mse;
    double final_mse;
    double epoch200_preupdate_mse;
    unsigned initial_correct;
    unsigned final_correct;
} QmlTrainResult;

void qml_legacy_initial_theta(double out[4]);
PqcStatus qml_forward(const double *features, const double *theta,
                      size_t feature_count, double *probability,
                      Diagnostic *d);
PqcStatus qml_encoding_only(const double features[2], double *probability,
                            Diagnostic *d);
PqcStatus qml_train_xor(const double initial_theta[4], FILE *progress,
                        QmlTrainResult *out, Diagnostic *d);

#endif
