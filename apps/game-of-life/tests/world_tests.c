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

/* Beer (2015), Figure 3: all 2^9 labeled local configurations, with
   no rotation/reflection quotient. The oracle is the B3/S23 truth table;
   the production transition is executed only by world_step. */
static int local_process_partition(void) {
    static const int output[2][9] = {
        {0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0},
    };
    static const unsigned int multiplicity[9] = {1, 8, 28, 56, 70, 56, 28, 8, 1};
    World world = {0};
    int passed = 0;

    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        unsigned int seen[2][9] = {{0}};
        unsigned int production = 0, maintenance_on = 0, destruction = 0;
        unsigned int maintenance_off = 0, null = 0;
        CHECK(world_init(&world, 5, 5, toroidal) == 0);
        for (unsigned int mask = 0; mask < 512; ++mask) {
            StepStats stats;
            unsigned int neighbors = 0;
            int before = (int)((mask >> 4) & 1u);
            world_clear(&world);
            /* Row-major 3x3 patch at (1,1); center bit 4 is (2,2).
               Its full neighborhood is interior under both topologies. */
            for (unsigned int bit = 0; bit < 9; ++bit) {
                int state = (int)((mask >> bit) & 1u);
                world_set(&world, 1 + (int)(bit % 3), 1 + (int)(bit / 3), state);
                if (bit != 4) neighbors += (unsigned int)state;
            }
            ++seen[before][neighbors];
            CHECK(world_count_neighbors(&world, 2, 2) == (int)neighbors);
            world_step(&world, &stats);
            int after = world_get(&world, 2, 2);
            if (after != output[before][neighbors]) {
                fprintf(stderr, "local-process mask=%u topology=%d center=%d neighbors=%u\n",
                        mask, toroidal, before, neighbors);
                goto cleanup;
            }
            if (mask == 0) ++null;
            else if (!before && after) ++production;
            else if (before && after) ++maintenance_on;
            else if (before && !after) ++destruction;
            else ++maintenance_off;
        }
        for (int before = 0; before <= 1; ++before) {
            for (int neighbors = 0; neighbors <= 8; ++neighbors) {
                CHECK(seen[before][neighbors] == multiplicity[neighbors]);
            }
        }
        CHECK(production == 56 && maintenance_on == 84 && destruction == 172);
        CHECK(maintenance_off == 199 && null == 1);
        CHECK(production + maintenance_on + destruction + maintenance_off + null == 512);
        printf("Local processes (%s): production=%u ON-maintenance=%u destruction=%u "
               "OFF-maintenance=%u null=%u\n", toroidal ? "toroidal" : "bounded",
               production, maintenance_on, destruction, maintenance_off, null);
        world_destroy(&world);
    }
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

/* Radius-one support of a literal phase, including its OFF ring. This
   selects known source fixtures; it is not a general boundary detector. */
static int phase_domain(const char *phase, int x, int y) {
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int px = x + dx, py = y + dy;
            if (px >= 0 && px < 5 && py >= 0 && py < 5 && phase[py * 5 + px] == 'O') return 1;
        }
    }
    return 0;
}

static int source_process_networks(void) {
    static const struct {
        const char *name;
        const char *phase[4];
        int period, displacement;
        unsigned int nodes, links, production, destruction, on, off;
    } fixtures[] = {
        {"block", {"OO..." "OO..." "....." "....." "....."},
         1, 0, 16, 84, 0, 0, 4, 12},
        {"blinker", {"....." "OOO.." "....." "....." ".....",
                     ".O..." ".O..." ".O..." "....." "....."},
         2, 0, 30, 162, 4, 4, 2, 20},
        {"glider", {".O..." "..O.." "OOO.." "....." ".....",
                    "....." "O.O.." ".OO.." ".O..." ".....",
                    "....." "..O.." "O.O.." ".OO.." ".....",
                    "....." ".O..." "..OO." ".OO.." "....."},
         4, 1, 88, 566, 8, 8, 12, 60},
    };
    World world = {0};
    int passed = 0;
    for (size_t f = 0; f < sizeof(fixtures) / sizeof(fixtures[0]); ++f) {
        for (int toroidal = 0; toroidal <= 1; ++toroidal) {
            unsigned int nodes = 0, links = 0, production = 0, destruction = 0, on = 0, off = 0;
            CHECK(world_init(&world, 12, 12, toroidal) == 0);
            for (int phase = 0; phase < fixtures[f].period; ++phase) {
                const char *current = fixtures[f].phase[phase];
                int next_phase = (phase + 1) % fixtures[f].period;
                const char *next = fixtures[f].phase[next_phase];
                int shift = next_phase == 0 ? fixtures[f].displacement : 0;
                StepStats stats;
                CHECK(strlen(current) == 25);
                world_clear(&world);
                for (int y = 0; y < 5; ++y) {
                    for (int x = 0; x < 5; ++x) {
                        world_set(&world, x + 3, y + 3, current[y * 5 + x] == 'O');
                    }
                }
                CHECK(board_matches(&world, current, 5, 5, 3, 3, 0));
                world_step(&world, &stats);
                CHECK(board_matches(&world, next, 5, 5, 3 + shift, 3 + shift, 0));
                for (int y = -1; y <= 5; ++y) {
                    for (int x = -1; x <= 5; ++x) {
                        if (!phase_domain(current, x, y)) continue;
                        ++nodes;
                        int before = x >= 0 && x < 5 && y >= 0 && y < 5 && current[y * 5 + x] == 'O';
                        int after = world_get(&world, x + 3, y + 3);
                        if (!before && after) ++production;
                        else if (before && !after) ++destruction;
                        else if (before) ++on;
                        else ++off;
                        /* Products of this phase supply the nine input sites
                           of next-phase processes. Fold the final phase by the
                           declared translation, preserving phase identity.
                           Suppress a link only when it becomes a self-loop. */
                        for (int ty = y - 1; ty <= y + 1; ++ty) {
                            for (int tx = x - 1; tx <= x + 1; ++tx) {
                                if (!phase_domain(next, tx - shift, ty - shift)) continue;
                                if (next_phase == phase && tx - shift == x && ty - shift == y) continue;
                                ++links;
                            }
                        }
                    }
                }
            }
            CHECK(nodes == fixtures[f].nodes && links == fixtures[f].links);
            CHECK(production == fixtures[f].production && destruction == fixtures[f].destruction);
            CHECK(on == fixtures[f].on && off == fixtures[f].off);
            printf("Source network %s (%s): nodes=%u links=%u production=%u destruction=%u ON=%u OFF=%u\n",
                   fixtures[f].name, toroidal ? "toroidal" : "bounded", nodes, links,
                   production, destruction, on, off);
            world_destroy(&world);
        }
    }
    passed = 1;
cleanup:
    world_destroy(&world);
    return passed;
}

