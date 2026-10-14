#define _POSIX_C_SOURCE 200809L
#include "rle.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int allocation_call, fail_allocation, fail_close, fail_read, read_error;
void *rle_test_realloc(void *ptr, size_t size) {
    ++allocation_call;
    if (fail_allocation && allocation_call == fail_allocation) return NULL;
    return realloc(ptr, size);
}
int rle_test_fclose(FILE *stream) {
    int result = fclose(stream);
    if (fail_close) { errno = EIO; return EOF; }
    return result;
}
int rle_test_fgetc(FILE *stream) {
    if (fail_read > 0 && --fail_read == 0) { read_error = 1; errno = EIO; return EOF; }
    return fgetc(stream);
}
int rle_test_ferror(FILE *stream) {
    return read_error || ferror(stream);
}

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s: failed: %s\n", __FILE__, __LINE__, __func__, #condition); \
        goto cleanup; \
    } \
} while (0)

static CellsResult parse(const char *text, size_t size, CellsPattern *out, CellsError *error) {
    FILE *stream = tmpfile();
    if (!stream) return CELLS_IO_ERROR;
    if (fwrite(text, 1, size, stream) != size || fseek(stream, 0, SEEK_SET)) {
        fclose(stream); return CELLS_IO_ERROR;
    }
    errno = 0;
    CellsResult result = rle_parse(stream, out, error);
    int seek = fseek(stream, 0, SEEK_SET), close = fclose(stream);
    return seek || close ? CELLS_IO_ERROR : result;
}
static CellsResult parse_text(const char *text, CellsPattern *out, CellsError *error) {
    return parse(text, strlen(text), out, error);
}

static int rle_valid(void) {
    CellsPattern pattern = {0};
    World world = {0};
    CellsError error;
    int passed = 0;
    const char *versions[] = {
        "x = 3, y = 3\nbo$2bo$3o!",
        "#N Glider\r\n#C original fixture\r\nx=3,y=3,rule=B3/S23\r\nbo$\r\n2bo$3o!\r\n",
        "\nx=3, y=3, rule=23/3\rbo$2bo$3o$!\r#C trailing comment\r",
        "x=3,y=3,rule=s32/b3\n#C body comment\nbo$2bo$3o!",
        "x=3,y=3,rule=B3/S32\nbo$2bo$3o!",
        "x=3,y=3,rule=S23/B3\nbo$2bo$3o!",
        "x=3,y=3,rule=32/3\nbo$2bo$3o!",
    };
    const PatternCell expected[] = {{1,0},{2,1},{0,2},{1,2},{2,2}};
    for (size_t v = 0; v < sizeof(versions)/sizeof(versions[0]); ++v) {
        memset(&error, 0xff, sizeof(error));
        CHECK(parse_text(versions[v], &pattern, &error) == CELLS_OK);
        CHECK(error.code == CELLS_OK && !error.line && !error.column && !error.message[0]);
        CHECK(pattern.width == 3 && pattern.height == 3 && pattern.count == 5);
        CHECK(memcmp(pattern.cells, expected, sizeof(expected)) == 0);
    }
    CHECK(parse_text("x=10,y=8\n2$3b2o2$5bo!", &pattern, NULL) == CELLS_OK);
    CHECK(pattern.width == 3 && pattern.height == 3 && pattern.count == 3);
    CHECK(pattern.cells[0].dx == 0 && pattern.cells[0].dy == 0);
    CHECK(pattern.cells[2].dx == 2 && pattern.cells[2].dy == 2);
    CHECK(world_init(&world, 8, 7, 1) == 0);
    Pattern view = {"rle", pattern.cells, pattern.count, pattern.width, pattern.height};
    CHECK(pattern_apply(&world, &view, NULL) == PATTERN_PLACE_OK);
    CHECK(world_get(&world, 2, 2) && world_get(&world, 3, 2) && world_get(&world, 4, 4));
    char huge[128];
    snprintf(huge, sizeof(huge), "x=%d,y=%d\n%db%do!", INT_MAX, INT_MAX, INT_MAX-1, 1);
    CHECK(parse_text(huge, &pattern, NULL) == CELLS_OK);
    CHECK(pattern.width == 1 && pattern.height == 1 && pattern.count == 1 && !pattern.cells[0].dx);
    char long_comment[5050];
    memset(long_comment, 'c', 5001); long_comment[0] = '#';
    strcpy(long_comment + 5001, "\nx=1,y=1\no!");
    CHECK(parse_text(long_comment, &pattern, NULL) == CELLS_OK);
    cells_pattern_destroy(&pattern);
    cells_pattern_destroy(&pattern);
    CHECK(!pattern.cells && !pattern.count);
    passed = 1;
cleanup:
    world_destroy(&world);
    cells_pattern_destroy(&pattern);
    return passed;
}

