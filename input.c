#include "input.h"

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef PQC_TESTING
static size_t read_attempts = 0;
static size_t fail_read_on = 0;

static bool read_should_fail(void)
{
    read_attempts++;
    return fail_read_on > 0 && read_attempts == fail_read_on;
}
#endif

static void attach_context(Diagnostic *d, const char *filename, size_t line)
{
    if (d != NULL) {
        d->file = filename;
        d->line = line;
    }
}

static size_t split_tokens(char *text, char *tokens[], size_t maximum)
{
    size_t count = 0;
    char *current = text;

    while (*current != '\0') {
        while (*current == ' ' || *current == '\t') {
            current++;
        }
        if (*current == '\0') {
            break;
        }
        if (count == maximum) {
            return maximum + 1;
        }
        tokens[count++] = current;
        while (*current != '\0' && *current != ' ' && *current != '\t') {
            current++;
        }
        if (*current != '\0') {
            *current = '\0';
            current++;
        }
    }
    return count;
}

static bool integer_syntax(const char *text)
{
    size_t i = 0;

    if (text[0] == '+' || text[0] == '-') {
        i++;
    }
    if (text[i] == '\0') {
        return false;
    }
    for (; text[i] != '\0'; i++) {
        if (!isdigit((unsigned char)text[i])) {
            return false;
        }
    }
    return true;
}

static PqcStatus parse_index(const char *text, size_t maximum,
                             size_t *out, Diagnostic *d)
{
    char *end;
    intmax_t value;

    if (!integer_syntax(text)) {
        pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                     "invalid integer token '%s'", text);
        return PQC_E_FORMAT;
    }
    errno = 0;
    value = strtoimax(text, &end, 10);
    if (errno == ERANGE || *end != '\0' || value < 0) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "integer token '%s' is out of range", text);
        return PQC_E_RANGE;
    }
    if ((uintmax_t)value >= (uintmax_t)maximum) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "index '%s' is out of range", text);
        return PQC_E_RANGE;
    }
    *out = (size_t)value;
    return PQC_OK;
}

static bool word_equal_ignore_case(const char *text, const char *word)
{
    while (*text != '\0' && *word != '\0') {
        if (tolower((unsigned char)*text) !=
            tolower((unsigned char)*word)) {
            return false;
        }
        text++;
        word++;
    }
    return *text == '\0' && *word == '\0';
}

static bool nonfinite_word(const char *text)
{
    if (*text == '+' || *text == '-') {
        text++;
    }
    return word_equal_ignore_case(text, "nan") ||
           word_equal_ignore_case(text, "inf") ||
           word_equal_ignore_case(text, "infinity");
}

static bool angle_syntax(const char *text)
{
    size_t i = 0;
    size_t digits_before = 0;
    size_t digits_after = 0;

    if (text[i] == '+' || text[i] == '-') {
        i++;
    }
    while (isdigit((unsigned char)text[i])) {
        digits_before++;
        i++;
    }
    if (text[i] == '.') {
        i++;
        while (isdigit((unsigned char)text[i])) {
            digits_after++;
            i++;
        }
    }
    if (digits_before == 0 && digits_after == 0) {
        return false;
    }
    if (text[i] == 'e' || text[i] == 'E') {
        size_t exponent_digits = 0;

        i++;
        if (text[i] == '+' || text[i] == '-') {
            i++;
        }
        while (isdigit((unsigned char)text[i])) {
            exponent_digits++;
            i++;
        }
        if (exponent_digits == 0) {
            return false;
        }
    }
    return text[i] == '\0';
}

static PqcStatus parse_angle(const char *text, double *out, Diagnostic *d)
{
    char *end;
    double value;

    if (nonfinite_word(text)) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "angle '%s' must be finite", text);
        return PQC_E_RANGE;
    }
    if (!angle_syntax(text)) {
        pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                     "invalid angle token '%s'", text);
        return PQC_E_FORMAT;
    }
    errno = 0;
    value = strtod(text, &end);
    if (errno == ERANGE || *end != '\0' || !isfinite(value)) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "angle '%s' is out of range", text);
        return PQC_E_RANGE;
    }
    *out = value;
    return PQC_OK;
}