static int independent_block_boundaries(void) {
    World a = {0}, b = {0}, joint = {0};
    int passed = 0;
    /* Near pair: disjoint OFF rings touch; far pair: rings stay separated. */
    for (int right = 6; right <= 8; right += 2) {
        for (int toroidal = 0; toroidal <= 1; ++toroidal) {
            CHECK(world_init(&a, 14, 10, toroidal) == 0);
            CHECK(world_init(&b, 14, 10, toroidal) == 0);
            CHECK(world_init(&joint, 14, 10, toroidal) == 0);
            for (int dy = 0; dy < 2; ++dy) {
                for (int dx = 0; dx < 2; ++dx) {
                    world_set(&a, 2 + dx, 2 + dy, 1);
                    world_set(&b, right + dx, 2 + dy, 1);
                    world_set(&joint, 2 + dx, 2 + dy, 1);
                    world_set(&joint, right + dx, 2 + dy, 1);
                }
            }
            for (int tick = 0; tick <= 32; ++tick) {
                for (int y = 0; y < 10; ++y) {
                    for (int x = 0; x < 14; ++x) {
                        int left_live = x >= 2 && x <= 3 && y >= 2 && y <= 3;
                        int right_live = x >= right && x <= right + 1 && y >= 2 && y <= 3;
                        CHECK(world_get(&a, x, y) == left_live);
                        CHECK(world_get(&b, x, y) == right_live);
                        CHECK(world_get(&joint, x, y) == (left_live || right_live));
                    }
                }
                if (tick < 32) {
                    StepStats sa, sb, sj;
                    world_step(&a, &sa);
                    world_step(&b, &sb);
                    world_step(&joint, &sj);
                    CHECK(sa.live_cells == 4 && sb.live_cells == 4 && sj.live_cells == 8);
                    CHECK(sa.changed == 0 && sb.changed == 0 && sj.changed == 0);
                }
            }
            unsigned int cross_links = 0;
            /* A directed source link from the left selected process mask
               to the right mask. Every such link has a reverse here. */
            for (int ay = 1; ay <= 4; ++ay) {
                for (int ax = 1; ax <= 4; ++ax) {
                    for (int by = 1; by <= 4; ++by) {
                        for (int bx = right - 1; bx <= right + 2; ++bx) {
                            if (bx >= ax - 1 && bx <= ax + 1 && by >= ay - 1 && by <= ay + 1) ++cross_links;
                        }
                    }
                }
            }
            CHECK(cross_links == (right == 6 ? 10u : 0u));
            printf("Independent blocks (%s, right=%d): 32 unchanged steps, union parity, "
                   "cross-links=%u each direction\n", toroidal ? "toroidal" : "bounded", right, cross_links);
            world_destroy(&a);
            world_destroy(&b);
            world_destroy(&joint);
        }
    }
    passed = 1;
cleanup:
    world_destroy(&a);
    world_destroy(&b);
    world_destroy(&joint);
    return passed;
}

/* Logical sufficiency under all 512 neighborhood assignments. The output
   table is populated by world_step, not a second implementation of Life. */
static int sufficient_inputs(const int outputs[512], unsigned int actual, unsigned int fixed) {
    for (unsigned int completion = 0; completion < 512; ++completion) {
        if ((completion & fixed) == (actual & fixed) && outputs[completion] != outputs[actual]) return 0;
    }
    return 1;
}

