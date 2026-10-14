#include "rle.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    FILE *stream;
    CellsError *error;
    CellsResult result;
    int c, after_cr;
    size_t line, column;
} Reader;

static int fail(Reader *r, CellsResult code, const char *message) {
    if (r->result == CELLS_OK) {
        r->result = code;
        if (r->error) {
            r->error->code = code;
            r->error->line = r->line;
            r->error->column = r->column;
            snprintf(r->error->message, sizeof(r->error->message), "%s", message);
        }
    }
    return 0;
}

static void advance(Reader *r) {
    if (r->c == '\n') { ++r->line; r->column = 1; }
    else ++r->column;
    int c = fgetc(r->stream);
    if (r->after_cr && c == '\n') c = fgetc(r->stream);
    r->after_cr = c == '\r';
    r->c = c == '\r' ? '\n' : c;
    if (c == EOF && ferror(r->stream)) fail(r, CELLS_IO_ERROR, "Cannot read .rle input.");
    else if (c == 0 || c > 127) {
        fail(r, CELLS_INVALID_FORMAT, "NUL or non-ASCII byte in .rle input.");
        r->c = EOF;
    }
}

static void horizontal_space(Reader *r) {
    while (r->c == ' ' || r->c == '\t') advance(r);
}

static void skip_lines(Reader *r) {
    while (r->c == '\n' || r->c == ' ' || r->c == '\t' || (r->c == '#' && r->column == 1)) {
        if (r->c == '#') {
            while (r->c != '\n' && r->c != EOF) advance(r);
        } else advance(r);
    }
}

static int expect(Reader *r, int c) {
    horizontal_space(r);
    if (r->c != c) return fail(r, CELLS_INVALID_FORMAT, "Malformed RLE header.");
    advance(r);
    return 1;
}

static int integer(Reader *r, int *value) {
    int n = 0;
    if (r->c < '0' || r->c > '9') return fail(r, CELLS_INVALID_FORMAT, "Expected positive decimal integer.");
    while (r->c >= '0' && r->c <= '9') {
        int digit = r->c - '0';
        if (n > (INT_MAX - digit) / 10) return fail(r, CELLS_TOO_LARGE, "RLE integer exceeds INT_MAX.");
        n = n * 10 + digit;
        advance(r);
    }
    if (!n) return fail(r, CELLS_INVALID_FORMAT, "RLE dimensions and run counts must be positive.");
    *value = n;
    return 1;
}

static int header(Reader *r, int *width, int *height) {
    skip_lines(r);
    if (!expect(r, 'x') || !expect(r, '=')) return 0;
    horizontal_space(r);
    if (!integer(r, width) || !expect(r, ',') || !expect(r, 'y') || !expect(r, '=')) return 0;
    horizontal_space(r);
    if (!integer(r, height)) return 0;
    horizontal_space(r);
    if (r->c == ',') {
        advance(r);
        horizontal_space(r);
        const char *key = "rule";
        for (; *key; ++key) {
            if (r->c != *key) return fail(r, CELLS_INVALID_FORMAT, "Expected rule field.");
            advance(r);
        }
        if (!expect(r, '=')) return 0;
        horizontal_space(r);
        char rule[16];
        size_t n = 0;
        while (r->c != EOF && r->c != '\n' && r->c != ' ' && r->c != '\t') {
            if (n == sizeof(rule) - 1) return fail(r, CELLS_INVALID_FORMAT, "Unsupported RLE rule; expected Conway Life.");
            rule[n++] = (char)toupper((unsigned char)r->c);
            advance(r);
        }
        rule[n] = '\0';
        if (strcmp(rule, "B3/S23") && strcmp(rule, "B3/S32") &&
            strcmp(rule, "S23/B3") && strcmp(rule, "S32/B3") &&
            strcmp(rule, "23/3") && strcmp(rule, "32/3")) {
            return fail(r, CELLS_INVALID_FORMAT, "Unsupported RLE rule; expected Conway Life.");
        }
        horizontal_space(r);
    }
    if (r->c != '\n') return fail(r, CELLS_INVALID_FORMAT, "RLE header must end before pattern data.");
    advance(r);
    return r->result == CELLS_OK;
}