static int rle_invalid(void) {
    CellsPattern pattern = {0};
    CellsError error;
    int passed = 0;
    CHECK(parse_text("x=1,y=1\no!", &pattern, NULL) == CELLS_OK);
    CellsOffset *original = pattern.cells;
    const char *invalid[] = {"", "#C only comment", "o!", "y=1,x=1\no!", "x=0,y=1\n!",
        "x=-1,y=1\no!", "x=1,y=1,z=1\no!", "x=1,y=1,rule=B36/S23\no!",
        "x=1,y=1,rule=\no!", "x=1,y=1,rule=B3 / S23\no!", "x=1,y=1 o!",
        "x=1,y=1\no", "x=1,y=1\n0o!", "x=1,y=1\n2o!", "x=1,y=1\n2b!",
        "x=1,y=1\n2$!", "x=1,y=1\no$o!", "x=1,y=1\n1!", "x=1,y=1\n1 o!",
        "x=1,y=1\n1\n1o!", "x=1,y=1\nO!", "x=1,y=1\npA!", "x=1,y=1\no!!",
        "x=1,y=1\no!junk", "x=1,y=1\no#inline\n!", "x=1,y=1\n#!\n"};
    for (size_t i = 0; i < sizeof(invalid)/sizeof(invalid[0]); ++i) {
        CHECK(parse_text(invalid[i], &pattern, &error) == CELLS_INVALID_FORMAT);
        CHECK(error.code == CELLS_INVALID_FORMAT && error.line && error.column && error.message[0]);
        CHECK(pattern.cells == original && pattern.count == 1 && pattern.width == 1);
    }
    CHECK(parse_text("x=2,y=1\n3o!", &pattern, &error) == CELLS_INVALID_FORMAT);
    CHECK(error.line == 2 && error.column == 2);
    const char nul[] = "x=1,y=1\no!\0";
    CHECK(parse(nul, sizeof(nul)-1, &pattern, &error) == CELLS_INVALID_FORMAT);
    CHECK(error.line == 2 && error.column == 3);
    CHECK(parse_text("#\xff\nx=1,y=1\no!", &pattern, &error) == CELLS_INVALID_FORMAT);
    const char *empty[] = {"x=1,y=1\n!", "x=2,y=2\n2b$2b!"};
    for (size_t i = 0; i < 2; ++i) CHECK(parse_text(empty[i], &pattern, &error) == CELLS_EMPTY);
    CHECK(parse_text("x=18446744073709551616,y=1\no!", &pattern, &error) == CELLS_TOO_LARGE);
    CHECK(parse_text("x=1,y=1\n18446744073709551616o!", &pattern, &error) == CELLS_TOO_LARGE);
    CHECK(pattern.cells == original);
    CHECK(rle_parse(NULL, &pattern, &error) == CELLS_INVALID_ARGUMENT);
    CHECK(!error.line && !error.column);
    CHECK(parse_text("x=1,y=1\no!", NULL, &error) == CELLS_INVALID_ARGUMENT);
    CHECK(rle_load(NULL, &pattern, NULL) == CELLS_INVALID_ARGUMENT);
    CHECK(rle_load("", &pattern, NULL) == CELLS_INVALID_ARGUMENT);
    CHECK(rle_load("unused", NULL, &error) == CELLS_INVALID_ARGUMENT);
    passed = 1;
cleanup:
    cells_pattern_destroy(&pattern);
    return passed;
}