static int minimal_sufficient_boundaries(void) {
    World world = {0};
    int outputs[512];
    int passed = 0;
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        CHECK(world_init(&world, 5, 5, toroidal) == 0);
        for (unsigned int mask = 0; mask < 512; ++mask) {
            StepStats stats;
            world_clear(&world);
            for (unsigned int bit = 0; bit < 9; ++bit) {
                world_set(&world, 1 + (int)(bit % 3), 1 + (int)(bit / 3), (int)((mask >> bit) & 1u));
            }
            world_step(&world, &stats);
            outputs[mask] = world_get(&world, 2, 2);
        }
        /* At (5,2), the right block provides east/southeast ON sites.
           At (4,2), the left block provides west/southwest ON sites.
           Both centers and their other six neighbors are OFF. */
        const unsigned int actuals[] = {(1u << 5) | (1u << 8), (1u << 3) | (1u << 6)};
        const unsigned int required_cross_bits[] = {1u << 3, 1u << 5};
        for (size_t side = 0; side < 2; ++side) {
            unsigned int actual = actuals[side];
            unsigned int expected_fixed = 511u ^ actual;
            unsigned int minima = 0, minimum = 0;
            CHECK(outputs[actual] == 0);
            CHECK(sufficient_inputs(outputs, actual, expected_fixed));
            for (unsigned int fixed = 0; fixed < 512; ++fixed) {
                if (!sufficient_inputs(outputs, actual, fixed)) continue;
                int minimal = 1;
                for (unsigned int bit = 1; bit < 512; bit <<= 1) {
                    if ((fixed & bit) && sufficient_inputs(outputs, actual, fixed ^ bit)) {
                        minimal = 0;
                        break;
                    }
                }
                if (minimal) { ++minima; minimum = fixed; }
            }
            CHECK(minima == 1 && minimum == expected_fixed);
            CHECK((minimum & required_cross_bits[side]) != 0);
            /* Removing any one of the seven fixed OFF literals admits an
               ON output. The two actual ON neighbors remain unconstrained. */
            for (unsigned int bit = 1; bit < 512; bit <<= 1) {
                if (minimum & bit) CHECK(!sufficient_inputs(outputs, actual, minimum ^ bit));
            }
        }
        printf("Minimal sufficient boundaries (%s): unique seven-OFF set on each side; "
               "reciprocal cross-ring conditions retained\n", toroidal ? "toroidal" : "bounded");
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

/* Selected-fixture Candidate B audit; no membership classification. */
static int minimal_sufficient_components(void) {
    World truth = {0}, fixture = {0};
    int passed = 0, outputs[512];
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        CHECK(world_init(&truth, 5, 5, toroidal) == 0);
        CHECK(world_init(&fixture, 14, 10, toroidal) == 0);
        for (unsigned int mask = 0; mask < 512; ++mask) {
            StepStats stats;
            world_clear(&truth);
            for (unsigned int bit = 0; bit < 9; ++bit) {
                world_set(&truth, 1 + (int)(bit % 3), 1 + (int)(bit / 3), (int)((mask >> bit) & 1u));
            }
            world_step(&truth, &stats);
            outputs[mask] = world_get(&truth, 2, 2);
        }
        for (int right = 6; right <= 8; right += 2) {
            int xs[32], ys[32], on[32], neighbors[32];
            unsigned int unions[32], intersections[32];
            int reach[2][32][32] = {{{0}}};
            world_clear(&fixture);
            for (int group = 0; group < 2; ++group) {
                int origin = group ? right : 2;
                for (int y = 2; y <= 3; ++y) {
                    for (int x = origin; x <= origin + 1; ++x) world_set(&fixture, x, y, 1);
                }
                for (int local = 0; local < 16; ++local) {
                    int i = group * 16 + local;
                    xs[i] = origin - 1 + local % 4;
                    ys[i] = 1 + local / 4;
                }
            }
            for (int i = 0; i < 32; ++i) {
                unsigned int actual = 0, count = 0, minima = 0;
                for (unsigned int bit = 0; bit < 9; ++bit) {
                    if (world_get(&fixture, xs[i] - 1 + (int)(bit % 3), ys[i] - 1 + (int)(bit / 3))) {
                        actual |= 1u << bit;
                        if (bit != 4) ++count;
                    }
                }
                on[i] = (int)((actual >> 4) & 1u);
                neighbors[i] = (int)count;
                unions[i] = 0;
                intersections[i] = 511;
                for (unsigned int fixed = 0; fixed < 512; ++fixed) {
                    if (!sufficient_inputs(outputs, actual, fixed)) continue;
                    int minimal = 1;
                    for (unsigned int bit = 1; bit < 512; bit <<= 1) {
                        if ((fixed & bit) && sufficient_inputs(outputs, actual, fixed ^ bit)) {
                            minimal = 0;
                            break;
                        }
                    }
                    if (minimal) {
                        ++minima;
                        unions[i] |= fixed;
                        intersections[i] &= fixed;
                    }
                }
                CHECK(minima > 0);
                /* Independent symbolic minima for the three local classes. */
                if (on[i]) {
                    CHECK(count == 3 && minima == 4);
                    CHECK(unions[i] == 511 && intersections[i] == (511u ^ actual));
                } else if (count == 2) {
                    CHECK(minima == 1 && unions[i] == (511u ^ actual));
                    CHECK(intersections[i] == unions[i]);
                } else {
                    CHECK(count == 1 && minima == 8);
                    CHECK(unions[i] == (511u ^ actual) && intersections[i] == 0);
                }
            }
            for (int i = 0; i < 32; ++i) {
                for (int j = 0; j < 32; ++j) {
                    int dx = xs[j] - xs[i], dy = ys[j] - ys[i];
                    if (i == j) reach[0][i][i] = reach[1][i][i] = 1;
                    if (dx < -1 || dx > 1 || dy < -1 || dy > 1 || i == j) continue;
                    unsigned int bit = 1u << ((dy + 1) * 3 + dx + 1);
                    reach[0][j][i] = (unions[i] & bit) != 0;
                    reach[1][j][i] = (intersections[i] & bit) != 0;
                }
            }
            for (int policy = 0; policy < 2; ++policy) {
                for (int k = 0; k < 32; ++k) {
                    for (int i = 0; i < 32; ++i) {
                        for (int j = 0; j < 32; ++j) reach[policy][i][j] |= reach[policy][i][k] && reach[policy][k][j];
                    }
                }
                unsigned int components = 0;
                for (int i = 0; i < 32; ++i) {
                    int earlier = 0;
                    for (int j = 0; j < 32; ++j) {
                        int same = reach[policy][i][j] && reach[policy][j][i];
                        int close = right == 6 || i / 16 == j / 16;
                        int expected = i == j;
                        if (policy == 0) {
                            expected |= on[i] && on[j] && i / 16 == j / 16;
                            expected |= !on[i] && !on[j] && close;
                        } else {
                            expected |= !on[i] && !on[j] && neighbors[i] == 2 && neighbors[j] == 2 && close;
                        }
                        CHECK(same == expected);
                        if (j < i && same) earlier = 1;
                    }
                    if (!earlier) ++components;
                }
                CHECK(components == (policy == 0 ? (right == 6 ? 3u : 4u) : (right == 6 ? 17u : 18u)));
                printf("Minimal components (%s, right=%d, %s): %u; complete partition matches\n",
                       toroidal ? "toroidal" : "bounded", right,
                       policy == 0 ? "any-minimum" : "every-minimum", components);
            }
        }
        world_destroy(&truth);
        world_destroy(&fixture);
    }
    passed = 1;
cleanup:
    world_destroy(&truth);
    world_destroy(&fixture);
    return passed;
}

