#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PI 3.14159265358979323846

struct cx {
    double r;
    double i;
};

struct st {
    int n;
    int sz;
    struct cx* arr;
};

struct gt {
    int tp; // 0=X, 1=H, 2=RX, 3=RY, 4=RZ, 5=CX
    int t;
    int c;
    double th;
};

struct st* make_st(int n) {
    if (n < 1 || n > 30) return NULL;
    struct st* s = (struct st*)malloc(sizeof(struct st));
    if (!s) return NULL;
    s->n = n;
    s->sz = 1 << n;
    s->arr = (struct cx*)malloc(s->sz * sizeof(struct cx));
    if (!s->arr) {
        free(s);
        return NULL;
    }
    for (int i = 0; i < s->sz; i++) {
        s->arr[i].r = (i == 0) ? 1.0 : 0.0;
        s->arr[i].i = 0.0;
    }
    return s;
}

void free_st(struct st* s) {
    if (s) {
        free(s->arr);
        free(s);
    }
}

void norm(struct st* s) {
    if (!s) return;
    double sq = 0.0;
    for (int i = 0; i < s->sz; i++) {
        sq += s->arr[i].r * s->arr[i].r + s->arr[i].i * s->arr[i].i;
    }
    if (sq > 0.0) {
        double val = sqrt(sq);
        for (int i = 0; i < s->sz; i++) {
            s->arr[i].r /= val;
            s->arr[i].i /= val;
        }
    }
}

void pr_p(struct st* s) {
    if (!s) return;
    for (int i = 0; i < s->sz; i++) {
        double p = s->arr[i].r * s->arr[i].r + s->arr[i].i * s->arr[i].i;
        if (p > 1e-4) {
            printf(" |");
            for (int j = s->n - 1; j >= 0; j--) {
                printf("%d", (i >> j) & 1);
            }
            printf("> : %.4f\n", p);
        }
    }
    printf("\n");
}

int chk_gt(struct st* s, struct gt g) {
    if (!s) return 0;
    if (g.t < 0 || g.t >= s->n) return 0;
    if (g.tp == 5) {
        if (g.c < 0 || g.c >= s->n) return 0;
        if (g.c == g.t) return 0;
    }
    return 1;
}

void pr_gt(struct gt g) {
    if (g.tp == 0) {
        printf("[X Gate] Applied to qubit %d. Flips the state probability between |0> and |1>.\n", g.t);
    } else if (g.tp == 1) {
        printf("[H Gate] Applied to qubit %d. Creates superposition, mixing |0> and |1> evenly.\n", g.t);
    } else if (g.tp == 2) {
        printf("[RX Gate] Target %d, theta %.4f. Rotates state probability amplitudes around the X-axis of the Bloch sphere.\n", g.t, g.th);
    } else if (g.tp == 3) {
        printf("[RY Gate] Target %d, theta %.4f. Rotates state probability amplitudes around the Y-axis.\n", g.t, g.th);
    } else if (g.tp == 4) {
        printf("[RZ Gate] Target %d, theta %.4f. Applies a relative phase shift around the Z-axis (does not change probability magnitudes).\n", g.t, g.th);
    } else if (g.tp == 5) {
        printf("[CX Gate] Control %d, Target %d. Flips target if control is |1>. Used to create entanglement.\n", g.c, g.t);
    }
}