CellsResult rle_parse(FILE *stream, CellsPattern *out, CellsError *error) {
    if (error) memset(error, 0, sizeof(*error));
    Reader r = {.stream = stream, .error = error, .c = EOF};
    if (!stream || !out) {
        fail(&r, CELLS_INVALID_ARGUMENT, "Missing stream or output pattern.");
        return r.result;
    }
    r.line = 1;
    CellsPattern parsed = {0};
    size_t capacity = 0;
    int width, height, x = 0, y = 0, min_x = INT_MAX, min_y = INT_MAX, max_x = 0, max_y = 0;
    advance(&r);
    if (!header(&r, &width, &height)) goto cleanup;
    while (r.result == CELLS_OK) {
        skip_lines(&r);
        if (r.c == '!') {
            advance(&r);
            skip_lines(&r);
            if (r.c != EOF) { fail(&r, CELLS_INVALID_FORMAT, "Unexpected data after RLE terminator."); goto cleanup; }
            if (r.result != CELLS_OK) goto cleanup;
            break;
        }
        if (r.c == EOF) { fail(&r, CELLS_INVALID_FORMAT, "Missing RLE terminator '!'."); goto cleanup; }
        int run = 1;
        if (r.c >= '0' && r.c <= '9' && !integer(&r, &run)) goto cleanup;
        int tag = r.c;
        if (tag != 'b' && tag != 'o' && tag != '$') {
            fail(&r, CELLS_INVALID_FORMAT, "Expected RLE tag 'b', 'o' or '$'."); goto cleanup;
        }
        if (tag == '$') {
            /* Advancing to exactly height permits the common final '$!'. */
            if (run > height - y) { fail(&r, CELLS_INVALID_FORMAT, "Row advance exceeds declared height."); goto cleanup; }
            y += run;
            x = 0;
        } else {
            if (y >= height || run > width - x) { fail(&r, CELLS_INVALID_FORMAT, "Cell run exceeds declared dimensions."); goto cleanup; }
            if (tag == 'o') {
                size_t limit = SIZE_MAX / sizeof(*parsed.cells);
                if ((size_t)run > limit - parsed.count) { fail(&r, CELLS_TOO_LARGE, "Too many live cells."); goto cleanup; }
                /* Grow by observed cells rather than allocating an untrusted huge run. */
                for (int i = 0; i < run; ++i) {
                    if (parsed.count == capacity) {
                        if (capacity == limit) { fail(&r, CELLS_TOO_LARGE, "Too many live cells."); goto cleanup; }
                        size_t next = capacity ? (capacity > limit / 2 ? limit : capacity * 2) : 64;
                        CellsOffset *cells = realloc(parsed.cells, next * sizeof(*cells));
                        if (!cells) { fail(&r, CELLS_NO_MEMORY, "Cannot allocate pattern cells."); goto cleanup; }
                        parsed.cells = cells;
                        capacity = next;
                    }
                    parsed.cells[parsed.count++] = (CellsOffset){x + i, y};
                }
                if (x < min_x) min_x = x;
                if (y < min_y) min_y = y;
                if (x + run - 1 > max_x) max_x = x + run - 1;
                if (y > max_y) max_y = y;
            }
            x += run;
        }
        advance(&r);
    }
    if (r.result != CELLS_OK) goto cleanup;
    if (!parsed.count) { fail(&r, CELLS_EMPTY, "Pattern contains no live cells."); goto cleanup; }
    parsed.width = max_x - min_x + 1;
    parsed.height = max_y - min_y + 1;
    for (size_t i = 0; i < parsed.count; ++i) {
        parsed.cells[i].dx -= min_x;
        parsed.cells[i].dy -= min_y;
    }
    cells_pattern_destroy(out);
    *out = parsed;
    return CELLS_OK;
cleanup:
    cells_pattern_destroy(&parsed);
    return r.result;
}

CellsResult rle_load(const char *path, CellsPattern *out, CellsError *error) {
    if (error) memset(error, 0, sizeof(*error));
    Reader r = {.error = error};
    if (!path || !*path || !out) {
        fail(&r, CELLS_INVALID_ARGUMENT, "Missing path or output pattern.");
        return r.result;
    }
    FILE *stream = fopen(path, "rb");
    if (!stream) {
        char message[160];
        snprintf(message, sizeof(message), "Cannot open pattern: %s", strerror(errno));
        fail(&r, CELLS_IO_ERROR, message);
        return r.result;
    }
    CellsPattern parsed = {0};
    CellsResult result = rle_parse(stream, &parsed, error);
    if (fclose(stream) != 0 && result == CELLS_OK) {
        fail(&r, CELLS_IO_ERROR, "Cannot close pattern.");
        result = r.result;
    }
    if (result == CELLS_OK) { cells_pattern_destroy(out); *out = parsed; }
    else cells_pattern_destroy(&parsed);
    return result;
}