/* Root-owned exploratory audit after apparatus plan 0002 closure. */
static int periodic_component_audit(int domain_mode) {
    int full_board = domain_mode == 1;
    int scan_board = domain_mode != 0;
    enum { MAX_NODES = 576 };
    static const struct {
        const char *name, *phase[4];
        int period, shift, nodes, products, raw_links;
    } fixtures[] = {
        {"block", {"OO..." "OO..." "....." "....." "....."}, 1, 0, 16, 4, 84},
        {"blinker", {"....." "OOO.." "....." "....." ".....",
                     ".O..." ".O..." ".O..." "....." "....."}, 2, 0, 30, 6, 162},
        {"toad", {"....." ".OOO." "OOO.." "....." ".....",
                  "..O.." "O..O." "O..O." ".O..." "....."}, 2, 0, 52, 12, 0},
        {"glider", {".O..." "..O.." "OOO.." "....." ".....",
                    "....." "O.O.." ".OO.." ".O..." ".....",
                    "....." "..O.." "O.O.." ".OO.." ".....",
                    "....." ".O..." "..OO." ".OO.." "....."}, 4, 1, 88, 20, 566},
    };
    static const int output[2][9] = {
        {0, 0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0, 0},
    };
    World world = {0};
    int passed = 0;
    for (int toroidal = scan_board ? 1 : 0; toroidal <= 1; ++toroidal) {
        int outputs[512], cached[512] = {0};
        unsigned int union_cache[512] = {0}, intersection_cache[512] = {0};
        CHECK(world_init(&world, 12, 12, toroidal) == 0);
        for (unsigned int mask = 0; mask < 512; ++mask) {
            StepStats stats;
            int count = 0;
            world_clear(&world);
            for (unsigned int bit = 0; bit < 9; ++bit) {
                int value = (int)((mask >> bit) & 1u);
                world_set(&world, 3 + (int)(bit % 3), 3 + (int)(bit / 3), value);
                if (bit != 4) count += value;
            }
            world_step(&world, &stats);
            outputs[mask] = world_get(&world, 4, 4);
            CHECK(outputs[mask] == output[(mask >> 4) & 1u][count]);
        }
        for (size_t f = 0; f < sizeof(fixtures) / sizeof(fixtures[0]); ++f) {
            int phases[MAX_NODES], xs[MAX_NODES], ys[MAX_NODES], products[MAX_NODES], n = 0, product_count = 0;
            unsigned int unions[MAX_NODES], intersections[MAX_NODES];
            unsigned char edges[2][MAX_NODES][MAX_NODES] = {{{0}}}, reach[2][MAX_NODES][MAX_NODES] = {{{0}}};
            for (int phase = 0; phase < fixtures[f].period; ++phase) {
                const char *current = fixtures[f].phase[phase];
                int next = (phase + 1) % fixtures[f].period;
                int shift = next == 0 ? fixtures[f].shift : 0;
                StepStats stats;
                world_clear(&world);
                for (int y = 0; y < 5; ++y) {
                    for (int x = 0; x < 5; ++x) world_set(&world, x + 3, y + 3, current[y * 5 + x] == 'O');
                }
                /* Read literal input masks before native evolution. */
                int start = n;
                int low = scan_board ? -3 : -1, high = scan_board ? 8 : 5;
                for (int y = low; y <= high; ++y) {
                    for (int x = low; x <= high; ++x) {
                        if (!scan_board && !phase_domain(current, x, y)) continue;
                        CHECK(n < MAX_NODES);
                        unsigned int actual = 0;
                        for (unsigned int bit = 0; bit < 9; ++bit) {
                            if (world_get(&world, x + 2 + (int)(bit % 3), y + 2 + (int)(bit / 3))) actual |= 1u << bit;
                        }
                        if (domain_mode == 2) {
                            CHECK((actual != 0) == phase_domain(current, x, y));
                            if (actual == 0) continue;
                        }
                        if (!cached[actual]) {
                            unsigned int minima = 0;
                            intersection_cache[actual] = 511;
                            for (unsigned int fixed = 0; fixed < 512; ++fixed) {
                                if (!sufficient_inputs(outputs, actual, fixed)) continue;
                                int minimal = 1;
                                for (unsigned int bit = 1; bit < 512; bit <<= 1) {
                                    if ((fixed & bit) && sufficient_inputs(outputs, actual, fixed ^ bit)) {
                                        minimal = 0;
                                        break;
                                    }
                                }
                                if (minimal) {
                                    ++minima;
                                    union_cache[actual] |= fixed;
                                    intersection_cache[actual] &= fixed;
                                }
                            }
                            CHECK(minima > 0);
                            /* Analytic explanation checked after the scope audit:
                               null OFF has 8+28 seven-OFF minima; four ON
                               neighbors force OFF regardless of the center. */
                            if (actual == 0) {
                                CHECK(minima == 36 && union_cache[actual] == 511);
                                CHECK(intersection_cache[actual] == 0);
                            }
                            unsigned int neighbors_on = 0;
                            for (unsigned int bit = 0; bit < 9; ++bit) {
                                if (bit != 4) neighbors_on += (actual >> bit) & 1u;
                            }
                            if (neighbors_on == 4) {
                                CHECK(minima == 1 && union_cache[actual] == (actual & ~16u));
                                CHECK(intersection_cache[actual] == union_cache[actual]);
                            }
                            cached[actual] = 1;
                        }
                        phases[n] = phase; xs[n] = x; ys[n] = y;
                        unions[n] = union_cache[actual]; intersections[n] = intersection_cache[actual];
                        ++n;
                    }
                }
                world_step(&world, &stats);
                CHECK(board_matches(&world, fixtures[f].phase[next], 5, 5, 3 + shift, 3 + shift, 0));
                for (int i = start; i < n; ++i) {
                    products[i] = world_get(&world, xs[i] + 3, ys[i] + 3);
                    product_count += products[i];
                }
            }
            CHECK(n == (full_board ? 144 * fixtures[f].period : fixtures[f].nodes));
            CHECK(product_count == fixtures[f].products);
            int raw_links = 0;
            for (int source = 0; source < n; ++source) {
                int next = (phases[source] + 1) % fixtures[f].period;
                int shift = next == 0 ? fixtures[f].shift : 0;
                for (int target = 0; target < n; ++target) {
                    if (phases[target] != next || source == target) continue;
                    int dx = xs[source] - shift - xs[target], dy = ys[source] - shift - ys[target];
                    if (scan_board) {
                        dx = (dx % 12 + 12) % 12; if (dx > 6) dx -= 12;
                        dy = (dy % 12 + 12) % 12; if (dy > 6) dy -= 12;
                    }
                    if (dx < -1 || dx > 1 || dy < -1 || dy > 1) continue;
                    unsigned int bit = 1u << ((dy + 1) * 3 + dx + 1);
                    ++raw_links;
                    edges[0][source][target] = (unions[target] & bit) != 0;
                    edges[1][source][target] = (intersections[target] & bit) != 0;
                }
            }
            if (full_board) CHECK(raw_links == n * (fixtures[f].period == 1 ? 8 : 9));
            else if (fixtures[f].raw_links) CHECK(raw_links == fixtures[f].raw_links);
            for (int policy = 0; policy < 2; ++policy) {
                for (int i = 0; i < n; ++i) {
                    for (int j = 0; j < n; ++j) reach[policy][i][j] = i == j || edges[policy][i][j];
                }
                for (int k = 0; k < n; ++k) {
                    for (int i = 0; i < n; ++i) {
                        for (int j = 0; j < n; ++j) reach[policy][i][j] |= reach[policy][i][k] && reach[policy][k][j];
                    }
                }
                /* Independent graph traversal checks every reachability pair. */
                for (int source = 0; source < n; ++source) {
                    int visited[MAX_NODES] = {0}, queue[MAX_NODES], head = 0, tail = 0;
                    visited[source] = 1; queue[tail++] = source;
                    while (head < tail) {
                        int from = queue[head++];
                        for (int j = 0; j < n; ++j) {
                            if (edges[policy][from][j] && !visited[j]) { visited[j] = 1; queue[tail++] = j; }
                        }
                    }
                    for (int j = 0; j < n; ++j) CHECK(visited[j] == reach[policy][source][j]);
                }
                int components = 0, on_components = 0, off_singletons = 0, largest_productive = 0;
                printf("Periodic minimal %s (%s, %s, %s): nodes=%d raw=%d components(size:ON)=",
                       fixtures[f].name, full_board ? "whole-board" : (scan_board ? "native-non-null" : "selected-mask"), toroidal ? "toroidal" : "bounded",
                       policy ? "every-minimum" : "any-minimum", n, raw_links);
                for (int i = 0; i < n; ++i) {
                    int earlier = 0, size = 0, on_count = 0;
                    for (int j = 0; j < n; ++j) {
                        if (reach[policy][i][j] && reach[policy][j][i]) {
                            if (j < i) earlier = 1;
                            ++size; on_count += products[j];
                        }
                    }
                    if (earlier) continue;
                    ++components;
                    if (on_count) {
                        ++on_components;
                        if (size > largest_productive) largest_productive = size;
                    }
                    if (size == 1 && on_count == 0) ++off_singletons;
                    else printf(" %d:%d", size, on_count);
                }
                if (domain_mode == 2) {
                    static const int expected_components[2][4] = {{2, 2, 1, 1}, {9, 14, 23, 43}};
                    static const int expected_largest[2][4] = {{4, 6, 52, 88}, {1, 6, 30, 46}};
                    CHECK(components == expected_components[policy][f]);
                    CHECK(largest_productive == expected_largest[policy][f]);
                    CHECK(on_components == (policy == 1 && f == 0 ? 4 : 1));
                }
                if (off_singletons) printf(" %dx1:0", off_singletons);
                printf("; total=%d ON-components=%d\n", components, on_components);
            }
        }
        world_destroy(&world);
    }
    passed = 1;
cleanup:
    world_destroy(&world);
    return passed;
}

