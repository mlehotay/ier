#include "world.h"
#include "pattern.h"

#include <stdio.h>
#include <string.h>

/* Checks stay active even when the compiler defines NDEBUG. */
#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s: failed: %s\n", \
                __FILE__, __LINE__, __func__, #condition); \
        goto cleanup; \
    } \
} while (0)

static int stats_equal(const StepStats *a, const StepStats *b) {
    return a->live_cells == b->live_cells && a->births == b->births &&
           a->deaths == b->deaths && a->survivors == b->survivors &&
           a->changed == b->changed;
}

static int empty_world(void) {
    World world = {0};
    const StepStats zero = {0};
    int passed = 0;

    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        CHECK(world_init(&world, 5, 5, toroidal) == 0);
        for (int step = 0; step < 4; ++step) {
            StepStats stats;
            world_step(&world, &stats);
            CHECK(stats_equal(&stats, &zero));
            for (int y = 0; y < world.h; ++y) {
                for (int x = 0; x < world.w; ++x) {
                    CHECK(world_get(&world, x, y) == 0);
                }
            }
        }
        world_destroy(&world);
    }
    passed = 1;
cleanup:
    world_destroy(&world);
    return passed;
}

static int isolated_cell(void) {
    World world = {0};
    const StepStats expected = {.deaths = 1, .changed = 1};
    const StepStats zero = {0};
    StepStats stats;
    int passed = 0;

    CHECK(world_init(&world, 5, 5, 0) == 0);
    world_set(&world, 2, 2, 1);
    CHECK(world_get(&world, 2, 2) == 1);
    world_recompute_stats(&world, &stats);
    CHECK(stats.live_cells == 1);
    world_step(&world, &stats);
    CHECK(world_get(&world, 2, 2) == 0);
    CHECK(stats_equal(&stats, &expected));
    world_step(&world, &stats);
    CHECK(stats_equal(&stats, &zero));
    passed = 1;
cleanup:
    world_destroy(&world);
    return passed;
}

static int seeded_repeatability(void) {
    World a = {0}, b = {0};
    const unsigned int seed = 42;
    int passed = 0;

    CHECK(world_init(&a, 9, 7, 1) == 0);
    CHECK(world_init(&b, 9, 7, 1) == 0);
    world_randomize(&a, seed);
    world_randomize(&b, seed);
    CHECK(memcmp(a.cells, b.cells, (size_t)a.w * (size_t)a.h) == 0);
    for (int step = 0; step < 8; ++step) {
        StepStats sa, sb;
        world_step(&a, &sa);
        world_step(&b, &sb);
        CHECK(memcmp(a.cells, b.cells, (size_t)a.w * (size_t)a.h) == 0);
        CHECK(stats_equal(&sa, &sb));
    }
    passed = 1;
cleanup:
    world_destroy(&a);
    world_destroy(&b);
    return passed;
}

typedef struct {
    const char *name;
    int (*run)(void);
} TestCase;

/* Literal phases are independent of the engine and registry cell lists. */
static int board_matches(const World *world, const char *rows, int width, int height,
                          int origin_x, int origin_y, int wrap) {
    for (int y = 0; y < world->h; ++y) {
        for (int x = 0; x < world->w; ++x) {
            int expected = 0;
            for (int py = 0; py < height; ++py) {
                for (int px = 0; px < width; ++px) {
                    if (rows[py * width + px] != 'O') continue;
                    int wx = origin_x + px, wy = origin_y + py;
                    if (wrap) { wx %= world->w; wy %= world->h; }
                    if (x == wx && y == wy) expected = 1;
                }
            }
            if (world_get(world, x, y) != expected) {
                fprintf(stderr, "Board mismatch at (%d,%d): expected %d, actual %d\n",
                        x, y, expected, world_get(world, x, y));
                return 0;
            }
        }
    }
    return 1;
}

