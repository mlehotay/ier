#include "app.h"
#include "term.h"

#include <stdio.h>
#include <limits.h>
#include <string.h>

/* Capture the renderer's terminal boundary without a TTY or ANSI parser. */
static int screen_w, screen_h, cursor_x, cursor_y, invalid, writes, moves;
static char screen[24][128];

void term_move_cursor(int x, int y) {
    ++moves;
    if (x < 0 || y < 0 || x >= screen_w || y >= screen_h) invalid = 1;
    cursor_x = x;
    cursor_y = y;
}

void term_write_n(const char *s, size_t n) {
    ++writes;
    if (cursor_x < 0 || cursor_y < 0 || cursor_y >= screen_h ||
        n > (size_t)(screen_w - cursor_x) ||
        (cursor_y == screen_h - 1 && n >= (size_t)(screen_w - cursor_x))) {
        invalid = 1;
        return;
    }
    for (size_t i = 0; i < n; ++i) {
        if ((unsigned char)s[i] < 32 || (unsigned char)s[i] > 126) invalid = 1;
        if (cursor_y < 24 && cursor_x < 128) screen[cursor_y][cursor_x] = s[i];
        ++cursor_x;
    }
}

void term_hide_cursor(void) {}
void term_show_cursor(void) {}

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %dx%d: failed: %s\n", \
                __FILE__, __LINE__, screen_w, screen_h, #condition); \
        return 1; \
    } \
} while (0)

static int rect_inside(const Rect *r) {
    return r->x >= 0 && r->y >= 0 && r->w >= 0 && r->h >= 0 &&
           r->x + r->w <= screen_w && r->y + r->h <= screen_h;
}

static int render_bounds(void) {
    const int widths[] = {-1, 0, 1, 2, 19, 20, 59, 60, 80, 4096, 4097};
    const int heights[] = {-1, 0, 1, 2, 4, 5, 13, 14, 24};
    App app = {0};
    CHECK(world_init(&app.world, 4, 3, 1) == 0);
    world_randomize(&app.world, 42);
    app.startup.delay_ms = 100;
    memset(app.cmd.buf, 'x', sizeof(app.cmd.buf) - 1);
    strcpy(app.cmd.message, "Command result");
    unsigned scenarios = 0;

    for (size_t wi = 0; wi < sizeof(widths) / sizeof(widths[0]); ++wi) {
        for (size_t hi = 0; hi < sizeof(heights) / sizeof(heights[0]); ++hi) {
            ui_layout_compute(&app.layout, widths[wi], heights[hi]);
            screen_w = widths[wi] > 0 ? widths[wi] : 0;
            screen_h = heights[hi] > 0 ? heights[hi] : 0;
            CHECK(app.layout.screen_w == screen_w && app.layout.screen_h == screen_h);
            CHECK(rect_inside(&app.layout.cmd));
            CHECK(rect_inside(&app.layout.grid));
            CHECK(rect_inside(&app.layout.stats));
            CHECK(app.layout.stats_visible == (screen_w >= 60 && screen_h >= 14));
            app.terminal_too_small = app.layout.grid.w == 0 || app.layout.grid.h == 0;
            CHECK(app.terminal_too_small == (screen_w < 20 || screen_h < 5));
            for (int variant = 0; variant < 8; ++variant) {
                int active = variant & 1, stats = (variant >> 1) & 1;
                strcpy(app.cmd.message, (variant >> 2) & 1 ? "" : "Command result");
                app.cmd.active = active;
                app.show_stats = stats;
                invalid = writes = moves = 0;
                cursor_x = cursor_y = 0;
                memset(screen, '#', sizeof(screen));
                ui_draw_frame(&app);
                CHECK(!invalid);
                if (!screen_w || !screen_h) {
                    CHECK(writes == 0 && moves == 0);
                    continue;
                }
                for (int y = 0; y < screen_h; ++y) {
                    for (int x = 0; x < screen_w && x < 128; ++x) {
                        if (y == screen_h - 1 && x == screen_w - 1) {
                            CHECK(screen[y][x] == '#');
                        } else {
                            CHECK(screen[y][x] != '#');
                        }
                    }
                }
                if (app.terminal_too_small && screen_w >= 20 && screen_h >= 2) {
                    CHECK(memcmp(screen[0], "Terminal too small.", 19) == 0);
                }
                if (active && screen_w >= 10) {
                    int row = screen_h >= 2 ? 1 : 0;
                    CHECK(memcmp(screen[row], "(h=help):x", 10) == 0);
                }
                ++scenarios;
            }
        }
    }
    world_destroy(&app.world);
    printf("PASS render-bounds (%u frames: tiny, threshold, wide, input, feedback and stats modes)\n", scenarios);
    return 0;
}

