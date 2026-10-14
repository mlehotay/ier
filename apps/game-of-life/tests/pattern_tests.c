#include "pattern.h"
#include "cells.h"

#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

/* Fixed reference phases, not evolution tests. Sources: docs/patterns.md. */
static const struct {
    const char *name, *rows;
    int width, height;
    size_t count;
} fixtures[] = {
    {"block", "OO\nOO\n", 2, 2, 4},
    {"blinker", "OOO\n", 3, 1, 3},
    {"toad", ".OOO\nOOO.\n", 4, 2, 6},
    {"beacon", "OO..\nOO..\n..OO\n..OO\n", 4, 4, 8},
    {"glider", ".O.\n..O\nOOO\n", 3, 3, 5},
    {"lwss", ".O..O\nO....\nO...O\nOOOO.\n", 5, 4, 9},
    {"gosper",
     "........................O...........\n"
     "......................O.O...........\n"
     "............OO......OO............OO\n"
     "...........O...O....OO............OO\n"
     "OO........O.....O...OO..............\n"
     "OO........O...O.OO....O.O...........\n"
     "..........O.....O.......O...........\n"
     "...........O...O....................\n"
     "............OO......................\n", 36, 9, 36},
};

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s: failed: %s\n", __FILE__, __LINE__, __func__, #condition); \
        goto cleanup; \
    } \
} while (0)

static int pattern_registry(void) {
    int passed = 0;
    CHECK(pattern_count() == sizeof(fixtures) / sizeof(fixtures[0]));
    for (size_t i = 0; i < pattern_count(); ++i) {
        const Pattern *p = pattern_at(i);
        CHECK(p && strcmp(p->name, fixtures[i].name) == 0);
        CHECK(pattern_lookup(p->name) == p && pattern_lookup(p->name) == pattern_at(i));
        CHECK(p->cells && p->count == fixtures[i].count);
        int width = -1, height = -1;
        CHECK(pattern_dimensions(p, &width, &height) == 0);
        CHECK(width == fixtures[i].width && height == fixtures[i].height);
        CHECK(pattern_dimensions(p, NULL, &height) == 0);
        CHECK(pattern_dimensions(p, &width, NULL) == 0);
        CHECK(pattern_dimensions(p, NULL, NULL) == 0);
        int min_x = width, min_y = height, max_x = -1, max_y = -1;
        for (size_t j = 0; j < p->count; ++j) {
            int x = p->cells[j].dx, y = p->cells[j].dy;
            CHECK(x >= 0 && y >= 0 && x < width && y < height);
            if (x < min_x) min_x = x;
            if (y < min_y) min_y = y;
            if (x > max_x) max_x = x;
            if (y > max_y) max_y = y;
            for (size_t k = 0; k < j; ++k) {
                CHECK(x != p->cells[k].dx || y != p->cells[k].dy);
            }
            if (j) {
                CHECK(y > p->cells[j - 1].dy ||
                      (y == p->cells[j - 1].dy && x > p->cells[j - 1].dx));
            }
        }
        CHECK(min_x == 0 && min_y == 0 && max_x + 1 == width && max_y + 1 == height);
        for (size_t j = 0; j < i; ++j) CHECK(strcmp(p->name, pattern_at(j)->name) != 0);
    }
    CHECK(!pattern_at(pattern_count()) && !pattern_at(SIZE_MAX));
    const char *bad_names[] = {NULL, "", "unknown", "Glider", "LWSS", "glider ", "glid"};
    for (size_t i = 0; i < sizeof(bad_names) / sizeof(bad_names[0]); ++i) {
        CHECK(pattern_lookup(bad_names[i]) == NULL);
    }
    int width = 99, height = 98;
    CHECK(pattern_dimensions(NULL, &width, &height) == -1);
    CHECK(width == 99 && height == 98);
    Pattern empty = {0};
    CHECK(pattern_dimensions(&empty, &width, &height) == -1);
    CHECK(width == 99 && height == 98);
    Pattern invalid = *pattern_at(0);
    invalid.width = -1;
    CHECK(pattern_dimensions(&invalid, &width, &height) == -1);
    invalid = *pattern_at(0); invalid.height = 0;
    CHECK(pattern_dimensions(&invalid, &width, &height) == -1);
    invalid = *pattern_at(0); invalid.cells = NULL;
    CHECK(pattern_dimensions(&invalid, &width, &height) == -1);
    invalid = *pattern_at(0); invalid.count = 0;
    CHECK(pattern_dimensions(&invalid, &width, &height) == -1);
    CHECK(width == 99 && height == 98);
    passed = 1;
cleanup:
    return passed;
}