PqcStatus input_parse_count(const char *line, size_t length,
                            size_t maximum, size_t *out, Diagnostic *d)
{
    char copy[PQC_MAX_LINE_BYTES + 1u];
    char *tokens[2];
    size_t count;
    char *end;
    intmax_t value;

    if (line == NULL || out == NULL || maximum == 0) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "count parser argument is missing");
        return PQC_E_INTERNAL;
    }
    if (length > PQC_MAX_LINE_BYTES) {
        pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                     "line exceeds %u bytes", PQC_MAX_LINE_BYTES);
        return PQC_E_FORMAT;
    }
    memcpy(copy, line, length);
    copy[length] = '\0';
    count = split_tokens(copy, tokens, 2);
    if (count != 1 || !integer_syntax(tokens[0])) {
        pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                     "expected exactly one integer count");
        return PQC_E_FORMAT;
    }

    errno = 0;
    value = strtoimax(tokens[0], &end, 10);
    if (errno == ERANGE || *end != '\0') {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "count is outside the integer range");
        return PQC_E_RANGE;
    }
    if (value <= 0) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "count must be positive");
        return PQC_E_RANGE;
    }
    if ((uintmax_t)value > (uintmax_t)maximum) {
        pqc_diag_set(d, PQC_E_LIMIT, NULL, 0,
                     "count exceeds %" PRIuMAX, (uintmax_t)maximum);
        return PQC_E_LIMIT;
    }
    *out = (size_t)value;
    return PQC_OK;
}

PqcStatus input_parse_gate(const char *line, size_t length, size_t qubits,
                           Gate *out, bool *present, Diagnostic *d)
{
    char copy[PQC_MAX_LINE_BYTES + 1u];
    char *tokens[4];
    size_t count;
    size_t first;
    size_t second;
    double angle;
    Gate gate;
    PqcStatus status;

    if (line == NULL || out == NULL || present == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "gate parser argument is missing");
        return PQC_E_INTERNAL;
    }
    *present = false;
    if (length > PQC_MAX_LINE_BYTES) {
        pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                     "line exceeds %u bytes", PQC_MAX_LINE_BYTES);
        return PQC_E_FORMAT;
    }
    memcpy(copy, line, length);
    copy[length] = '\0';
    count = split_tokens(copy, tokens, 4);
    if (count == 0) {
        return PQC_OK;
    }

    if (strcmp(tokens[0], "X") == 0 || strcmp(tokens[0], "H") == 0) {
        if (count != 2) {
            pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                         "%s gate requires one operand", tokens[0]);
            return PQC_E_FORMAT;
        }
        status = parse_index(tokens[1], qubits, &first, d);
        if (status != PQC_OK) {
            return status;
        }
        gate = (Gate){
            .kind = strcmp(tokens[0], "X") == 0 ? GATE_X : GATE_H,
            .target = (int)first,
            .control = -1,
            .angle = 0.0
        };
    } else if (strcmp(tokens[0], "RX") == 0 ||
               strcmp(tokens[0], "RY") == 0 ||
               strcmp(tokens[0], "RZ") == 0) {
        if (count != 3) {
            pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                         "%s gate requires a target and angle", tokens[0]);
            return PQC_E_FORMAT;
        }
        status = parse_index(tokens[1], qubits, &first, d);
        if (status != PQC_OK) {
            return status;
        }
        status = parse_angle(tokens[2], &angle, d);
        if (status != PQC_OK) {
            return status;
        }
        gate = (Gate){
            .kind = strcmp(tokens[0], "RX") == 0 ? GATE_RX :
                    (strcmp(tokens[0], "RY") == 0 ? GATE_RY : GATE_RZ),
            .target = (int)first,
            .control = -1,
            .angle = angle
        };
    } else if (strcmp(tokens[0], "CX") == 0) {
        if (count != 3) {
            pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                         "CX gate requires a control and target");
            return PQC_E_FORMAT;
        }
        status = parse_index(tokens[1], qubits, &first, d);
        if (status != PQC_OK) {
            return status;
        }
        status = parse_index(tokens[2], qubits, &second, d);
        if (status != PQC_OK) {
            return status;
        }
        gate = (Gate){
            .kind = GATE_CX,
            .target = (int)second,
            .control = (int)first,
            .angle = 0.0
        };
    } else {
        pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                     "unsupported gate '%s'", tokens[0]);
        return PQC_E_FORMAT;
    }

    status = gate_validate(&gate, qubits, d);
    if (status != PQC_OK) {
        return status;
    }
    *out = gate;
    *present = true;
    return PQC_OK;
}