static int rle_allocation(void) {
    CellsPattern pattern = {0};
    CellsError error;
    int passed = 0;
    CHECK(parse_text("x=1,y=1\no!", &pattern, NULL) == CELLS_OK);
    CellsOffset *original = pattern.cells;
    for (int failure = 1; failure <= 2; ++failure) {
        allocation_call = 0; fail_allocation = failure;
        CHECK(parse_text("x=128,y=1\n128o!", &pattern, &error) == CELLS_NO_MEMORY);
        CHECK(pattern.cells == original && pattern.count == 1);
    }
    fail_allocation = 0;
    CHECK(parse_text("x=128,y=1\n128o!", &pattern, &error) == CELLS_OK);
    CHECK(pattern.count == 128 && pattern.cells[127].dx == 127);
    passed = 1;
cleanup:
    fail_allocation = 0;
    cells_pattern_destroy(&pattern);
    return passed;
}

static int rle_files(void) {
    CellsPattern pattern = {0};
    CellsError error;
    char directory[] = "/tmp/gol-rle-XXXXXX", path[128] = "";
    FILE *stream = NULL;
    int passed = 0, created = 0;
    CHECK(mkdtemp(directory) != NULL); created = 1;
    snprintf(path, sizeof(path), "%s/pattern.rle", directory);
    CHECK(rle_load(path, &pattern, &error) == CELLS_IO_ERROR);
    stream = fopen(path, "wb"); CHECK(stream);
    CHECK(fputs("x=2,y=2\n2o$2o!", stream) >= 0);
    CHECK(fclose(stream) == 0); stream = NULL;
    CHECK(rle_load(path, &pattern, &error) == CELLS_OK && pattern.count == 4);
    CellsOffset *original = pattern.cells;
    fail_close = 1;
    CHECK(rle_load(path, &pattern, &error) == CELLS_IO_ERROR && pattern.cells == original);
    fail_close = 0;
    errno = 0; fail_read = 14;
    CHECK(rle_load(path, &pattern, &error) == CELLS_IO_ERROR && pattern.cells == original);
    fail_read = read_error = 0; errno = 0;
    stream = fopen(path, "wb"); CHECK(stream);
    CHECK(fputs("x=2,y=2\n2o$garbage", stream) >= 0);
    CHECK(fclose(stream) == 0); stream = NULL;
    CHECK(rle_load(path, &pattern, &error) == CELLS_INVALID_FORMAT && pattern.cells == original);
    passed = 1;
cleanup:
    fail_read = read_error = 0; fail_close = 0; errno = 0;
    if (stream) fclose(stream);
    if (path[0]) unlink(path);
    if (created) rmdir(directory);
    cells_pattern_destroy(&pattern);
    return passed;
}

int main(int argc, char **argv) {
    const char *selection = argc == 2 ? argv[1] : "all";
    const struct { const char *name; int (*run)(void); } cases[] = {
        {"rle-valid", rle_valid}, {"rle-invalid", rle_invalid},
        {"rle-allocation", rle_allocation}, {"rle-files", rle_files},
    };
    int selected = 0, failed = 0;
    if (argc > 2) return 2;
    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
        if (strcmp(selection, "all") && strcmp(selection, cases[i].name)) continue;
        ++selected;
        if (cases[i].run()) printf("PASS %s\n", cases[i].name);
        else { ++failed; fprintf(stderr, "FAIL %s\n", cases[i].name); }
    }
    if (!selected) { fprintf(stderr, "Unknown/empty RLE case: '%s'\n", selection); return 2; }
    printf("RLE tests: %d selected, %d passed, %d failed\n", selected, selected-failed, failed);
    return failed ? 1 : 0;
}