static int pattern_fixtures(void) {
    CellsPattern parsed = {0};
    CellsError error;
    FILE *stream = NULL;
    int passed = 0;
    for (size_t i = 0; i < sizeof(fixtures) / sizeof(fixtures[0]); ++i) {
        stream = tmpfile(); CHECK(stream != NULL);
        CHECK(fputs(fixtures[i].rows, stream) >= 0 && fseek(stream, 0, SEEK_SET) == 0);
        CHECK(cells_parse(stream, &parsed, &error) == CELLS_OK);
        int closed = fclose(stream); stream = NULL; CHECK(closed == 0);
        const Pattern *p = pattern_lookup(fixtures[i].name);
        CHECK(p && p->count == parsed.count && p->width == parsed.width && p->height == parsed.height);
        for (size_t j = 0; j < p->count; ++j) {
            CHECK(p->cells[j].dx == parsed.cells[j].dx && p->cells[j].dy == parsed.cells[j].dy);
        }
        /* Owned parser output can supply the same borrowed view without casts. */
        Pattern view = {NULL, parsed.cells, parsed.count, parsed.width, parsed.height};
        int width, height;
        CHECK(pattern_dimensions(&view, &width, &height) == 0);
        CHECK(width == fixtures[i].width && height == fixtures[i].height);
    }
    passed = 1;
cleanup:
    if (stream) fclose(stream);
    cells_pattern_destroy(&parsed);
    return passed;
}

static int placed_world(const World *world, const Pattern *pattern, int x, int y) {
    unsigned long expected_live = 0;
    for (int wy = 0; wy < world->h; ++wy) {
        for (int wx = 0; wx < world->w; ++wx) {
            int expected = (wx == 0 && wy == 0) || (wx == world->w - 1 && wy == world->h - 1);
            for (size_t i = 0; i < pattern->count; ++i) {
                if (wx == x + pattern->cells[i].dx && wy == y + pattern->cells[i].dy) expected = 1;
            }
            if (world_get(world, wx, wy) != expected || world->next[wy * world->w + wx] != 1) return 0;
            expected_live += (unsigned long)expected;
        }
    }
    StepStats stats;
    world_recompute_stats(world, &stats);
    return stats.live_cells == expected_live;
}

static void placement_base(World *world) {
    world_clear(world);
    world_set(world, 0, 0, 1);
    world_set(world, world->w - 1, world->h - 1, 1);
    memset(world->next, 1, (size_t)world->w * (size_t)world->h);
}

static int pattern_placement(void) {
    World world = {0};
    int passed = 0;
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        for (size_t i = 0; i < pattern_count(); ++i) {
            const Pattern *p = pattern_at(i);
            CHECK(world_init(&world, p->width + 5, p->height + 4, toroidal) == 0);
            placement_base(&world);
            CHECK(pattern_place_centered(&world, p) == PATTERN_PLACE_OK);
            CHECK(placed_world(&world, p, 2, 2));
            placement_base(&world);
            CHECK(pattern_apply(&world, p, NULL) == PATTERN_PLACE_OK);
            CHECK(placed_world(&world, p, 2, 2));
            PlacementConfig options = {0};
            placement_base(&world);
            CHECK(pattern_apply(&world, p, &options) == PATTERN_PLACE_OK);
            CHECK(placed_world(&world, p, 2, 2));
            options = (PlacementConfig){.centered = 1, .has_xy = 1, .x = 5, .y = 4};
            placement_base(&world);
            CHECK(pattern_apply(&world, p, &options) == PATTERN_PLACE_OK);
            CHECK(placed_world(&world, p, 5, 4));
            placement_base(&world);
            CHECK(pattern_place(&world, p, 0, 0) == PATTERN_PLACE_OK);
            CHECK(placed_world(&world, p, 0, 0));
            CHECK(world.w == p->width + 5 && world.h == p->height + 4 && world.toroidal == toroidal);
            world_destroy(&world);
            CHECK(world_init(&world, p->width, p->height, toroidal) == 0);
            CHECK(pattern_place_centered(&world, p) == PATTERN_PLACE_OK);
            StepStats stats; world_recompute_stats(&world, &stats);
            CHECK(stats.live_cells == p->count);
            world_destroy(&world);
        }
    }
    passed = 1;
cleanup:
    world_destroy(&world);
    return passed;
}

