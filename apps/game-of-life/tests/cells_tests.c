#define _POSIX_C_SOURCE 200809L
#include "cells.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int allocation_call, fail_allocation, fail_close;

void *cells_test_realloc(void *ptr, size_t size) {
    ++allocation_call;
    if (fail_allocation && allocation_call == fail_allocation) return NULL;
    return realloc(ptr, size);
}
int cells_test_fclose(FILE *stream) {
    int result = fclose(stream);
    if (fail_close) { errno = EIO; return EOF; }
    return result;
}

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s: failed: %s\n", __FILE__, __LINE__, __func__, #condition); \
        goto cleanup; \
    } \
} while (0)

static CellsResult parse_bytes(const void *bytes, size_t length, CellsPattern *out, CellsError *error) {
    FILE *stream = tmpfile();
    if (!stream) return CELLS_IO_ERROR;
    if (fwrite(bytes, 1, length, stream) != length || fseek(stream, 0, SEEK_SET) != 0) {
        fclose(stream);
        return CELLS_IO_ERROR;
    }
    CellsResult result = cells_parse(stream, out, error);
    /* The parser must leave stream ownership with the caller. */
    int seek_result = fseek(stream, 0, SEEK_SET);
    int close_result = fclose(stream);
    if (seek_result != 0 || close_result != 0) return CELLS_IO_ERROR;
    return result;
}

static int cell_at(const CellsPattern *pattern, size_t i, int x, int y) {
    return i < pattern->count && pattern->cells[i].dx == x && pattern->cells[i].dy == y;
}

static int cells_valid(void) {
    CellsPattern pattern = {0};
    CellsError error;
    int passed = 0;
    const char *versions[] = {".O.\n..O\nOOO", "!Name: Glider\r\n!\r\n.O.\r\n..O\r\nOOO\r\n",
                              ".O.\r..O\rOOO\r"};
    for (size_t i = 0; i < sizeof(versions) / sizeof(versions[0]); ++i) {
        memset(&error, 0xFF, sizeof(error));
        CHECK(parse_bytes(versions[i], strlen(versions[i]), &pattern, &error) == CELLS_OK);
        CHECK(error.code == CELLS_OK && !error.line && !error.column && !error.message[0]);
        CHECK(pattern.width == 3 && pattern.height == 3 && pattern.count == 5);
        CHECK(cell_at(&pattern, 0, 1, 0) && cell_at(&pattern, 1, 2, 1));
        CHECK(cell_at(&pattern, 2, 0, 2) && cell_at(&pattern, 3, 1, 2) && cell_at(&pattern, 4, 2, 2));
    }
    const char padded[] = "\n!Name: custom\n.....\n..O..\n!interleaved\n\n....O\n..\n";
    CHECK(parse_bytes(padded, sizeof(padded) - 1, &pattern, NULL) == CELLS_OK);
    CHECK(pattern.width == 3 && pattern.height == 3 && pattern.count == 2);
    CHECK(cell_at(&pattern, 0, 0, 0) && cell_at(&pattern, 1, 2, 2));
    char long_row[5002];
    memset(long_row, '.', sizeof(long_row));
    long_row[0] = long_row[5000] = 'O'; long_row[5001] = '\n';
    CHECK(parse_bytes(long_row, sizeof(long_row), &pattern, &error) == CELLS_OK);
    CHECK(pattern.width == 5001 && pattern.height == 1 && pattern.count == 2);
    CHECK(cell_at(&pattern, 0, 0, 0) && cell_at(&pattern, 1, 5000, 0));
    char comment[5004];
    memset(comment, 'x', sizeof(comment));
    comment[0] = '!'; comment[5000] = '\n'; comment[5001] = 'O'; comment[5002] = '\n';
    CHECK(parse_bytes(comment, 5003, &pattern, &error) == CELLS_OK);
    CHECK(pattern.width == 1 && pattern.height == 1 && pattern.count == 1);
    cells_pattern_destroy(&pattern);
    cells_pattern_destroy(&pattern);
    CHECK(!pattern.cells && !pattern.count && !pattern.width && !pattern.height);
    passed = 1;
cleanup:
    cells_pattern_destroy(&pattern);
    return passed;
}

static int cells_invalid(void) {
    CellsPattern pattern = {0};
    CellsError error;
    int passed = 0;
    CHECK(parse_bytes("O", 1, &pattern, NULL) == CELLS_OK);
    CellsOffset *original = pattern.cells;
    const struct { const char *bytes; size_t length, line, column; } invalid[] = {
        {"o", 1, 1, 1}, {"*", 1, 1, 1}, {" O", 2, 1, 1}, {"O\t", 2, 1, 2},
        {"O!comment", 9, 1, 2}, {"OO\r\n..x", 7, 2, 3}, {"O\0.", 3, 1, 2},
        {"!\0comment", 9, 1, 2}, {"\xff", 1, 1, 1}, {"x = 3, y = 3", 12, 1, 1},
    };
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        CHECK(parse_bytes(invalid[i].bytes, invalid[i].length, &pattern, &error) == CELLS_INVALID_FORMAT);
        CHECK(error.code == CELLS_INVALID_FORMAT && error.line == invalid[i].line && error.column == invalid[i].column);
        CHECK(error.message[0]);
        CHECK(pattern.cells == original && pattern.count == 1 && cell_at(&pattern, 0, 0, 0));
    }
    const char *empty[] = {"", "!Name: empty\n!\n", "...\n\n..\n"};
    for (size_t i = 0; i < sizeof(empty) / sizeof(empty[0]); ++i) {
        CHECK(parse_bytes(empty[i], strlen(empty[i]), &pattern, &error) == CELLS_EMPTY);
        CHECK(error.code == CELLS_EMPTY && pattern.cells == original && pattern.width == 1);
    }
    CHECK(cells_parse(NULL, &pattern, &error) == CELLS_INVALID_ARGUMENT);
    CHECK(cells_load(NULL, &pattern, NULL) == CELLS_INVALID_ARGUMENT);
    CHECK(cells_load("", &pattern, &error) == CELLS_INVALID_ARGUMENT);
    passed = 1;