static PqcStatus read_complete_file(const char *filename,
                                    unsigned char **out, size_t *length,
                                    Diagnostic *d)
{
    FILE *file;
    unsigned char *buffer;
    size_t bytes;
    bool injected_failure = false;

    if (filename == NULL || out == NULL || length == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, filename, 0,
                     "file reader argument is missing");
        return PQC_E_INTERNAL;
    }
    file = fopen(filename, "rb");
    if (file == NULL) {
        pqc_diag_set(d, PQC_E_IO, filename, 0,
                     "could not open input file");
        return PQC_E_IO;
    }
    buffer = pqc_malloc(PQC_MAX_FILE_BYTES + 1u);
    if (buffer == NULL) {
        (void)fclose(file);
        pqc_diag_set(d, PQC_E_ALLOC, filename, 0,
                     "could not allocate input buffer");
        return PQC_E_ALLOC;
    }

#ifdef PQC_TESTING
    if (read_should_fail()) {
        injected_failure = true;
        bytes = 0;
    } else
#endif
    {
        bytes = fread(buffer, 1, PQC_MAX_FILE_BYTES + 1u, file);
    }
    if (injected_failure || ferror(file)) {
        pqc_free(buffer);
        (void)fclose(file);
        pqc_diag_set(d, PQC_E_IO, filename, 0,
                     "could not read input file");
        return PQC_E_IO;
    }
    if (bytes > PQC_MAX_FILE_BYTES) {
        pqc_free(buffer);
        (void)fclose(file);
        pqc_diag_set(d, PQC_E_LIMIT, filename, 0,
                     "input file exceeds %u bytes", PQC_MAX_FILE_BYTES);
        return PQC_E_LIMIT;
    }
    if (fclose(file) != 0) {
        pqc_free(buffer);
        pqc_diag_set(d, PQC_E_IO, filename, 0,
                     "could not close input file");
        return PQC_E_IO;
    }

    *out = buffer;
    *length = bytes;
    return PQC_OK;
}

static PqcStatus next_file_line(const unsigned char *buffer, size_t bytes,
                                size_t *position, const char **line,
                                size_t *length, Diagnostic *d)
{
    size_t start = *position;
    size_t current = start;

    while (current < bytes && buffer[current] != '\n' &&
           buffer[current] != '\r') {
        unsigned char value = buffer[current];

        if (value == '\0' || value > 127 ||
            (value < 32 && value != '\t')) {
            pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                         "unsupported byte in input");
            return PQC_E_FORMAT;
        }
        current++;
    }
    if (current - start > PQC_MAX_LINE_BYTES) {
        pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                     "line exceeds %u bytes", PQC_MAX_LINE_BYTES);
        return PQC_E_FORMAT;
    }
    *line = (const char *)(buffer + start);
    *length = current - start;

    if (current < bytes && buffer[current] == '\r') {
        if (current + 1 >= bytes || buffer[current + 1] != '\n') {
            pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                         "bare carriage return is not supported");
            return PQC_E_FORMAT;
        }
        current += 2;
    } else if (current < bytes) {
        current++;
    }
    *position = current;
    return PQC_OK;
}