void app_gt(struct st* s, struct gt g, int exp) {
    if (!chk_gt(s, g)) {
        if (exp) printf("Error: Invalid gate\n");
        return;
    }
    if (exp) {
        pr_gt(g);
    }
    int sz = s->sz;
    int stp = 1 << g.t;
    
    if (g.tp == 0) {
        for (int i = 0; i < sz; i += 2 * stp) {
            for (int j = 0; j < stp; j++) {
                struct cx tmp = s->arr[i + j];
                s->arr[i + j] = s->arr[i + j + stp];
                s->arr[i + j + stp] = tmp;
            }
        }
    } else if (g.tp == 1) {
        double v = 1.0 / sqrt(2.0);
        for (int i = 0; i < sz; i += 2 * stp) {
            for (int j = 0; j < stp; j++) {
                struct cx a = s->arr[i + j];
                struct cx b = s->arr[i + j + stp];
                s->arr[i + j].r = (a.r + b.r) * v;
                s->arr[i + j].i = (a.i + b.i) * v;
                s->arr[i + j + stp].r = (a.r - b.r) * v;
                s->arr[i + j + stp].i = (a.i - b.i) * v;
            }
        }
    } else if (g.tp == 2) {
        double c = cos(g.th / 2.0);
        double sn = sin(g.th / 2.0);
        for (int i = 0; i < sz; i += 2 * stp) {
            for (int j = 0; j < stp; j++) {
                struct cx a = s->arr[i + j];
                struct cx b = s->arr[i + j + stp];
                s->arr[i + j].r = a.r * c + b.i * sn;
                s->arr[i + j].i = a.i * c - b.r * sn;
                s->arr[i + j + stp].r = b.r * c + a.i * sn;
                s->arr[i + j + stp].i = b.i * c - a.r * sn;
            }
        }
    } else if (g.tp == 3) {
        double c = cos(g.th / 2.0);
        double sn = sin(g.th / 2.0);
        for (int i = 0; i < sz; i += 2 * stp) {
            for (int j = 0; j < stp; j++) {
                struct cx a = s->arr[i + j];
                struct cx b = s->arr[i + j + stp];
                s->arr[i + j].r = a.r * c - b.r * sn;
                s->arr[i + j].i = a.i * c - b.i * sn;
                s->arr[i + j + stp].r = a.r * sn + b.r * c;
                s->arr[i + j + stp].i = a.i * sn + b.i * c;
            }
        }
    } else if (g.tp == 4) {
        double c1 = cos(-g.th / 2.0);
        double s1 = sin(-g.th / 2.0);
        double c2 = cos(g.th / 2.0);
        double s2 = sin(g.th / 2.0);
        for (int i = 0; i < sz; i += 2 * stp) {
            for (int j = 0; j < stp; j++) {
                struct cx a = s->arr[i + j];
                struct cx b = s->arr[i + j + stp];
                s->arr[i + j].r = a.r * c1 - a.i * s1;
                s->arr[i + j].i = a.r * s1 + a.i * c1;
                s->arr[i + j + stp].r = b.r * c2 - b.i * s2;
                s->arr[i + j + stp].i = b.r * s2 + b.i * c2;
            }
        }
    } else if (g.tp == 5) {
        for (int i = 0; i < sz; i++) {
            int cb = (i >> g.c) & 1;
            int tb = (i >> g.t) & 1;
            if (cb == 1 && tb == 0) {
                int f = i | (1 << g.t);
                struct cx tmp = s->arr[i];
                s->arr[i] = s->arr[f];
                s->arr[f] = tmp;
            }
        }
    }
    
    norm(s);
    
    if (exp) {
        pr_p(s);
    }
}

int get_gt(char* ln, struct gt* g) {
    char str[16];
    int t, c;
    double th;
    if (sscanf(ln, "%15s %d %d", str, &c, &t) == 3 && strcmp(str, "CX") == 0) {
        g->tp = 5; g->c = c; g->t = t; g->th = 0.0;
        return 1;
    }
    if (sscanf(ln, "%15s %d %lf", str, &t, &th) == 3) {
        if (strcmp(str, "RX") == 0) g->tp = 2;
        else if (strcmp(str, "RY") == 0) g->tp = 3;
        else if (strcmp(str, "RZ") == 0) g->tp = 4;
        else return 0;
        g->t = t; g->c = -1; g->th = th;
        return 1;
    }
    if (sscanf(ln, "%15s %d", str, &t) == 2) {
        if (strcmp(str, "X") == 0) g->tp = 0;
        else if (strcmp(str, "H") == 0) g->tp = 1;
        else return 0;
        g->t = t; g->c = -1; g->th = 0.0;
        return 1;
    }
    return 0;
}

double get_o(struct st* s) {
    if (!s) return 0.0;
    double p = 0.0;
    for (int i = 0; i < s->sz; i++) {
        int cnt = 0;
        int tmp = i;
        while (tmp > 0) {
            cnt += tmp & 1;
            tmp >>= 1;
        }
        if (cnt % 2 == 1) {
            p += s->arr[i].r * s->arr[i].r + s->arr[i].i * s->arr[i].i;
        }
    }
    return p;
}

double fwd(double* x, double* th, int nf, int exp) {
    struct st* s = make_st(nf);
    if (!s) return 0.0;
    for (int i = 0; i < nf; i++) {
        struct gt g = {3, i, -1, x[i] * PI};
        app_gt(s, g, exp);
    }
    for (int i = 0; i < nf; i++) {
        struct gt g = {3, i, -1, th[i]};
        app_gt(s, g, exp);
    }
    for (int i = 0; i < nf - 1; i++) {
        struct gt g = {5, i + 1, i, 0};
        app_gt(s, g, exp);
    }
    for (int i = 0; i < nf; i++) {
        struct gt g = {3, i, -1, th[nf + i]};
        app_gt(s, g, exp);
    }
    double p = get_o(s);
    free_st(s);
    return p;
}

void p_exp() {
    printf("\n[PQC Explanation]\n");
    printf("A Parameterized Quantum Circuit (PQC) acts as a quantum machine learning model.\n");
    printf("1. Data Encoding: Input features are mapped to quantum states via RY rotations.\n");
    printf("2. Parameter Layers: Trainable RY rotations apply continuous weights.\n");
    printf("3. Entanglement: CX gates correlate qubits to capture feature interactions.\n");
    printf("4. Measurement: The probability of measuring odd parity represents the classification target.\n");
    printf("Parameters are iteratively optimized using mean squared error and gradient descent.\n\n");
}