cleanup:
    cells_pattern_destroy(&pattern);
    return passed;
}

static int cells_allocation(void) {
    CellsPattern pattern = {0};
    CellsError error;
    char row[128];
    int passed = 0;
    memset(row, 'O', sizeof(row));
    CHECK(parse_bytes("O", 1, &pattern, NULL) == CELLS_OK);
    CellsOffset *original = pattern.cells;
    for (int failure = 1; failure <= 2; ++failure) {
        allocation_call = 0; fail_allocation = failure;
        CHECK(parse_bytes(row, sizeof(row), &pattern, &error) == CELLS_NO_MEMORY);
        CHECK(error.code == CELLS_NO_MEMORY && error.line == 1);
        CHECK(error.column == (failure == 1 ? 1u : 65u));
        char message[256];
        cells_format_error("large.cells", &error, message, sizeof(message));
        CHECK(strstr(message, "Out of memory") && strstr(message, "line 1") &&
              strstr(message, "large.cells"));
        CHECK(pattern.cells == original && pattern.count == 1 && cell_at(&pattern, 0, 0, 0));
    }
    fail_allocation = 0;
    CHECK(parse_bytes(row, sizeof(row), &pattern, &error) == CELLS_OK);
    CHECK(pattern.count == 128 && pattern.width == 128 && pattern.height == 1);
    CHECK(cell_at(&pattern, 127, 127, 0));
    passed = 1;
cleanup:
    fail_allocation = 0;
    cells_pattern_destroy(&pattern);
    return passed;
}

static int cells_files(void) {
    CellsPattern pattern = {0};
    CellsError error;
    char directory[] = "/tmp/gol-cells-XXXXXX", path[128] = "", missing[128];
    FILE *file = NULL;
    int created = 0, passed = 0;
    CHECK(mkdtemp(directory) != NULL);
    created = 1;
    snprintf(path, sizeof(path), "%s/pattern.cells", directory);
    snprintf(missing, sizeof(missing), "%s/missing.cells", directory);
    CHECK(parse_bytes("O", 1, &pattern, NULL) == CELLS_OK);
    CellsOffset *original = pattern.cells;
    CHECK(cells_load(missing, &pattern, &error) == CELLS_IO_ERROR);
    CHECK(error.message[0] && pattern.cells == original);
    file = fopen(path, "wb"); CHECK(file != NULL);
    CHECK(fputs("!fixture\n.O\nOO", file) >= 0);
    CHECK(fclose(file) == 0); file = NULL;
    fail_close = 1;
    CHECK(cells_load(path, &pattern, &error) == CELLS_IO_ERROR);
    CHECK(pattern.cells == original && cell_at(&pattern, 0, 0, 0));
    fail_close = 0;
    CHECK(cells_load(path, &pattern, &error) == CELLS_OK);
    CHECK(pattern.width == 2 && pattern.height == 2 && pattern.count == 3);
    CHECK(cell_at(&pattern, 0, 1, 0) && cell_at(&pattern, 1, 0, 1) && cell_at(&pattern, 2, 1, 1));
    original = pattern.cells;
    file = fopen(path, "wb"); CHECK(file != NULL);
    /* Reading a write-only stream must propagate its I/O error. */
    CHECK(ungetc('O', file) != EOF); /* Fail after some valid input was parsed. */
    CHECK(cells_parse(file, &pattern, &error) == CELLS_IO_ERROR);
    CHECK(ferror(file) && pattern.cells == original);
    char message[256];
    cells_format_error(path, &error, message, sizeof(message));
    CHECK(strstr(message, "I/O error") && strstr(message, "Cannot read") &&
          strstr(message, "pattern.cells"));
    CHECK(fclose(file) == 0); file = NULL;
    file = fopen(path, "wb"); CHECK(file != NULL);
    CHECK(fputs("O\ninvalid", file) >= 0);
    CHECK(fclose(file) == 0); file = NULL;
    CHECK(cells_load(path, &pattern, &error) == CELLS_INVALID_FORMAT);
    CHECK(error.line == 2 && error.column == 1 && pattern.cells == original);
    passed = 1;
cleanup:
    fail_close = 0;
    if (file) fclose(file);
    if (path[0]) unlink(path);
    if (created) rmdir(directory);
    cells_pattern_destroy(&pattern);
    return passed;
}

int main(int argc, char **argv) {
    const char *selection = argc == 2 ? argv[1] : "all";
    const struct { const char *name; int (*run)(void); } cases[] = {
        {"cells-valid", cells_valid}, {"cells-invalid", cells_invalid},
        {"cells-allocation", cells_allocation}, {"cells-files", cells_files},
    };
    int selected = 0, failed = 0;
    if (argc > 2) return 2;
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        if (strcmp(selection, "all") && strcmp(selection, cases[i].name)) continue;
        ++selected;
        if (cases[i].run()) printf("PASS %s\n", cases[i].name);
        else {
            ++failed;
            fprintf(stderr, "FAIL %s; reproduce: make test-cells TEST_CASE=%s\n", cases[i].name, cases[i].name);
        }
    }
    if (!selected) {
        fprintf(stderr, "Unknown/empty .cells case: '%s'\n", selection);
        return 2;
    }
    printf("Cells tests: %d selected, %d passed, %d failed\n", selected, selected - failed, failed);
    return failed ? 1 : 0;
}
