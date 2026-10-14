#ifndef CELLS_H
#define CELLS_H

#include "pattern.h"

#include <stddef.h>
#include <stdio.h>

typedef PatternCell CellsOffset;

typedef struct {
    CellsOffset *cells;
    size_t count;
    int width, height;
} CellsPattern;

typedef enum {
    CELLS_OK,
    CELLS_INVALID_ARGUMENT,
    CELLS_IO_ERROR,
    CELLS_INVALID_FORMAT,
    CELLS_EMPTY,
    CELLS_TOO_LARGE,
    CELLS_NO_MEMORY
} CellsResult;

typedef struct {
    CellsResult code;
    size_t line, column; /* 1-based, or 0 when not associated with input text */
    char message[160];
} CellsError;

/* Initialize patterns to {0}. Success replaces the owned output; failure
   leaves it unchanged. Coordinates are relative to the live-cell bounding box.
   Error output is optional. Stream parsing never closes the caller's stream. */
CellsResult cells_parse(FILE *stream, CellsPattern *out, CellsError *error);
CellsResult cells_load(const char *path, CellsPattern *out, CellsError *error);
void cells_pattern_destroy(CellsPattern *pattern);
/* Shared printable error detail for startup and runtime file loading. */
void cells_format_error(const char *path, const CellsError *error, char *out, size_t size);

#endif