static int oscillator(const char *name, const char *phase0, const char *phase1,
                      int width, int height, int top_padding,
                      const StepStats *odd, const StepStats *even) {
    World world = {0};
    int passed = 0;
    const Pattern *pattern = pattern_lookup(name);
    CHECK(pattern != NULL);
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        for (int origin = 0; origin < 2; ++origin) {
            int x = origin ? 7 : 3, y = origin ? 6 : 3;
            CHECK(world_init(&world, 16, 14, toroidal) == 0);
            CHECK(pattern_place(&world, pattern, x, y) == PATTERN_PLACE_OK);
            CHECK(board_matches(&world, phase0, width, height, x, y - top_padding, 0));
            for (int step = 1; step <= 8; ++step) {
                StepStats stats;
                world_step(&world, &stats);
                CHECK(board_matches(&world, step % 2 ? phase1 : phase0,
                                    width, height, x, y - top_padding, 0));
                CHECK(stats_equal(&stats, step % 2 ? odd : even));
                CHECK(world.w == 16 && world.h == 14 && world.toroidal == toroidal);
            }
            world_destroy(&world);
        }
    }
    passed = 1;
cleanup:
    world_destroy(&world);
    return passed;
}

static int block_stable(void) {
    const StepStats expected = {.live_cells = 4, .survivors = 4};
    return oscillator("block", "OOOO", "OOOO", 2, 2, 0, &expected, &expected);
}

static int blinker_period(void) {
    const StepStats expected = {.live_cells = 3, .births = 2, .deaths = 2, .survivors = 1, .changed = 4};
    return oscillator("blinker", "..." "OOO" "...", ".O." ".O." ".O.",
                      3, 3, 1, &expected, &expected);
}

static int toad_period(void) {
    const StepStats expected = {.live_cells = 6, .births = 4, .deaths = 4, .survivors = 2, .changed = 8};
    return oscillator("toad", "...." ".OOO" "OOO." "....", "..O." "O..O" "O..O" ".O..",
                      4, 4, 1, &expected, &expected);
}

static int beacon_period(void) {
    const StepStats odd = {.live_cells = 6, .deaths = 2, .survivors = 6, .changed = 2};
    const StepStats even = {.live_cells = 8, .births = 2, .survivors = 6, .changed = 2};
    return oscillator("beacon", "OO.." "OO.." "..OO" "..OO", "OO.." "O..." "...O" "..OO",
                      4, 4, 0, &odd, &even);
}

static int glider_translation(void) {
    World world = {0};
    int passed = 0;
    const Pattern *glider = pattern_lookup("glider");
    CHECK(glider != NULL);
    const char *shape = ".O." "..O" "OOO";
    const StepStats expected = {.live_cells = 5, .births = 2, .deaths = 2, .survivors = 3, .changed = 4};
    for (int scenario = 0; scenario < 3; ++scenario) {
        int toroidal = scenario != 0;
        int x = scenario == 2 ? 13 : 3, y = scenario == 2 ? 11 : 3;
        CHECK(world_init(&world, 16, 14, toroidal) == 0);
        CHECK(pattern_place(&world, glider, x, y) == PATTERN_PLACE_OK);
        CHECK(board_matches(&world, shape, 3, 3, x, y, toroidal));
        for (int step = 1; step <= 16; ++step) {
            StepStats stats;
            world_step(&world, &stats);
            CHECK(stats_equal(&stats, &expected));
            if (step % 4 == 0) {
                CHECK(board_matches(&world, shape, 3, 3, x + step / 4, y + step / 4, toroidal));
            }
            CHECK(world.w == 16 && world.h == 14 && world.toroidal == toroidal);
        }
        world_destroy(&world);
    }
    passed = 1;
cleanup:
    world_destroy(&world);
    return passed;
}