static int periodic_minimal_components(void) {
    return periodic_component_audit(0);
}

static int whole_board_minimal_components(void) {
    return periodic_component_audit(1);
}

static int native_non_null_components(void) {
    return periodic_component_audit(2);
}

/* Root-owned finite search; nominal union does not imply all-intervention
   independence or a physical membership verdict. */
static int paired_toad_components(void) {
    static const char *phases[] = {
        "....." ".OOO." "OOO.." "....." ".....",
        "..O.." "O..O." "O..O." ".O..." ".....",
    };
    static const int output[2][9] = {
        {0, 0, 0, 1, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 0, 0, 0, 0, 0},
    };
    World worlds[3] = {{0}}; /* isolated left/right, joint */
    int passed = 0;
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        int outputs[512], cached[512] = {0};
        unsigned int union_cache[512] = {0}, intersection_cache[512] = {0};
        unsigned int eligible = 0, mixed_cases[2] = {0}, histogram[2][25] = {{0}};
        for (int w = 0; w < 3; ++w) CHECK(world_init(&worlds[w], 24, 24, toroidal) == 0);
        for (unsigned int mask = 0; mask < 512; ++mask) {
            StepStats stats;
            int neighbors = 0;
            world_clear(&worlds[0]);
            for (unsigned int bit = 0; bit < 9; ++bit) {
                world_set(&worlds[0], 5 + (int)(bit % 3), 5 + (int)(bit / 3), (int)((mask >> bit) & 1u));
                if (bit != 4) neighbors += (int)((mask >> bit) & 1u);
            }
            world_step(&worlds[0], &stats);
            outputs[mask] = world_get(&worlds[0], 6, 6);
            CHECK(outputs[mask] == output[(mask >> 4) & 1u][neighbors]);
        }
        for (int second_phase = 0; second_phase <= 1; ++second_phase) {
            for (int dx = 4; dx <= 8; ++dx) {
                for (int dy = -4; dy <= 4; ++dy) {
                    int xs[128], ys[128], times[128], products[128], owners[128], n = 0;
                    unsigned int unions[128], intersections[128];
                    unsigned char edges[2][128][128] = {{{0}}}, reach[2][128][128] = {{{0}}};
                    for (int w = 0; w < 3; ++w) world_clear(&worlds[w]);
                    for (int y = 0; y < 5; ++y) {
                        for (int x = 0; x < 5; ++x) {
                            if (phases[0][y * 5 + x] == 'O') {
                                world_set(&worlds[0], 6 + x, 6 + y, 1);
                                world_set(&worlds[2], 6 + x, 6 + y, 1);
                            }
                            if (phases[second_phase][y * 5 + x] == 'O') {
                                world_set(&worlds[1], 6 + dx + x, 6 + dy + y, 1);
                                world_set(&worlds[2], 6 + dx + x, 6 + dy + y, 1);
                            }
                        }
                    }
                    int nominal = 1;
                    for (int tick = 0; tick <= 2; ++tick) {
                        CHECK(board_matches(&worlds[0], phases[tick % 2], 5, 5, 6, 6, 0));
                        CHECK(board_matches(&worlds[1], phases[(second_phase + tick) % 2], 5, 5, 6 + dx, 6 + dy, 0));
                        for (int y = 0; y < 24; ++y) {
                            for (int x = 0; x < 24; ++x) {
                                int left = world_get(&worlds[0], x, y), right = world_get(&worlds[1], x, y);
                                CHECK(!(left && right));
                                if (world_get(&worlds[2], x, y) != (left || right)) nominal = 0;
                            }
                        }
                        if (tick == 2) break;
                        int start = n;
                        if (nominal) {
                            for (int y = 0; y < 24; ++y) {
                                for (int x = 0; x < 24; ++x) {
                                    unsigned int actual = 0;
                                    for (unsigned int bit = 0; bit < 9; ++bit) {
                                        if (world_get(&worlds[2], x - 1 + (int)(bit % 3), y - 1 + (int)(bit / 3))) actual |= 1u << bit;
                                    }
                                    if (actual == 0) continue;
                                    CHECK(n < 128);
                                    if (!cached[actual]) {
                                        intersection_cache[actual] = 511;
                                        unsigned int minima = 0;
                                        for (unsigned int fixed = 0; fixed < 512; ++fixed) {
                                            if (!sufficient_inputs(outputs, actual, fixed)) continue;
                                            int minimal = 1;
                                            for (unsigned int bit = 1; bit < 512; bit <<= 1) {
                                                if ((fixed & bit) && sufficient_inputs(outputs, actual, fixed ^ bit)) { minimal = 0; break; }
                                            }
                                            if (minimal) {
                                                ++minima;
                                                union_cache[actual] |= fixed;
                                                intersection_cache[actual] &= fixed;
                                            }
                                        }
                                        CHECK(minima > 0);
                                        cached[actual] = 1;
                                    }
                                    xs[n] = x; ys[n] = y; times[n] = tick;
                                    unions[n] = union_cache[actual]; intersections[n] = intersection_cache[actual];
                                    ++n;
                                }
                            }
                        }
                        for (int w = 0; w < 3; ++w) { StepStats stats; world_step(&worlds[w], &stats); }
                        for (int i = start; i < n; ++i) {
                            products[i] = world_get(&worlds[2], xs[i], ys[i]);
                            owners[i] = world_get(&worlds[0], xs[i], ys[i]) | (world_get(&worlds[1], xs[i], ys[i]) << 1);
                        }
                    }
                    if (!nominal) continue;
                    ++eligible;
                    if (second_phase == 0 && dx == 4 && dy == -4) CHECK(n == 102);
                    int product_count = 0;
                    for (int i = 0; i < n; ++i) { CHECK(products[i] == (owners[i] != 0)); product_count += products[i]; }
                    CHECK(product_count == 24);
                    for (int source = 0; source < n; ++source) {
                        for (int target = 0; target < n; ++target) {
                            if (times[source] == times[target]) continue;
                            int rx = xs[source] - xs[target], ry = ys[source] - ys[target];
                            if (rx < -1 || rx > 1 || ry < -1 || ry > 1) continue;
                            unsigned int bit = 1u << ((ry + 1) * 3 + rx + 1);
                            edges[0][source][target] = (unions[target] & bit) != 0;
                            edges[1][source][target] = (intersections[target] & bit) != 0;
                        }
                    }
                    for (int policy = 0; policy < 2; ++policy) {
                        for (int i = 0; i < n; ++i) {
                            for (int j = 0; j < n; ++j) reach[policy][i][j] = i == j || edges[policy][i][j];
                        }
                        for (int k = 0; k < n; ++k) {
                            for (int i = 0; i < n; ++i) {
                                for (int j = 0; j < n; ++j) reach[policy][i][j] |= reach[policy][i][k] && reach[policy][k][j];
                            }
                        }
                        for (int source = 0; source < n; ++source) {
                            int visited[128] = {0}, queue[128], head = 0, tail = 0;
                            visited[source] = 1; queue[tail++] = source;
                            while (head < tail) {
                                int from = queue[head++];
                                for (int j = 0; j < n; ++j) {
                                    if (edges[policy][from][j] && !visited[j]) { visited[j] = 1; queue[tail++] = j; }
                                }
                            }
                            for (int j = 0; j < n; ++j) CHECK(visited[j] == reach[policy][source][j]);
                        }
                        int on_components = 0, mixed = 0;
                        for (int i = 0; i < n; ++i) {
                            int earlier = 0, size = 0, left_on = 0, right_on = 0;
                            for (int j = 0; j < n; ++j) {
                                if (reach[policy][i][j] && reach[policy][j][i]) {
                                    if (j < i) earlier = 1;
                                    ++size; left_on += owners[j] == 1; right_on += owners[j] == 2;
                                }
                            }
                            if (earlier || !(left_on || right_on)) continue;
                            ++on_components;
                            if (second_phase == 0 && dx == 8 && dy == 0) {
                                CHECK(size == (policy == 0 ? 52 : 30));
                                CHECK((left_on == 12 && right_on == 0) || (left_on == 0 && right_on == 12));
                            }
                            if (second_phase == 0 && dx == 4 && dy == -4) {
                                CHECK(size == (policy == 0 ? 102 : 62));
                                CHECK(left_on == 12 && right_on == 12);
                            }
                            if (left_on && right_on) {
                                if (!mixed && mixed_cases[policy] == 0) {
                                    printf("Paired toad mixed witness (%s, %s): phase=%d dx=%d dy=%d nodes=%d component=%d ON-left=%d ON-right=%d\n",
                                           toroidal ? "toroidal" : "bounded", policy ? "every-minimum" : "any-minimum",
                                           second_phase, dx, dy, n, size, left_on, right_on);
                                }
                                mixed = 1;
                            }
                        }
                        CHECK(on_components >= 1 && on_components <= 24);
                        if (second_phase == 0 && dx == 4 && dy == -4) CHECK(on_components == 1);
                        if (second_phase == 0 && dx == 8 && dy == 0) CHECK(on_components == 2);
                        ++histogram[policy][on_components];
                        mixed_cases[policy] += (unsigned int)mixed;
                    }
                }
            }
        }
        CHECK(eligible > 0);
        for (int policy = 0; policy < 2; ++policy) {
            printf("Paired toad summary (%s, %s): eligible=%u/90 mixed=%u ON-component-histogram=",
                   toroidal ? "toroidal" : "bounded", policy ? "every-minimum" : "any-minimum", eligible, mixed_cases[policy]);
            for (int count = 1; count <= 24; ++count) if (histogram[policy][count]) printf(" %d:%u", count, histogram[policy][count]);
            printf("\n");
        }
        for (int w = 0; w < 3; ++w) world_destroy(&worlds[w]);
    }
    passed = 1;