static int stats_pane(void) {
    App app = {0};
    CHECK(world_init(&app.world, 4, 3, 0) == 0);
    world_set(&app.world, 0, 0, 1);
    world_set(&app.world, 1, 0, 1);
    world_set(&app.world, 2, 0, 1);
    world_recompute_stats(&app.world, &app.last_step);
    app.last_step.births = 2;
    app.last_step.deaths = 3;
    app.last_step.changed = 5;
    app.generation = 42;
    app.startup.delay_ms = 125;
    app.show_stats = 1;
    const int sizes[][2] = {{60, 14}, {80, 24}};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
        screen_w = sizes[i][0]; screen_h = sizes[i][1];
        ui_layout_compute(&app.layout, screen_w, screen_h);
        for (int paused = 0; paused <= 1; ++paused) {
            for (int toroidal = 0; toroidal <= 1; ++toroidal) {
                app.paused = paused;
                app.world.toroidal = toroidal;
                const char *lines[] = {"Stats", "gen: 42", "live: 3", "density: 0.2500",
                    "births: 2", "deaths: 3", "changed: 5",
                    paused ? "mode: paused" : "mode: running", "delay: 125ms", "world: 4x3",
                    toroidal ? "topology: toroidal" : "topology: bounded"};
                invalid = 0;
                memset(screen, '#', sizeof(screen));
                ui_draw_frame(&app);
                CHECK(!invalid);
                const Rect *r = &app.layout.stats;
                for (int y = 0; y < r->h; ++y) {
                    const char *line = y < 11 ? lines[y] : "";
                    size_t len = strlen(line);
                    for (int x = 0; x < r->w; ++x) {
                        if (r->y + y == screen_h - 1 && r->x + x == screen_w - 1) {
                            CHECK(screen[r->y + y][r->x + x] == '#');
                        } else {
                            CHECK(screen[r->y + y][r->x + x] ==
                                  ((size_t)x < len ? line[x] : ' '));
                        }
                    }
                }
                CHECK(app.generation == 42 && app.last_step.live_cells == 3);
                CHECK(app.last_step.births == 2 && app.last_step.deaths == 3 &&
                      app.last_step.changed == 5);
            }
        }
    }
    /* Full-width counters fit, and a shorter redraw erases their old digits. */
    app.generation = app.last_step.changed = ULONG_MAX;
    invalid = 0;
    ui_draw_frame(&app);
    char expected[64];
    snprintf(expected, sizeof(expected), "gen: %lu", ULONG_MAX);
    CHECK(memcmp(&screen[app.layout.stats.y + 1][app.layout.stats.x],
                 expected, strlen(expected)) == 0);
    snprintf(expected, sizeof(expected), "changed: %lu", ULONG_MAX);
    CHECK(strlen(expected) <= (size_t)app.layout.stats.w);
    CHECK(memcmp(&screen[app.layout.stats.y + 6][app.layout.stats.x],
                 expected, strlen(expected)) == 0);
    app.generation = app.last_step.changed = 0;
    ui_draw_frame(&app);
    for (int x = 0; x < app.layout.stats.w; ++x) {
        CHECK(screen[app.layout.stats.y + 6][app.layout.stats.x + x] ==
              (x < 10 ? "changed: 0"[x] : ' '));
    }
    app.show_stats = 0;
    ui_draw_frame(&app);
    for (int y = 0; y < app.layout.stats.h; ++y) {
        for (int x = 0; x < app.layout.stats.w; ++x) {
            if (app.layout.stats.y + y == screen_h - 1 &&
                app.layout.stats.x + x == screen_w - 1) continue;
            CHECK(screen[app.layout.stats.y + y][app.layout.stats.x + x] == ' ');
        }
    }
    CHECK(!invalid);
    world_destroy(&app.world);
    puts("PASS stats-pane (all fields, minimum pane, modes, counters and stale text)");
    return 0;
}

static int single_world(void) {
    App app = {0};
    const int sizes[][2] = {{80, 24}, {40, 10}};
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        CHECK(world_init(&app.world, 4, 3, toroidal) == 0);
        world_set(&app.world, 0, 0, 1);
        world_set(&app.world, 3, 2, 1);
        app.show_stats = 1;
        for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
            screen_w = sizes[i][0]; screen_h = sizes[i][1];
            ui_layout_compute(&app.layout, screen_w, screen_h);
            invalid = 0;
            /* Old tiled/live content must be blanked outside the real board. */
            memset(screen, 'O', sizeof(screen));
            ui_draw_frame(&app);
            CHECK(!invalid);
            for (int y = 0; y < app.layout.grid.h; ++y) {
                for (int x = 0; x < app.layout.grid.w; ++x) {
                    int sx = app.layout.grid.x + x, sy = app.layout.grid.y + y;
                    if (sx == screen_w - 1 && sy == screen_h - 1) continue;
                    char expected = (x == 0 && y == 0) || (x == 3 && y == 2) ? 'O' : ' ';
                    CHECK(screen[sy][sx] == expected);
                }
            }
            CHECK(app.world.w == 4 && app.world.h == 3 && app.world.toroidal == toroidal);
            CHECK(world_get(&app.world, 0, 0) && world_get(&app.world, 3, 2));
            /* Rendering bounds must not change simulation's neighbor wrapping. */
            CHECK(world_get(&app.world, -1, -1) == toroidal);
            CHECK(world_get(&app.world, 4, 3) == toroidal);
        }
        world_destroy(&app.world);
    }
    puts("PASS single-world (bounded/toroidal, stats visible/collapsed, stale content cleared)");
    return 0;
}

int main(int argc, char **argv) {
    const char *selection = argc == 2 ? argv[1] : "all";
    if (argc > 2 || (strcmp(selection, "all") != 0 &&
                    strcmp(selection, "render-bounds") != 0 &&
                    strcmp(selection, "stats-pane") != 0 &&
                    strcmp(selection, "single-world") != 0)) {
        fprintf(stderr, "Usage: %s [all|render-bounds|single-world|stats-pane]\n", argv[0]);
        return 2;
    }
    int failed = 0;
    if (strcmp(selection, "all") == 0 || strcmp(selection, "stats-pane") == 0) {
        failed |= stats_pane();
    }
    if (strcmp(selection, "all") == 0 || strcmp(selection, "render-bounds") == 0) {
        failed |= render_bounds();
    }
    if (strcmp(selection, "all") == 0 || strcmp(selection, "single-world") == 0) {
        failed |= single_world();
    }
    return failed;
}