void trn() {
    p_exp();
    int nf = 2;
    int np = 2 * nf;
    double th[4];
    for (int i = 0; i < np; i++) {
        th[i] = ((double)rand() / RAND_MAX) * PI;
    }
    double x[4][2] = {{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};
    double y[4] = {0.0, 1.0, 1.0, 0.0};
    int ep = 200;
    double lr = 0.5;
    double eps = 1e-4;
    
    printf("Training XOR problem using %d features.\n", nf);
    for (int e = 1; e <= ep; e++) {
        double err = 0.0;
        double gr[4] = {0};
        for (int k = 0; k < 4; k++) {
            double p = fwd(x[k], th, nf, 0);
            double dif = p - y[k];
            err += dif * dif;
            for (int i = 0; i < np; i++) {
                double tmp = th[i];
                th[i] = tmp + eps;
                double p1 = fwd(x[k], th, nf, 0);
                th[i] = tmp - eps;
                double p2 = fwd(x[k], th, nf, 0);
                th[i] = tmp;
                double d = (p1 - p2) / (2.0 * eps);
                gr[i] += 2.0 * dif * d;
            }
        }
        err /= 4.0;
        for (int i = 0; i < np; i++) {
            th[i] -= lr * (gr[i] / 4.0);
        }
        if (e % 50 == 0 || e == 1) {
            printf("Epoch %03d, Error: %.6f\n", e, err);
        }
    }
    printf("\nPredictions:\n");
    for (int k = 0; k < 4; k++) {
        double p = fwd(x[k], th, nf, 0);
        printf("Input [ %.1f, %.1f ] -> Pred: %.4f (Expected: %.1f)\n", x[k][0], x[k][1], p, y[k]);
    }
    printf("\n");
}

void r_fl() {
    char fn[256];
    printf("Enter filename: ");
    if (!fgets(fn, sizeof(fn), stdin)) return;
    fn[strcspn(fn, "\n")] = 0;
    
    FILE* f = fopen(fn, "r");
    if (!f) {
        printf("Cannot open file.\n");
        return;
    }
    
    int n = 2;
    char ln[256];
    int v_read = 0;
    
    while (fgets(ln, sizeof(ln), f)) {
        if (ln[0] == '\n' || ln[0] == '\r') continue;
        if (sscanf(ln, "%d", &n) == 1 && n > 0 && n <= 30) {
            v_read = 1;
            break;
        } else {
            printf("Invalid qubits.\n");
            fclose(f);
            return;
        }
    }
    if (!v_read) {
        printf("No qubits found.\n");
        fclose(f);
        return;
    }
    
    struct st* s = make_st(n);
    if (!s) {
        printf("Init failed.\n");
        fclose(f);
        return;
    }
    printf("Initialized %d qubits.\n", n);
    pr_p(s);
    
    int l_no = 1;
    int c = 0;
    while (fgets(ln, sizeof(ln), f)) {
        l_no++;
        if (ln[0] == '\n' || ln[0] == '\r') continue;
        struct gt g;
        if (get_gt(ln, &g)) {
            if (chk_gt(s, g)) {
                app_gt(s, g, 1);
                c++;
            } else {
                printf("Error at %d.\n", l_no);
            }
        } else {
            printf("Format error at %d.\n", l_no);
        }
    }
    
    printf("Gates applied: %d\n", c);
    fclose(f);
    free_st(s);
}

void r_cmd() {
    char in[256];
    int n = 0;
    printf("Enter number of qubits: ");
    if (!fgets(in, sizeof(in), stdin)) return;
    if (sscanf(in, "%d", &n) != 1 || n < 1 || n > 10) {
        printf("Invalid. Using 2.\n");
        n = 2;
    }
    
    struct st* s = make_st(n);
    if (!s) {
        printf("Init failed.\n");
        return;
    }
    printf("Initialized %d qubits.\n", n);
    pr_p(s);
    
    printf("Enter gate (e.g. RY 0 1.57). DONE to exit.\n");
    int c = 0;
    while (1) {
        printf("> ");
        if (!fgets(in, sizeof(in), stdin)) break;
        if (strncmp(in, "DONE", 4) == 0 || strncmp(in, "done", 4) == 0) break;
        if (in[0] == '\n' || in[0] == '\r') continue;
        
        struct gt g;
        if (get_gt(in, &g)) {
            if (chk_gt(s, g)) {
                app_gt(s, g, 1);
                c++;
            } else {
                printf("Invalid gate index.\n");
            }
        } else {
            printf("Invalid input.\n");
        }
    }
    
    printf("Gates applied: %d\n", c);
    free_st(s);
}

int main() {
    while (1) {
        printf("1. Interactive\n");
        printf("2. File\n");
        printf("3. Train\n");
        printf("4. Exit\n");
        printf("Choice: ");
        
        char in[256];
        if (!fgets(in, sizeof(in), stdin)) break;
        
        int ch = 0;
        if (sscanf(in, "%d", &ch) != 1) {
            printf("Invalid Choice\n");
            continue;
        }
        
        if (ch == 1) {
            r_cmd();
        } else if (ch == 2) {
            r_fl();
        } else if (ch == 3) {
            trn();
        } else if (ch == 4) {
            break;
        } else {
            printf("Invalid Choice\n");
        }
    }
    return 0;
}