cleanup:
    for (int w = 0; w < 3; ++w) world_destroy(&worlds[w]);
    return passed;
}

/* Predicted inhibitory OFF-condition coupling despite nominal superposition. */
static int paired_toad_interventions(void) {
    static const char *phase[] = {
        "....." ".OOO." "OOO.." "....." ".....",
        "..O.." "O..O." "O..O." ".O..." ".....",
    };
    World worlds[5] = {{0}}; /* source, receiver, joint, edited source/joint */
    int passed = 0;
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        for (int w = 0; w < 5; ++w) CHECK(world_init(&worlds[w], 24, 24, toroidal) == 0);
        for (int far = 0; far <= 1; ++far) {
            for (int direction = 0; direction <= 1; ++direction) {
                int rx = far ? 14 : 10, ry = far ? 6 : 2;
                int sx = direction ? rx : 6, sy = direction ? ry : 6;
                int tx = direction ? 6 : rx, ty = direction ? 6 : ry;
                int px = direction ? rx - 1 : 10, py = direction ? ry + 3 : 6;
                int qx = direction ? 8 : rx + (far ? 2 : 1), qy = direction ? 6 : ry + 3;
                if (far && !direction) qy = ry;
                for (int w = 0; w < 5; ++w) {
                    world_clear(&worlds[w]);
                    for (int y = 0; y < 5; ++y) {
                        for (int x = 0; x < 5; ++x) {
                            if (phase[0][y * 5 + x] != 'O') continue;
                            if (w != 1) world_set(&worlds[w], sx + x, sy + y, 1);
                            if (w == 1 || w == 2 || w == 4) world_set(&worlds[w], tx + x, ty + y, 1);
                        }
                    }
                }
                CHECK(world_get(&worlds[0], px, py) == 0 && world_get(&worlds[1], px, py) == 0);
                CHECK(world_count_neighbors(&worlds[0], px, py) > 0);
                CHECK(world_count_neighbors(&worlds[1], px, py) == 0);
                world_set(&worlds[3], px, py, 1);
                world_set(&worlds[4], px, py, 1);
                CHECK(world_get(&worlds[2], qx, qy) == 0 && world_get(&worlds[4], qx, qy) == 0);
                CHECK(world_count_neighbors(&worlds[2], qx, qy) == 3);
                CHECK(world_count_neighbors(&worlds[1], qx, qy) == 3);
                CHECK(world_count_neighbors(&worlds[4], qx, qy) == (far ? 3 : 4));
                CHECK(world_count_neighbors(&worlds[3], qx, qy) == (far ? 0 : 1));
                for (int tick = 0; tick <= 2; ++tick) {
                    unsigned int compositional_difference = 0, receiver_on_losses = 0;
                    CHECK(board_matches(&worlds[0], phase[tick % 2], 5, 5, sx, sy, 0));
                    CHECK(board_matches(&worlds[1], phase[tick % 2], 5, 5, tx, ty, 0));
                    for (int y = 0; y < 24; ++y) {
                        for (int x = 0; x < 24; ++x) {
                            int original_union = world_get(&worlds[0], x, y) || world_get(&worlds[1], x, y);
                            int edited_union = world_get(&worlds[3], x, y) || world_get(&worlds[1], x, y);
                            if (far && tick < 2) {
                                int source_patch = world_get(&worlds[3], x, y) || world_count_neighbors(&worlds[3], x, y);
                                int receiver_patch = world_get(&worlds[1], x, y) || world_count_neighbors(&worlds[1], x, y);
                                CHECK(!(source_patch && receiver_patch));
                            }
                            CHECK(world_get(&worlds[2], x, y) == original_union);
                            compositional_difference += world_get(&worlds[4], x, y) != edited_union;
                            receiver_on_losses += world_get(&worlds[1], x, y) && !world_get(&worlds[4], x, y);
                        }
                    }
                    if (far || tick == 0) CHECK(compositional_difference == 0 && receiver_on_losses == 0);
                    if (tick == 1) {
                        CHECK(world_get(&worlds[2], qx, qy) == 1);
                        CHECK(world_get(&worlds[1], qx, qy) == 1);
                        CHECK(world_get(&worlds[3], qx, qy) == 0);
                        CHECK(world_get(&worlds[4], qx, qy) == far);
                        if (!far) CHECK(compositional_difference > 0 && receiver_on_losses > 0);
                    }
                    if (tick > 0) {
                        printf("Paired toad intervention (%s, %s, %s, tick=%d): edit=(%d,%d) target=(%d,%d) composition-diff=%u receiver-ON-losses=%u\n",
                               toroidal ? "toroidal" : "bounded", far ? "far" : "near",
                               direction ? "right-to-left" : "left-to-right", tick,
                               px, py, qx, qy, compositional_difference, receiver_on_losses);
                    }
                    if (tick < 2) {
                        for (int w = 0; w < 5; ++w) { StepStats stats; world_step(&worlds[w], &stats); }
                    }
                }
            }
        }
        for (int w = 0; w < 5; ++w) world_destroy(&worlds[w]);
    }
    passed = 1;