static int pattern_rejection(void) {
    World world = {0};
    int passed = 0;
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        CHECK(world_init(&world, 6, 5, toroidal) == 0);
        placement_base(&world);
        uint8_t before[30]; memcpy(before, world.cells, sizeof(before));
        uint8_t *cells = world.cells, *next = world.next;
        const int coords[][2] = {{5,0}, {0,4}, {-1,0}, {0,-1}, {INT_MAX,0}, {0,INT_MAX}, {INT_MIN,0}};
        for (size_t i = 0; i < sizeof(coords) / sizeof(coords[0]); ++i) {
            CHECK(pattern_place(&world, pattern_lookup("block"), coords[i][0], coords[i][1]) == PATTERN_PLACE_OUT_OF_BOUNDS);
            CHECK(memcmp(world.cells, before, sizeof(before)) == 0);
        }
        CHECK(pattern_place_centered(&world, pattern_lookup("gosper")) == PATTERN_PLACE_OUT_OF_BOUNDS);
        PlacementConfig bad_position = {.centered = 1, .has_xy = 1, .x = INT_MAX, .y = 0};
        CHECK(pattern_apply(&world, pattern_lookup("block"), &bad_position) == PATTERN_PLACE_OUT_OF_BOUNDS);
        PatternCell bad_cells[] = {{0,0}, {1,0}, {0,1}, {2,1}};
        Pattern malformed = {"bad", bad_cells, 4, 2, 2};
        CHECK(pattern_place(&world, &malformed, 1, 1) == PATTERN_PLACE_INVALID_PATTERN);
        CHECK(memcmp(world.cells, before, sizeof(before)) == 0);
        bad_cells[3] = (PatternCell){-1,1};
        CHECK(pattern_place_centered(&world, &malformed) == PATTERN_PLACE_INVALID_PATTERN);
        malformed = *pattern_lookup("block"); malformed.width = INT_MAX;
        CHECK(pattern_place_centered(&world, &malformed) == PATTERN_PLACE_OUT_OF_BOUNDS);
        malformed = *pattern_lookup("block"); malformed.count = 0;
        CHECK(pattern_place(&world, &malformed, 0, 0) == PATTERN_PLACE_INVALID_PATTERN);
        malformed = *pattern_lookup("block"); malformed.cells = NULL;
        CHECK(pattern_place(&world, &malformed, 0, 0) == PATTERN_PLACE_INVALID_PATTERN);
        CHECK(pattern_place(&world, NULL, 0, 0) == PATTERN_PLACE_INVALID_ARGUMENT);
        CHECK(pattern_place(NULL, pattern_lookup("block"), 0, 0) == PATTERN_PLACE_INVALID_ARGUMENT);
        World invalid = world; invalid.h = 0;
        CHECK(pattern_place_centered(&invalid, pattern_lookup("block")) == PATTERN_PLACE_INVALID_ARGUMENT);
        invalid = world; invalid.w = invalid.h = INT_MAX;
        CHECK(pattern_place_centered(&invalid, pattern_lookup("block")) == PATTERN_PLACE_INVALID_ARGUMENT);
        CHECK(world.cells == cells && world.next == next && world.w == 6 && world.h == 5 && world.toroidal == toroidal);
        CHECK(memcmp(world.cells, before, sizeof(before)) == 0);
        for (size_t i = 0; i < 30; ++i) CHECK(world.next[i] == 1);
        world_destroy(&world);
    }
    CHECK(strstr(pattern_place_message(PATTERN_PLACE_OUT_OF_BOUNDS), "outside") != NULL);
    passed = 1;
cleanup:
    world_destroy(&world);
    return passed;
}

static int pattern_parsed_placement(void) {
    World world = {0};
    CellsPattern parsed = {0};
    FILE *stream = NULL;
    int passed = 0;
    stream = tmpfile(); CHECK(stream != NULL);
    CHECK(fputs("!custom\n..O...\n....O.\n", stream) >= 0 && fseek(stream, 0, SEEK_SET) == 0);
    CHECK(cells_parse(stream, &parsed, NULL) == CELLS_OK);
    int closed = fclose(stream); stream = NULL; CHECK(closed == 0);
    Pattern view = {NULL, parsed.cells, parsed.count, parsed.width, parsed.height};
    CHECK(world_init(&world, 8, 6, 1) == 0);
    placement_base(&world);
    CHECK(pattern_apply(&world, &view, NULL) == PATTERN_PLACE_OK);
    CHECK(placed_world(&world, &view, 2, 2));
    PlacementConfig explicit = {.has_xy = 1, .x = 5, .y = 4};
    placement_base(&world);
    CHECK(pattern_apply(&world, &view, &explicit) == PATTERN_PLACE_OK);
    CHECK(placed_world(&world, &view, 5, 4));
    CHECK(parsed.count == 2 && parsed.width == 3 && parsed.height == 2);
    CHECK(parsed.cells[0].dx == 0 && parsed.cells[0].dy == 0 && parsed.cells[1].dx == 2 && parsed.cells[1].dy == 1);
    passed = 1;
cleanup:
    if (stream) fclose(stream);
    world_destroy(&world);
    cells_pattern_destroy(&parsed);
    return passed;
}

int main(int argc, char **argv) {
    const char *selection = argc == 2 ? argv[1] : "all";
    const struct { const char *name; int (*run)(void); } cases[] = {
        {"pattern-registry", pattern_registry}, {"pattern-fixtures", pattern_fixtures},
        {"pattern-placement", pattern_placement}, {"pattern-rejection", pattern_rejection},
        {"pattern-parsed-placement", pattern_parsed_placement},
    };
    int selected = 0, failed = 0;
    if (argc > 2) return 2;
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        if (strcmp(selection, "all") && strcmp(selection, cases[i].name)) continue;
        ++selected;
        if (cases[i].run()) printf("PASS %s\n", cases[i].name);
        else {
            ++failed;
            fprintf(stderr, "FAIL %s; reproduce: make test-patterns TEST_CASE=%s\n", cases[i].name, cases[i].name);
        }
    }
    if (!selected) {
        fprintf(stderr, "Unknown/empty pattern case: '%s'\n", selection);
        return 2;
    }
    printf("Pattern tests: %d selected, %d passed, %d failed\n", selected, selected - failed, failed);
    return failed ? 1 : 0;
}