static int gosper_emission(void) {
    World world = {0};
    int passed = 0;
    const Pattern *gun = pattern_lookup("gosper");
    CHECK(gun != NULL);
    /* Fixed period-30 phase and independent literal output checkpoints.
       References and geometry are documented in docs/patterns.md. */
    const char *core =
        "........................O..........."
        "......................O.O..........."
        "............OO......OO............OO"
        "...........O...O....OO............OO"
        "OO........O.....O...OO.............."
        "OO........O...O.OO....O.O..........."
        "..........O.....O.......O..........."
        "...........O...O...................."
        "............OO......................";
    const struct { int x, y; const char *shape; } output[] = {
        {23, 9,  "O.." ".OO" "OO."},
        {30, 17, "O.O" ".OO" ".O."},
        {38, 24, "O.." ".OO" "OO."},
        {45, 32, "O.O" ".OO" ".O."},
    };
    CHECK(strlen(core) == 36 * 9);
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        for (int origin = 0; origin < 2; ++origin) {
            int ox = origin ? 8 : 3, oy = origin ? 8 : 3;
            CHECK(world_init(&world, 96, 80, toroidal) == 0);
            CHECK(pattern_place(&world, gun, ox, oy) == PATTERN_PLACE_OK);
            CHECK(board_matches(&world, core, 36, 9, ox, oy, 0));
            unsigned long previous_live = 36;
            for (int step = 1; step <= 124; ++step) {
                StepStats stats;
                world_step(&world, &stats);
                CHECK(stats.live_cells + stats.deaths == previous_live + stats.births);
                CHECK(stats.survivors + stats.births == stats.live_cells);
                CHECK(stats.survivors + stats.deaths == previous_live);
                CHECK(stats.changed == stats.births + stats.deaths);
                previous_live = stats.live_cells;
                CHECK(world.w == 96 && world.h == 80 && world.toroidal == toroidal);
                if (step % 30 == 0) {
                    int emitted = step / 30;
                    CHECK(stats.live_cells == (unsigned long)(36 + 5 * emitted));
                    /* Scan the complete board: stable gun phase plus exactly
                       the expected glider train, with no additional debris. */
                    for (int y = 0; y < world.h; ++y) {
                        for (int x = 0; x < world.w; ++x) {
                            int gx = x - ox, gy = y - oy;
                            int expected = gx >= 0 && gx < 36 && gy >= 0 && gy < 9 && core[gy * 36 + gx] == 'O';
                            for (int g = 0; g < emitted; ++g) {
                                int px = gx - output[g].x, py = gy - output[g].y;
                                if (px >= 0 && px < 3 && py >= 0 && py < 3 && output[g].shape[py * 3 + px] == 'O') expected = 1;
                            }
                            CHECK(world_get(&world, x, y) == expected);
                        }
                    }
                } else if (step >= 34 && step % 30 == 4) {
                    int emitted = step / 30;
                    /* Four steps later each detached glider keeps its phase
                       and shifts (+1,+1). The surrounding border stays dead. */
                    for (int g = 0; g < emitted; ++g) {
                        for (int py = -1; py <= 3; ++py) {
                            for (int px = -1; px <= 3; ++px) {
                                int expected = px >= 0 && px < 3 && py >= 0 && py < 3 && output[g].shape[py * 3 + px] == 'O';
                                CHECK(world_get(&world, ox + output[g].x + 1 + px,
                                                oy + output[g].y + 1 + py) == expected);
                            }
                        }
                    }
                }
            }
            world_destroy(&world);
        }
    }
    passed = 1;
cleanup:
    world_destroy(&world);
    return passed;
}

static const TestCase cases[] = {
    {"empty-world", empty_world},
    {"isolated-cell", isolated_cell},
    {"seeded-repeatability", seeded_repeatability},
    {"block-stable", block_stable},
    {"blinker-period", blinker_period},
    {"toad-period", toad_period},
    {"beacon-period", beacon_period},
    {"glider-translation", glider_translation},
    {"gosper-emission", gosper_emission},
};

int main(int argc, char **argv) {
    const size_t count = sizeof(cases) / sizeof(cases[0]);
    const char *selection = argc == 2 ? argv[1] : "all";
    size_t selected = 0, failed = 0;

    if (argc > 2 || count == 0 || selection[0] == '\0') {
        fprintf(stderr, "Usage: %s [all|CASE]; selection must be nonempty\n", argv[0]);
        return 2;
    }
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(selection, "all") != 0 && strcmp(selection, cases[i].name) != 0) continue;
        ++selected;
        if (cases[i].run()) {
            printf("PASS %s\n", cases[i].name);
        } else {
            ++failed;
            fprintf(stderr, "FAIL %s; reproduce: make test-unit TEST_CASE=%s\n",
                    cases[i].name, cases[i].name);
        }
    }
    if (selected == 0) {
        fprintf(stderr, "Unknown test case: '%s'; available cases:\n", selection);
        for (size_t i = 0; i < count; ++i) fprintf(stderr, "  %s\n", cases[i].name);
        return 2;
    }
    printf("World tests: %zu selected, %zu passed, %zu failed\n",
           selected, selected - failed, failed);
    return failed ? 1 : 0;
}