cleanup:
    for (int w = 0; w < 5; ++w) world_destroy(&worlds[w]);
    return passed;
}

static int certificate_domain(int extended, int x, int y) {
    int lo = extended ? 3 : 4, hi = extended ? 6 : 5;
    return x >= lo && x <= hi && y >= lo && y <= hi;
}

static int certificate_external_support(int extended, int x, int y) {
    if (extended) {
        return ((x == 2 || x == 7) && y >= 3 && y <= 6) ||
               ((y == 2 || y == 7) && x >= 3 && x <= 6);
    }
    return x >= 3 && x <= 6 && y >= 3 && y <= 6 && !certificate_domain(0, x, y);
}

/* Joint successor guarantees do not select a unique organizational boundary. */
static int block_support_certificates(void) {
    World world = {0};
    int passed = 0;
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        CHECK(world_init(&world, 12, 12, toroidal) == 0);
        for (int extended = 0; extended <= 1; ++extended) {
            unsigned int sufficient_checks = 0, necessity_checks = 0, support_count = 0;
            for (int y = 0; y < 12; ++y) {
                for (int x = 0; x < 12; ++x) support_count += certificate_external_support(extended, x, y);
            }
            CHECK(support_count == (extended ? 16u : 12u));
            for (int far_fill = 0; far_fill <= 1; ++far_fill) {
                unsigned int completions = extended ? 16u : 1u;
                for (unsigned int corners = 0; corners < completions; ++corners) {
                    int lo = extended ? 2 : 3, hi = extended ? 7 : 6;
                    for (int y = 0; y < 12; ++y) {
                        for (int x = 0; x < 12; ++x) {
                            int inside_patch = x >= lo && x <= hi && y >= lo && y <= hi;
                            int value = inside_patch ? certificate_domain(0, x, y) : far_fill;
                            world_set(&world, x, y, value);
                        }
                    }
                    if (extended) {
                        world_set(&world, 2, 2, (int)(corners & 1u));
                        world_set(&world, 7, 2, (int)((corners >> 1) & 1u));
                        world_set(&world, 2, 7, (int)((corners >> 2) & 1u));
                        world_set(&world, 7, 7, (int)((corners >> 3) & 1u));
                    }
                    StepStats stats;
                    world_step(&world, &stats);
                    for (int y = 0; y < 12; ++y) {
                        for (int x = 0; x < 12; ++x) {
                            if (certificate_domain(extended, x, y)) {
                                CHECK(world_get(&world, x, y) == certificate_domain(0, x, y));
                            }
                        }
                    }
                    ++sufficient_checks;
                }
            }
            for (int py = 0; py < 12; ++py) {
                for (int px = 0; px < 12; ++px) {
                    if (!certificate_external_support(extended, px, py)) continue;
                    world_clear(&world);
                    for (int y = 4; y <= 5; ++y) {
                        for (int x = 4; x <= 5; ++x) world_set(&world, x, y, 1);
                    }
                    world_set(&world, px, py, 1);
                    StepStats stats;
                    world_step(&world, &stats);
                    unsigned int on_losses = 0, off_births = 0;
                    for (int y = 0; y < 12; ++y) {
                        for (int x = 0; x < 12; ++x) {
                            if (!certificate_domain(extended, x, y)) continue;
                            int before = certificate_domain(0, x, y), after = world_get(&world, x, y);
                            on_losses += before && !after;
                            off_births += !before && after;
                        }
                    }
                    if (extended) CHECK(off_births > 0 && on_losses == 0);
                    else CHECK(on_losses > 0);
                    ++necessity_checks;
                }
            }
            CHECK(necessity_checks == support_count);
            printf("Block support (%s, %s): support=%u sufficiency-completions=%u necessity-witnesses=%u\n",
                   toroidal ? "toroidal" : "bounded", extended ? "extended" : "core",
                   support_count, sufficient_checks, necessity_checks);
        }
        world_destroy(&world);
    }
    passed = 1;
cleanup:
    world_destroy(&world);
    return passed;
}

/* Literal regeneration oracles and the product-aggregation limit. */
static int block_regeneration_products(void) {
    World world = {0};
    int passed = 0;
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        CHECK(world_init(&world, 16, 12, toroidal) == 0);
        for (int paired = 0; paired <= 1; ++paired) {
            unsigned int outcomes[4] = {0}, repaired = 0;
            for (unsigned int left = 0; left < 16; ++left) {
                for (unsigned int right = 0; right < (paired ? 16u : 1u); ++right) {
                    world_clear(&world);
                    unsigned int left_count = 0, right_count = 0;
                    for (unsigned int bit = 0; bit < 4; ++bit) {
                        int x = (int)(bit % 2), y = (int)(bit / 2);
                        int left_on = (int)((left >> bit) & 1u), right_on = (int)((right >> bit) & 1u);
                        world_set(&world, 3 + x, 3 + y, left_on);
                        world_set(&world, 9 + x, 3 + y, right_on);
                        left_count += (unsigned int)left_on;
                        right_count += (unsigned int)right_on;
                    }
                    int keep_left = left_count >= 3, keep_right = right_count >= 3;
                    ++outcomes[keep_left | (keep_right << 1)];
                    if (!paired && left_count == 3) ++repaired;
                    for (int tick = 1; tick <= 8; ++tick) {
                        StepStats stats;
                        world_step(&world, &stats);
                        unsigned long expected_live = 4ul * (unsigned long)(keep_left + keep_right);
                        unsigned long expected_births = tick == 1 ? (unsigned long)(left_count == 3) + (unsigned long)(right_count == 3) : 0;
                        unsigned long expected_deaths = tick == 1 ? (left_count <= 2 ? left_count : 0) + (right_count <= 2 ? right_count : 0) : 0;
                        CHECK(stats.live_cells == expected_live);
                        CHECK(stats.births == expected_births && stats.deaths == expected_deaths);
                        CHECK(stats.survivors + stats.births == expected_live);
                        CHECK(stats.changed == expected_births + expected_deaths);
                        for (int y = 0; y < 12; ++y) {
                            for (int x = 0; x < 16; ++x) {
                                int expected = y >= 3 && y <= 4 &&
                                    ((keep_left && x >= 3 && x <= 4) || (keep_right && x >= 9 && x <= 10));
                                CHECK(world_get(&world, x, y) == expected);
                            }
                        }
                    }
                }
            }
            if (paired) {
                CHECK(outcomes[0] == 121 && outcomes[1] == 55 && outcomes[2] == 55 && outcomes[3] == 25);
            } else {
                CHECK(outcomes[0] == 11 && outcomes[1] == 5 && outcomes[2] == 0 && outcomes[3] == 0);
                CHECK(repaired == 4);
            }
            printf("Block regeneration (%s, %s): empty=%u left=%u right=%u both=%u; all complete boards match through tick 8\n",
                   toroidal ? "toroidal" : "bounded", paired ? "paired" : "single",
                   outcomes[0], outcomes[1], outcomes[2], outcomes[3]);
        }
        world_destroy(&world);
    }
    passed = 1;