PqcStatus input_load_circuit(const char *filename, Circuit *out,
                             Diagnostic *d)
{
    unsigned char *buffer = NULL;
    size_t bytes = 0;
    size_t position = 0;
    size_t physical_line = 0;
    bool header_found = false;
    Circuit temporary = {0};
    PqcStatus status;

    if (out == NULL || out->logical_qubits != 0 || out->count != 0 ||
        out->capacity != 0 || out->operations != NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, filename, 0,
                     "circuit output must be empty");
        return PQC_E_INTERNAL;
    }
    status = read_complete_file(filename, &buffer, &bytes, d);
    if (status != PQC_OK) {
        return status;
    }
    if (bytes >= 3 && buffer[0] == 0xef && buffer[1] == 0xbb &&
        buffer[2] == 0xbf) {
        pqc_diag_set(d, PQC_E_FORMAT, filename, 1,
                     "UTF-8 BOM is not supported");
        status = PQC_E_FORMAT;
        goto cleanup;
    }

    while (position < bytes) {
        const char *line;
        size_t length;
        Gate gate;
        bool present;

        physical_line++;
        status = next_file_line(buffer, bytes, &position, &line, &length, d);
        if (status != PQC_OK) {
            attach_context(d, filename, physical_line);
            goto cleanup;
        }
        if (!header_found) {
            size_t qubits;
            char copy[PQC_MAX_LINE_BYTES + 1u];
            char *tokens[1];

            memcpy(copy, line, length);
            copy[length] = '\0';
            if (split_tokens(copy, tokens, 1) == 0) {
                continue;
            }
            status = input_parse_count(line, length,
                                       PQC_MAX_LOGICAL_QUBITS, &qubits, d);
            if (status != PQC_OK) {
                attach_context(d, filename, physical_line);
                goto cleanup;
            }
            status = circuit_init(&temporary, qubits, d);
            if (status != PQC_OK) {
                attach_context(d, filename, physical_line);
                goto cleanup;
            }
            header_found = true;
            continue;
        }

        status = input_parse_gate(line, length, temporary.logical_qubits,
                                  &gate, &present, d);
        if (status != PQC_OK) {
            attach_context(d, filename, physical_line);
            goto cleanup;
        }
        if (present) {
            status = circuit_append(&temporary, gate, physical_line, d);
            if (status != PQC_OK) {
                attach_context(d, filename, physical_line);
                goto cleanup;
            }
        }
    }
    if (!header_found) {
        pqc_diag_set(d, PQC_E_FORMAT, filename, 0,
                     "circuit file has no qubit-count header");
        status = PQC_E_FORMAT;
        goto cleanup;
    }

    *out = temporary;
    memset(&temporary, 0, sizeof(temporary));
    status = PQC_OK;

cleanup:
    circuit_destroy(&temporary);
    pqc_free(buffer);
    return status;
}

static PqcStatus parse_edge_line(const char *line, size_t length,
                                 size_t vertices, size_t *u, size_t *v,
                                 bool *present, Diagnostic *d)
{
    char copy[PQC_MAX_LINE_BYTES + 1u];
    char *tokens[3];
    size_t count;
    PqcStatus status;

    *present = false;
    memcpy(copy, line, length);
    copy[length] = '\0';
    count = split_tokens(copy, tokens, 3);
    if (count == 0) {
        return PQC_OK;
    }
    if (count != 2) {
        pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                     "topology row requires exactly two endpoints");
        return PQC_E_FORMAT;
    }
    status = parse_index(tokens[0], vertices, u, d);
    if (status != PQC_OK) {
        return status;
    }
    status = parse_index(tokens[1], vertices, v, d);
    if (status != PQC_OK) {
        return status;
    }
    if (*u == *v) {
        pqc_diag_set(d, PQC_E_RANGE, NULL, 0,
                     "topology self-loop is not supported");
        return PQC_E_RANGE;
    }
    *present = true;
    return PQC_OK;
}

PqcStatus input_load_topology(const char *filename, Graph **out,
                              FILE *warnings, Diagnostic *d)
{
    unsigned char *buffer = NULL;
    size_t bytes = 0;
    size_t position = 0;
    size_t physical_line = 0;
    size_t edge_rows = 0;
    bool header_found = false;
    Graph *temporary = NULL;
    PqcStatus status;

    if (out == NULL || *out != NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, filename, 0,
                     "graph output must be empty");
        return PQC_E_INTERNAL;
    }
    status = read_complete_file(filename, &buffer, &bytes, d);
    if (status != PQC_OK) {
        return status;
    }
    if (bytes >= 3 && buffer[0] == 0xef && buffer[1] == 0xbb &&
        buffer[2] == 0xbf) {
        pqc_diag_set(d, PQC_E_FORMAT, filename, 1,
                     "UTF-8 BOM is not supported");
        status = PQC_E_FORMAT;
        goto cleanup;
    }

    while (position < bytes) {
        const char *line;
        size_t length;

        physical_line++;
        status = next_file_line(buffer, bytes, &position, &line, &length, d);
        if (status != PQC_OK) {
            attach_context(d, filename, physical_line);
            goto cleanup;
        }
        if (!header_found) {
            size_t vertices;
            char copy[PQC_MAX_LINE_BYTES + 1u];
            char *tokens[1];

            memcpy(copy, line, length);
            copy[length] = '\0';
            if (split_tokens(copy, tokens, 1) == 0) {
                continue;
            }
            status = input_parse_count(line, length, PQC_MAX_VERTICES,
                                       &vertices, d);
            if (status != PQC_OK) {
                attach_context(d, filename, physical_line);
                goto cleanup;
            }
            status = graph_create(vertices, &temporary, d);
            if (status != PQC_OK) {
                attach_context(d, filename, physical_line);
                goto cleanup;
            }
            header_found = true;
            continue;
        }

        {
            size_t u;
            size_t v;
            bool present;
            bool inserted;

            status = parse_edge_line(line, length,
                                     graph_vertex_count(temporary),
                                     &u, &v, &present, d);
            if (status != PQC_OK) {
                attach_context(d, filename, physical_line);
                goto cleanup;
            }
            if (!present) {
                continue;
            }
            if (edge_rows == PQC_MAX_EDGE_ROWS) {
                pqc_diag_set(d, PQC_E_LIMIT, filename, physical_line,
                             "topology exceeds %u edge rows",
                             PQC_MAX_EDGE_ROWS);
                status = PQC_E_LIMIT;
                goto cleanup;
            }
            edge_rows++;
            status = graph_add_edge(temporary, u, v, &inserted, d);
            if (status != PQC_OK) {
                attach_context(d, filename, physical_line);
                goto cleanup;
            }
            if (!inserted && warnings != NULL) {
                if (fprintf(warnings,
                            "W_DUPLICATE_EDGE file=\"%s\" line=%" PRIuMAX
                            "\n", filename, (uintmax_t)physical_line) < 0) {
                    pqc_diag_set(d, PQC_E_IO, filename, physical_line,
                                 "could not write duplicate-edge warning");
                    status = PQC_E_IO;
                    goto cleanup;
                }
            }
        }
    }
    if (!header_found) {
        pqc_diag_set(d, PQC_E_FORMAT, filename, 0,
                     "topology file has no vertex-count header");
        status = PQC_E_FORMAT;
        goto cleanup;
    }

    *out = temporary;
    temporary = NULL;
    status = PQC_OK;

