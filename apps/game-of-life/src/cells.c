#include "cells.h"

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static CellsResult cells_error(CellsError *error, CellsResult code,
                               size_t line, size_t column, const char *format, ...) {
    if (error) {
        memset(error, 0, sizeof(*error));
        error->code = code;
        error->line = line;
        error->column = column;
        va_list args;
        va_start(args, format);
        vsnprintf(error->message, sizeof(error->message), format, args);
        va_end(args);
    }
    return code;
}

void cells_pattern_destroy(CellsPattern *pattern) {
    if (!pattern) return;
    free(pattern->cells);
    memset(pattern, 0, sizeof(*pattern));
}

void cells_format_error(const char *path, const CellsError *error, char *out, size_t size) {
    if (!out || !size) return;
    const char *kind = "Load error";
    if (error) {
        switch (error->code) {
            case CELLS_INVALID_FORMAT: kind = "Invalid .cells format"; break;
            case CELLS_EMPTY: kind = "Empty pattern"; break;
            case CELLS_IO_ERROR: kind = "I/O error"; break;
            case CELLS_TOO_LARGE: kind = "Pattern too large"; break;
            case CELLS_NO_MEMORY: kind = "Out of memory"; break;
            case CELLS_INVALID_ARGUMENT: kind = "Invalid load arguments"; break;
            case CELLS_OK: break;
        }
    }
    char location[80] = "";
    if (error && error->line) {
        snprintf(location, sizeof(location), " (line %zu, column %zu)", error->line, error->column);
    }
    char detail[160];
    snprintf(detail, sizeof(detail), "%s", error && error->message[0] ? error->message : "No error detail available");
    size_t length = strlen(detail);
    if (length && detail[length - 1] == '.') detail[length - 1] = '\0';
    snprintf(out, size, "%s: %s%s; file '%s'.", kind, detail, location, path ? path : "");
    for (char *p = out; *p; ++p) {
        if ((unsigned char)*p < 32 || (unsigned char)*p >= 127) *p = '?';
    }
}

CellsResult cells_parse(FILE *stream, CellsPattern *out, CellsError *error) {
    if (error) memset(error, 0, sizeof(*error));
    if (!stream || !out) {
        return cells_error(error, CELLS_INVALID_ARGUMENT, 0, 0, "Missing stream or output pattern.");
    }

    CellsPattern parsed = {0};
    size_t capacity = 0, row = 0, line = 1, column = 0;
    int min_x = INT_MAX, min_y = INT_MAX, max_x = 0, max_y = 0;
    int comment = 0, after_cr = 0, c;
    CellsResult result = CELLS_OK;

    /* Streaming avoids line-buffer truncation, including long comment lines. */
    while ((c = fgetc(stream)) != EOF) {
        if (after_cr && c == '\n') {
            after_cr = 0;
            continue;
        }
        after_cr = 0;
        if (c == '\n' || c == '\r') {
            if (!comment) ++row; /* A blank line is a dead pattern row. */
            ++line;
            column = 0;
            comment = 0;
            after_cr = c == '\r';
            continue;
        }
        if (c == 0) {
            result = cells_error(error, CELLS_INVALID_FORMAT, line, column + 1,
                                 "NUL byte in .cells input.");
            goto cleanup;
        }
        if (column == 0 && c == '!') comment = 1;
        if (!comment) {
            if (c != '.' && c != 'O') {
                result = cells_error(error, CELLS_INVALID_FORMAT, line, column + 1,
                                     "Invalid byte 0x%02X; expected '.' or 'O'.", (unsigned)c);
                goto cleanup;
            }
            if (column >= INT_MAX || row >= INT_MAX) {
                result = cells_error(error, CELLS_TOO_LARGE, line, column + 1,
                                     "Pattern coordinates exceed the integer range.");
                goto cleanup;
            }
            if (c == 'O') {
                if (parsed.count == capacity) {
                    const size_t limit = SIZE_MAX / sizeof(*parsed.cells);
                    if (capacity == limit) {
                        result = cells_error(error, CELLS_TOO_LARGE, line, column + 1,
                                             "Too many live cells.");
                        goto cleanup;
                    }
                    size_t next = capacity ? (capacity > limit / 2 ? limit : capacity * 2) : 64;
                    CellsOffset *cells = realloc(parsed.cells, next * sizeof(*cells));
                    if (!cells) {
                        result = cells_error(error, CELLS_NO_MEMORY, line, column + 1,
                                             "Cannot allocate pattern cells.");
                        goto cleanup;
                    }
                    parsed.cells = cells;
                    capacity = next;
                }
                int x = (int)column, y = (int)row;
                parsed.cells[parsed.count++] = (CellsOffset){x, y};
                if (x < min_x) min_x = x;
                if (y < min_y) min_y = y;
                if (x > max_x) max_x = x;
                if (y > max_y) max_y = y;
            }
        }
        ++column;
    }
    if (ferror(stream)) {
        result = cells_error(error, CELLS_IO_ERROR, line, column + 1, "Cannot read .cells input.");
        goto cleanup;
    }
    if (!parsed.count) {
        result = cells_error(error, CELLS_EMPTY, 0, 0, "Pattern contains no live cells.");
        goto cleanup;
    }
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
    return result;
}

CellsResult cells_load(const char *path, CellsPattern *out, CellsError *error) {
    if (error) memset(error, 0, sizeof(*error));
    if (!path || !path[0] || !out) {
        return cells_error(error, CELLS_INVALID_ARGUMENT, 0, 0, "Missing path or output pattern.");
    }
    FILE *stream = fopen(path, "rb");
    if (!stream) return cells_error(error, CELLS_IO_ERROR, 0, 0, "Cannot open pattern: %s", strerror(errno));
    CellsPattern parsed = {0};
    CellsResult result = cells_parse(stream, &parsed, error);
    int close_result = fclose(stream);
    if (result == CELLS_OK && close_result != 0) {
        result = cells_error(error, CELLS_IO_ERROR, 0, 0, "Cannot close pattern: %s", strerror(errno));
    }
    if (result == CELLS_OK) {
        cells_pattern_destroy(out);
        *out = parsed;
    } else {
        cells_pattern_destroy(&parsed);
    }
    return result;
}