cleanup:
    world_destroy(&world);
    return passed;
}

/* Frozen protocol: root unity/review/discrimination-cases.md, 2026-11-15.
   Scheduled exterior overwrites are interventions, not whole-world F-steps. */
static int replay_center_count(const World *world) {
    int count = 0;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx || dy) count += world_get(world, 4 + dx, 4 + dy);
        }
    }
    return count;
}

static int boundary_replay(void) {
    static const int output[2][9] = {
        {0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0},
    };
    World worlds[4] = {{0}}; /* baseline, perturbed, nominal replay, perturbed replay */
    int passed = 0;
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        unsigned int divergent = 0;
        for (int w = 0; w < 4; ++w) CHECK(world_init(&worlds[w], 9, 9, toroidal) == 0);
        for (unsigned int mask = 0; mask < 256; ++mask) {
            int baseline[5][81], full_states[5], replay_states[5];
            for (int w = 0; w < 4; ++w) {
                world_clear(&worlds[w]);
                unsigned int bit = 0;
                for (int y = 3; y <= 5; ++y) {
                    for (int x = 3; x <= 5; ++x) {
                        if (x == 4 && y == 4) continue;
                        world_set(&worlds[w], x, y, (int)((mask >> bit++) & 1u));
                    }
                }
                world_set(&worlds[w], 4, 4, w == 1 || w == 3);
            }
            for (int tick = 0; tick <= 4; ++tick) {
                for (int y = 0; y < 9; ++y) {
                    for (int x = 0; x < 9; ++x) baseline[tick][y * 9 + x] = world_get(&worlds[0], x, y);
                }
                full_states[tick] = world_get(&worlds[1], 4, 4);
                if (mask == 3) {
                    /* Two adjacent cells die; adding the center makes an L
                       triomino that fills its fourth corner into a block. */
                    for (int y = 0; y < 9; ++y) {
                        for (int x = 0; x < 9; ++x) {
                            int pair = y == 3 && (x == 3 || x == 4);
                            int block = x >= 3 && x <= 4 && y >= 3 && y <= 4;
                            CHECK(baseline[tick][y * 9 + x] == (tick == 0 && pair));
                            CHECK(world_get(&worlds[1], x, y) ==
                                  (tick == 0 ? pair || (x == 4 && y == 4) : block));
                        }
                    }
                }
                if (tick < 4) {
                    for (int w = 0; w < 2; ++w) {
                        int expected = output[world_get(&worlds[w], 4, 4)][replay_center_count(&worlds[w])];
                        StepStats stats;
                        world_step(&worlds[w], &stats);
                        CHECK(world_get(&worlds[w], 4, 4) == expected);
                    }
                }
            }
            int first = 0;
            replay_states[0] = 1;
            for (int tick = 0; tick < 4; ++tick) {
                for (int w = 2; w < 4; ++w) {
                    for (int y = 0; y < 9; ++y) {
                        for (int x = 0; x < 9; ++x) {
                            if (x != 4 || y != 4) world_set(&worlds[w], x, y, baseline[tick][y * 9 + x]);
                        }
                    }
                    int expected = output[world_get(&worlds[w], 4, 4)][replay_center_count(&worlds[w])];
                    StepStats stats;
                    world_step(&worlds[w], &stats);
                    CHECK(world_get(&worlds[w], 4, 4) == expected);
                }
                CHECK(world_get(&worlds[2], 4, 4) == baseline[tick + 1][40]);
                if (mask == 3) {
                    for (int y = 0; y < 9; ++y) {
                        for (int x = 0; x < 9; ++x) {
                            int block = x >= 3 && x <= 4 && y >= 3 && y <= 4;
                            CHECK(world_get(&worlds[2], x, y) == 0);
                            CHECK(world_get(&worlds[3], x, y) == (tick == 0 && block));
                        }
                    }
                }
                replay_states[tick + 1] = world_get(&worlds[3], 4, 4);
                if (!first && replay_states[tick + 1] != full_states[tick + 1]) first = tick + 1;
            }
            CHECK(replay_states[1] == full_states[1]);
            if (mask == 3) CHECK(first == 2);
            if (first) {
                if (divergent == 0) {
                    printf("Boundary replay witness (%s): mask=%u first=%d full=",
                           toroidal ? "toroidal" : "bounded", mask, first);
                    for (int tick = 0; tick <= 4; ++tick) printf("%d", full_states[tick]);
                    printf(" replay=");
                    for (int tick = 0; tick <= 4; ++tick) printf("%d", replay_states[tick]);
                    printf("\n");
                }
                ++divergent;
            }
        }
        printf("Boundary replay (%s): 256 nominal matches through tick 4; "
               "%u perturbed histories diverge\n", toroidal ? "toroidal" : "bounded", divergent);
        for (int w = 0; w < 4; ++w) world_destroy(&worlds[w]);
    }
    passed = 1;
cleanup:
    for (int w = 0; w < 4; ++w) world_destroy(&worlds[w]);
    return passed;
}

static const TestCase cases[] = {
    {"empty-world", empty_world},
    {"isolated-cell", isolated_cell},
    {"local-process-partition", local_process_partition},
    {"source-process-networks", source_process_networks},
    {"independent-block-boundaries", independent_block_boundaries},
    {"minimal-sufficient-boundaries", minimal_sufficient_boundaries},
    {"minimal-sufficient-components", minimal_sufficient_components},
    {"periodic-minimal-components", periodic_minimal_components},
    {"whole-board-minimal-components", whole_board_minimal_components},
    {"native-non-null-components", native_non_null_components},
    {"paired-toad-components", paired_toad_components},
    {"paired-toad-interventions", paired_toad_interventions},
    {"block-support-certificates", block_support_certificates},
    {"block-regeneration-products", block_regeneration_products},
    {"boundary-replay", boundary_replay},
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