cleanup:
    graph_destroy(&temporary);
    pqc_free(buffer);
    return status;
}

PqcStatus input_terminal_line(FILE *stream,
                              char out[PQC_MAX_LINE_BYTES + 1u],
                              size_t *length, bool *eof, Diagnostic *d)
{
    size_t used = 0;
    bool overlong = false;
    bool invalid_nul = false;
    bool bare_cr = false;
    int value;

    if (stream == NULL || out == NULL || length == NULL || eof == NULL) {
        pqc_diag_set(d, PQC_E_INTERNAL, NULL, 0,
                     "terminal reader argument is missing");
        return PQC_E_INTERNAL;
    }
    out[0] = '\0';
    *length = 0;
    *eof = false;

    while (true) {
#ifdef PQC_TESTING
        if (read_should_fail()) {
            pqc_diag_set(d, PQC_E_IO, NULL, 0,
                         "could not read terminal input");
            return PQC_E_IO;
        }
#endif
        value = fgetc(stream);
        if (value == EOF || value == '\n') {
            break;
        }
        if (value == '\r') {
#ifdef PQC_TESTING
            if (read_should_fail()) {
                pqc_diag_set(d, PQC_E_IO, NULL, 0,
                             "could not read terminal input");
                return PQC_E_IO;
            }
#endif
            value = fgetc(stream);
            if (value == '\n') {
                break;
            }
            bare_cr = true;
            if (value == EOF) {
                break;
            }
        }
        if (value == '\0') {
            invalid_nul = true;
        }
        if (used < PQC_MAX_LINE_BYTES) {
            out[used++] = (char)value;
        } else {
            overlong = true;
        }
    }

    if (value == EOF && ferror(stream)) {
        pqc_diag_set(d, PQC_E_IO, NULL, 0,
                     "could not read terminal input");
        return PQC_E_IO;
    }
    if (value == EOF && used == 0 && !overlong && !invalid_nul && !bare_cr) {
        *eof = true;
        return PQC_OK;
    }
    if (overlong || invalid_nul || bare_cr) {
        out[0] = '\0';
        pqc_diag_set(d, PQC_E_FORMAT, NULL, 0,
                     overlong ? "terminal line exceeds %u bytes" :
                     (invalid_nul ? "terminal line contains NUL" :
                      "bare carriage return is not supported"),
                     PQC_MAX_LINE_BYTES);
        return PQC_E_FORMAT;
    }
    out[used] = '\0';
    *length = used;
    return PQC_OK;
}

#ifdef PQC_TESTING
void input_test_fail_read_on(size_t attempt)
{
    fail_read_on = attempt;
    read_attempts = 0;
}
#endif
