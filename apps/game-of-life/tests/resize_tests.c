#define _POSIX_C_SOURCE 200809L
#include "app.h"
#include "cli.h"
#include "stats.h"
#include "term.h"

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

volatile sig_atomic_t term_resize_pending;
static int terminal_w = 80, terminal_h = 24, size_error;
static int allocation_call, fail_allocation;
static int cursor_x, cursor_y, invalid_output;
static char screen[40][120];

/* src/world.c alone is built with calloc redirected to this test hook. */
void *resize_test_calloc(size_t count, size_t size) {
    ++allocation_call;
    if (fail_allocation && allocation_call == fail_allocation) return NULL;
    return calloc(count, size);
}

int term_get_size(int *w, int *h) {
    if (size_error) return -1;
    *w = terminal_w;
    *h = terminal_h;
    return 0;
}
int term_read_key_nonblocking(void) { return -1; }
void term_hide_cursor(void) {}
void term_show_cursor(void) {}
void term_move_cursor(int x, int y) {
    if (x < 0 || y < 0 || x >= terminal_w || y >= terminal_h) invalid_output = 1;
    cursor_x = x;
    cursor_y = y;
}
void term_write_n(const char *text, size_t length) {
    if (cursor_x < 0 || cursor_x >= terminal_w || cursor_y < 0 || cursor_y >= terminal_h ||
        length > (size_t)(terminal_w - cursor_x) ||
        (cursor_y == terminal_h - 1 && length >= (size_t)(terminal_w - cursor_x))) {
        invalid_output = 1;
        return;
    }
    for (size_t i = 0; i < length; ++i) screen[cursor_y][cursor_x++] = text[i];
}

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s (%dx%d): failed: %s\n", \
                __FILE__, __LINE__, __func__, terminal_w, terminal_h, #condition); \
        goto cleanup; \
    } \
} while (0)

static int redraw_complete(const App *app) {
    invalid_output = 0;
    memset(screen, '#', sizeof(screen));
    ui_draw_frame(app);
    if (invalid_output) return 0;
    for (int y = 0; y < terminal_h; ++y) {
        for (int x = 0; x < terminal_w; ++x) {
            if (y == terminal_h - 1 && x == terminal_w - 1) {
                if (screen[y][x] != '#') return 0;
            } else if (screen[y][x] == '#') return 0;
        }
    }
    if (app->layout.stats_visible && app->show_stats) {
        char expected[64];
        snprintf(expected, sizeof(expected), "live: %lu", app->last_step.live_cells);
        if (memcmp(&screen[app->layout.stats.y + 2][app->layout.stats.x],
                   expected, strlen(expected)) != 0) return 0;
    }
    return 1;
}

static int overlap_matches(const App *app, const uint8_t *old, int old_w, int old_h) {
    unsigned long live = 0;
    for (int y = 0; y < app->world.h; ++y) {
        for (int x = 0; x < app->world.w; ++x) {
            int expected = x < old_w && y < old_h ? old[y * old_w + x] : 0;
            if (world_get(&app->world, x, y) != expected) return 0;
            if (app->world.next[y * app->world.w + x] != 0) return 0;
            live += (unsigned long)expected;
        }
    }
    const StepStats *s = &app->last_step;
    return s->live_cells == live && s->births == 0 && s->deaths == 0 &&
           s->survivors == 0 && s->changed == 0;
}

static int stats_match(const App *app, unsigned long generation,
                       unsigned long births, unsigned long deaths, unsigned long changed) {
    unsigned long live = 0;
    for (int y = 0; y < app->world.h; ++y) {
        for (int x = 0; x < app->world.w; ++x) {
            live += app->world.cells[y * app->world.w + x] != 0;
        }
    }
    StatsSnapshot s;
    stats_collect(app, &s);
    double density = (double)live / ((double)app->world.w * app->world.h);
    return s.generation == generation && s.live_cells == live && s.density == density &&
           s.births == births && s.deaths == deaths && s.changed == changed &&
           s.world_w == app->world.w && s.world_h == app->world.h &&
           s.paused == app->paused && s.delay_ms == app->startup.delay_ms &&
           s.toroidal == app->world.toroidal;
}

static int startup_matches(const App *app, const Pattern *pattern, int x, int y) {
    for (int wy = 0; wy < app->world.h; ++wy) {
        for (int wx = 0; wx < app->world.w; ++wx) {
            int expected = 0;
            for (size_t i = 0; i < pattern->count; ++i) {
                if (wx == x + pattern->cells[i].dx && wy == y + pattern->cells[i].dy) expected = 1;
            }
            if (world_get(&app->world, wx, wy) != expected || app->world.next[wy * app->world.w + wx]) return 0;
        }
    }
    return app->last_step.live_cells == pattern->count && stats_match(app, 0, 0, 0, 0) &&
           !app->paused && app->running && app->needs_redraw && app->show_stats &&
           !app->resized && !app->world_resize_pending && !app->cmd.active &&
           !app->cmd.message[0] && !app->cmd.len && !app->cmd.cursor && !app->cmd.overflow;
}

static int startup_order(void) {
    App app = {0};
    World reference = {0};
    int passed = 0;
    terminal_w = 80; terminal_h = 24;
    StartupConfig cfg;
    cli_startup_defaults(&cfg);
    char *args[] = {"gol", "--width", "8", "--height", "6", "--seed", "17",
                    "--delay", "123", "--bounded"};
    CHECK(cli_parse_args((int)(sizeof(args)/sizeof(args[0])), args, &cfg, stderr) == 0);
    /* A fresh object must not depend on the previous bytes of its storage. */
    memset(&app, 0xa5, sizeof(app));
    CHECK(app_init(&app, &cfg) == 0);
    CHECK(world_init(&reference, 8, 6, 0) == 0);
    world_randomize(&reference, 17);
    CHECK(app.world.w == 8 && app.world.h == 6 && !app.world.toroidal);
    CHECK(memcmp(app.world.cells, reference.cells, 48) == 0);
    CHECK(stats_match(&app, 0, 0, 0, 0) && !app.last_step.survivors);
    CHECK(app.startup.seed == 17 && app.startup.delay_ms == 123);
    CHECK(app.placement.centered && !app.placement.has_xy);
    CHECK(app.running && !app.paused && app.show_stats && app.needs_redraw);
    CHECK(!app.resized && !app.world_resize_pending && !app.terminal_too_small);
    CHECK(!app.cmd.active && !app.cmd.len && !app.cmd.cursor && !app.cmd.overflow &&
          !app.cmd.buf[0] && !app.cmd.message[0]);
    for (int i = 0; i < 48; ++i) CHECK(!app.world.next[i]);
    CHECK(redraw_complete(&app));
    app_destroy(&app);
    world_destroy(&reference);

    /* NULL config and CLI defaults initialize the same board and metadata. */
    cli_startup_defaults(&cfg);
    CHECK(app_init(&app, NULL) == 0);
    uint8_t defaults[49 * 21];
    CHECK(app.world.w == 49 && app.world.h == 21 && app.world.toroidal);
    memcpy(defaults, app.world.cells, sizeof(defaults));
    CHECK(app.startup.seed == cfg.seed && app.startup.delay_ms == cfg.delay_ms);
    CHECK(stats_match(&app, 0, 0, 0, 0));
    app_destroy(&app);
    CHECK(app_init(&app, &cfg) == 0);
    CHECK(memcmp(defaults, app.world.cells, sizeof(defaults)) == 0);
    app_destroy(&app);

    /* One explicit dimension leaves the other pane-derived. */
    cfg.width = 8;
    CHECK(app_init(&app, &cfg) == 0);
    CHECK(app.world.w == 8 && app.world.h == 21 && stats_match(&app, 0, 0, 0, 0));
    app_destroy(&app);
    cfg.width = 0; cfg.height = 6;
    CHECK(app_init(&app, &cfg) == 0);
    CHECK(app.world.w == 49 && app.world.h == 6);
    app_destroy(&app);

    cfg.height = 0;
    terminal_w = 1; terminal_h = 1; size_error = 1;
    CHECK(app_init(&app, &cfg) == 0);
    CHECK(app.layout.screen_w == 80 && app.layout.screen_h == 24 && !app.terminal_too_small);
    CHECK(memcmp(defaults, app.world.cells, sizeof(defaults)) == 0);
    app_destroy(&app);
    size_error = 0;
    CHECK(app_init(&app, &cfg) == 0);
    CHECK(app.world.w == 1 && app.world.h == 1 && app.terminal_too_small);
    CHECK(stats_match(&app, 0, 0, 0, 0) && app.running && app.needs_redraw);
    app_update(&app);
    CHECK(app.generation == 0);
    passed = 1;
cleanup:
    size_error = 0; terminal_w = 80; terminal_h = 24;
    world_destroy(&reference);
    app_destroy(&app);
    return passed;
}

static int startup_patterns(void) {
    App app = {0};
    int passed = 0;
    terminal_w = 80; terminal_h = 24;
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        for (size_t i = 0; i < pattern_count(); ++i) {
            const Pattern *pattern = pattern_at(i);
            for (int position = 0; position < 4; ++position) {
                StartupConfig cfg;
                cli_startup_defaults(&cfg);
                char width[16], height[16];
                snprintf(width, sizeof(width), "%d", pattern->width + (position == 3 ? 0 : 5));
                snprintf(height, sizeof(height), "%d", pattern->height + (position == 3 ? 0 : 4));
                char *args[] = {"gol", "--pattern", (char *)pattern->name,
                    "--width", width, "--height", height, "--seed", position % 2 ? "17" : "42",
                    toroidal ? "--toroidal" : "--bounded", "--delay", "123",
                    "--x", "1", "--y", "2", "--center"};
                if (position == 1) {
                    /* Center flag alone, with no explicit startup dimensions. */
                    char *center[] = {"gol", "--pattern", (char *)pattern->name, "--center",
                        toroidal ? "--toroidal" : "--bounded", "--seed", "0"};
                    CHECK(cli_parse_args(7, center, &cfg, stderr) == 0);
                } else {
                    CHECK(cli_parse_args(position == 2 ? 17 : 12, args, &cfg, stderr) == 0);
                }
                CHECK(app_init(&app, &cfg) == 0);
                int x = position == 2 ? 1 : (app.world.w - pattern->width) / 2;
                int y = position == 2 ? 2 : (app.world.h - pattern->height) / 2;
                CHECK(startup_matches(&app, pattern, x, y));
                CHECK(app.world.toroidal == toroidal && app.startup.seed == cfg.seed);
                if (position == 1) CHECK(app.world.w == 49 && app.world.h == 21);
                else CHECK(app.world.w == pattern->width + (position == 3 ? 0 : 5));
                CHECK(redraw_complete(&app));
                app_destroy(&app);
            }
        }
    }
    passed = 1;
cleanup:
    app_destroy(&app);
    return passed;
}

static int startup_rejects(const StartupConfig *cfg, const char *message) {
    App app = {0};
    FILE *capture = tmpfile();
    if (!capture) return 0;
    fflush(stderr);
    int saved = dup(STDERR_FILENO);
    if (saved < 0 || dup2(fileno(capture), STDERR_FILENO) < 0) {
        if (saved >= 0) close(saved);
        fclose(capture);
        return 0;
    }
    int result = app_init(&app, cfg);
    fflush(stderr);
    int restored = dup2(saved, STDERR_FILENO);
    close(saved);
    rewind(capture);
    char text[1024] = {0};
    size_t size = fread(text, 1, sizeof(text) - 1, capture);
    fclose(capture);
    int ok = restored >= 0 && result == -1 && size && strstr(text, message) &&
             !strchr(text, '\033') && !app.world.cells && !app.world.next &&
             !app.world.w && !app.world.h;
    app_destroy(&app);
    return ok;
}

static int startup_fixture(const char *path, const char *text) {
    FILE *stream = fopen(path, "wb");
    if (!stream) return 0;
    int ok = fputs(text, stream) >= 0;
    return fclose(stream) == 0 && ok;
}

static int startup_files(void) {
    App app = {0};
    int passed = 0;
    char directory[] = "/tmp/gol-startup-XXXXXX", path[256] = "";
    terminal_w = 80; terminal_h = 24;
    CHECK(mkdtemp(directory) != NULL);
    snprintf(path, sizeof(path), "%s/with spaces.cells", directory);
    CHECK(startup_fixture(path, "! original fixture\n....\n...O\n....\n.O..\n"));
    const PatternCell offsets[] = {{2, 0}, {0, 2}};
    const Pattern reference = {"fixture", offsets, 2, 3, 3};
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        for (int explicit = 0; explicit <= 1; ++explicit) {
            StartupConfig cfg;
            cli_startup_defaults(&cfg);
            char *args[] = {"gol", "--load", path, "--width", "8", "--height", "7",
                toroidal ? "--toroidal" : "--bounded", "--seed", explicit ? "99" : "0",
                "--center", "--x", "5", "--y", "4"};
            CHECK(cli_parse_args(explicit ? 15 : 11, args, &cfg, stderr) == 0);
            CHECK(app_init(&app, &cfg) == 0);
            CHECK(startup_matches(&app, &reference, explicit ? 5 : 2, explicit ? 4 : 2));
            CHECK(app.world.w == 8 && app.world.h == 7 && app.world.toroidal == toroidal);
            CHECK(redraw_complete(&app));
            app_destroy(&app);
        }
    }
    StartupConfig cfg;
    cli_startup_defaults(&cfg);
    cfg.mode = STARTUP_LOAD;
    strcpy(cfg.load_path, path);
    CHECK(startup_fixture(path, "O.X\n"));
    CHECK(startup_rejects(&cfg, "line 1, column 3"));
    CHECK(startup_fixture(path, "! empty\n...\n"));
    CHECK(startup_rejects(&cfg, "no live cells"));
    CHECK(startup_fixture(path, "OOOO\n"));
    cfg.width = 3;
    CHECK(startup_rejects(&cfg, "outside the world"));
    cfg.width = 8;
    cfg.placement = (PlacementConfig){.has_xy = 1, .centered = 1, .x = 7, .y = 0};
    CHECK(startup_rejects(&cfg, "outside the world"));
    CHECK(unlink(path) == 0);
    CHECK(startup_rejects(&cfg, "Cannot open"));
    passed = 1;
cleanup:
    if (path[0]) unlink(path);
    rmdir(directory);
    app_destroy(&app);
    return passed;
}

static int startup_failures(void) {
    StartupConfig cfg;
    int passed = 0;
    terminal_w = 80; terminal_h = 24;
    cli_startup_defaults(&cfg);
    cfg.mode = STARTUP_PATTERN;
    strcpy(cfg.pattern_name, "unknown");
    CHECK(startup_rejects(&cfg, "unknown pattern"));
    strcpy(cfg.pattern_name, "glider");
    cfg.width = cfg.height = 2;
    CHECK(startup_rejects(&cfg, "outside the world"));
    cfg.width = cfg.height = 8;
    cfg.placement = (PlacementConfig){.has_xy = 1, .centered = 1, .x = INT_MAX, .y = 0};
    CHECK(startup_rejects(&cfg, "outside the world"));
    cfg.placement.x = -1;
    CHECK(startup_rejects(&cfg, "outside the world"));
    cfg.placement.has_xy = 0;
    for (int allocation = 1; allocation <= 2; ++allocation) {
        allocation_call = 0; fail_allocation = allocation;
        CHECK(startup_rejects(&cfg, "cannot allocate"));
    }
    fail_allocation = 0;
    terminal_w = 19; terminal_h = 4;
    cfg.width = cfg.height = 0;
    CHECK(startup_rejects(&cfg, "outside the world"));
    terminal_w = 80; terminal_h = 24;
    cfg.mode = (StartupMode)99;
    CHECK(startup_rejects(&cfg, "invalid startup mode"));
    passed = 1;
cleanup:
    fail_allocation = 0;
    terminal_w = 80; terminal_h = 24;
    return passed;
}

static int command_help(void) {
    App app = {0};
    StartupConfig cfg = {.width = 8, .height = 6, .seed = 42, .delay_ms = 100};
    int passed = 0;
    terminal_w = 80; terminal_h = 24;
    CHECK(app_init(&app, &cfg) == 0);
    app.paused = 1;
    app.generation = 77;
    uint8_t cells[48], next[48];
    memcpy(cells, app.world.cells, sizeof(cells));
    memcpy(next, app.world.next, sizeof(next));
    StepStats stats = app.last_step;
    Layout layout = app.layout;
    app_handle_key(&app, 'h');
    CHECK(redraw_complete(&app) && memcmp(screen[0], "[PAUSED]", 8) == 0);
    CHECK(memcmp(screen[1], "q quit", 6) == 0);
    app_handle_key(&app, ':');
    app_handle_key(&app, 'h');
    CHECK(app.cmd.active && !strcmp(app.cmd.buf, "h"));
    CHECK(redraw_complete(&app) && memcmp(screen[1], "(h=help):h", 10) == 0);
    app_handle_key(&app, '\n');
    CHECK(!app.cmd.active && strstr(app.cmd.message, "OK: Commands: help/h"));
    CHECK(redraw_complete(&app) && memcmp(screen[1], "OK: Commands:", 13) == 0);
    app_handle_key(&app, 'h');
    CHECK(!app.cmd.message[0] && redraw_complete(&app));
    CHECK(memcmp(screen[1], "q quit", 6) == 0);
    app_handle_key(&app, ':');
    app_handle_key(&app, 27);
    CHECK(redraw_complete(&app) && memcmp(screen[1], "q quit", 6) == 0);
    CHECK(app.paused && app.generation == 77 && app.startup.delay_ms == 100);
    CHECK(memcmp(&layout, &app.layout, sizeof(layout)) == 0);
    CHECK(memcmp(cells, app.world.cells, sizeof(cells)) == 0);
    CHECK(memcmp(next, app.world.next, sizeof(next)) == 0);
    CHECK(memcmp(&stats, &app.last_step, sizeof(stats)) == 0);
    app_handle_key(&app, ':');
    for (int i = 0; i < CMD_BUF_CAP; ++i) app_handle_key(&app, 'x');
    CHECK(app.cmd.overflow && redraw_complete(&app));
    CHECK(memcmp(screen[1], "Error: command too long", 23) == 0);
    app_handle_key(&app, '\n');
    CHECK(strstr(app.cmd.message, "Error: Command exceeds") && !app.cmd.active);
    CHECK(memcmp(cells, app.world.cells, sizeof(cells)) == 0 && app.generation == 77);
    passed = 1;
cleanup:
    app_destroy(&app);
    return passed;
}

static int command_routing(void) {
    App app = {0};
    StartupConfig cfg = {.width = 8, .height = 6, .seed = 42, .delay_ms = 100};
    int passed = 0;
    terminal_w = 80; terminal_h = 24;
    CHECK(app_init(&app, &cfg) == 0);
    app.paused = 1;
    const char *commands[] = {"clear", "pattern glider 0 0", "seed 17", "randomize", "delay 250"};
    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i) {
        app_handle_key(&app, ':');
        CHECK(app.cmd.active && app.cmd.message[0] == '\0');
        for (const char *p = commands[i]; *p; ++p) app_handle_key(&app, *p);
        app_handle_key(&app, '\r');
        CHECK(!app.cmd.active && app.cmd.message[0] && app.needs_redraw && app.paused);
        CHECK(redraw_complete(&app));
        if (i == 0) CHECK(app.last_step.live_cells == 0 && stats_match(&app, 0, 0, 0, 0));
        if (i == 1) CHECK(app.last_step.live_cells == 5 && world_get(&app.world, 1, 0));
    }
    CHECK(app.startup.seed == 17 && app.startup.delay_ms == 250 && app.generation == 0);
    app_handle_key(&app, ':');
    const char *text = "delay 2x";
    for (const char *p = text; *p; ++p) app_handle_key(&app, *p);
    app_handle_key(&app, 127);
    app_handle_key(&app, '\n');
    CHECK(app.startup.delay_ms == 2);
    app_handle_key(&app, ':');
    app_handle_key(&app, 'q');
    app_handle_key(&app, 27);
    CHECK(!app.cmd.active && app.running);
    /* Decimal command maxima also remain safe with normal-mode delay keys. */
    char maximum[64];
    snprintf(maximum, sizeof(maximum), "delay %d", INT_MAX);
    app_handle_key(&app, ':');
    for (const char *p = maximum; *p; ++p) app_handle_key(&app, *p);
    app_handle_key(&app, '\n');
    app.cmd.message[0] = '\0';
    app_handle_key(&app, '-');
    CHECK(app.startup.delay_ms == INT_MAX);
    app_handle_key(&app, '+');
    CHECK(app.startup.delay_ms == INT_MAX - 10);
    CHECK(stats_match(&app, 0, 0, 0, 0));
    passed = 1;
cleanup:
    app_destroy(&app);
    return passed;
}

static int stats_lifecycle(void) {
    App app = {0};
    StartupConfig cfg = {.width = 7, .height = 7, .seed = 42, .delay_ms = 125};
    int passed = 0;
    terminal_w = 80; terminal_h = 24;
    CHECK(app_init(&app, &cfg) == 0);
    /* Startup statistics must describe the board before its first tick. */
    CHECK(stats_match(&app, 0, 0, 0, 0));
    uint8_t initial[49];
    memcpy(initial, app.world.cells, sizeof(initial));
    CHECK(redraw_complete(&app));
    app_handle_key(&app, 'c');
    CHECK(stats_match(&app, 0, 0, 0, 0) && app.last_step.live_cells == 0);
    CHECK(redraw_complete(&app));

    /* One oscillator and a distant isolated cell give known nonzero counts. */
    world_set(&app.world, 2, 3, 1);
    world_set(&app.world, 3, 3, 1);
    world_set(&app.world, 4, 3, 1);
    world_set(&app.world, 0, 0, 1);
    world_recompute_stats(&app.world, &app.last_step);
    app_update(&app);
    CHECK(stats_match(&app, 1, 2, 3, 5) && app.last_step.live_cells == 3);
    CHECK(redraw_complete(&app));
    app_handle_key(&app, ' ');
    CHECK(app.paused && stats_match(&app, 1, 2, 3, 5));
    app_update(&app);
    CHECK(stats_match(&app, 1, 2, 3, 5));
    app_handle_key(&app, '+');
    CHECK(app.startup.delay_ms == 115 && stats_match(&app, 1, 2, 3, 5));
    app_handle_key(&app, '-');
    CHECK(app.startup.delay_ms == 125 && stats_match(&app, 1, 2, 3, 5));
    app_handle_key(&app, 's');
    CHECK(!app.show_stats && stats_match(&app, 1, 2, 3, 5));
    CHECK(redraw_complete(&app));
    app_handle_key(&app, 's');
    CHECK(app.show_stats && stats_match(&app, 1, 2, 3, 5));
    app_handle_key(&app, 'n');
    CHECK(stats_match(&app, 2, 2, 2, 4) && app.last_step.live_cells == 3);
    CHECK(redraw_complete(&app));

    cmd_enter(&app.cmd);
    strcpy(app.cmd.buf, "torus on");
    cmd_execute(&app);
    CHECK(app.world.toroidal && stats_match(&app, 2, 0, 0, 0));
    CHECK(redraw_complete(&app));
    cmd_enter(&app.cmd);
    strcpy(app.cmd.buf, "torus off");
    cmd_execute(&app);
    CHECK(!app.world.toroidal && stats_match(&app, 2, 0, 0, 0));
    app.cmd.message[0] = '\0';
    app_handle_key(&app, 'r');
    CHECK(memcmp(initial, app.world.cells, sizeof(initial)) == 0);
    CHECK(stats_match(&app, 0, 0, 0, 0) && app.paused);
    CHECK(redraw_complete(&app));
    app_handle_key(&app, 'c');
    CHECK(stats_match(&app, 0, 0, 0, 0) && app.last_step.live_cells == 0);
    CHECK(redraw_complete(&app));
    /* Geometry changes update the density denominator immediately. */
    world_set(&app.world, 0, 0, 1);
    world_recompute_stats(&app.world, &app.last_step);
    terminal_w = 60; terminal_h = 14;
    term_resize_pending = 1;
    app_handle_resize(&app);
    CHECK(app.world.w == 29 && app.world.h == 11 && app.last_step.live_cells == 1);
    CHECK(stats_match(&app, 0, 0, 0, 0) && redraw_complete(&app));
    passed = 1;
cleanup:
    term_resize_pending = 0;
    app_destroy(&app);
    return passed;
}

static int resize_transitions(void) {
    const int sizes[][2] = {{59,24}, {60,13}, {60,14}, {20,5}, {19,4},
                            {1,1}, {0,0}, {100,30}, {80,24}};
    App app = {0};
    int passed = 0;
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        for (int paused = 0; paused <= 1; ++paused) {
            StartupConfig cfg = {.seed = 42, .delay_ms = 100, .toroidal = toroidal};
            terminal_w = 80; terminal_h = 24;
            CHECK(app_init(&app, &cfg) == 0);
            world_clear(&app.world);
            for (int y = 0; y < app.world.h; ++y) {
                for (int x = 0; x < app.world.w; ++x) {
                    world_set(&app.world, x, y, (x + y) % 5 == 0);
                }
            }
            /* Resize an evolved board with a populated scratch buffer. */
            world_step(&app.world, &app.last_step);
            world_set(&app.world, 0, 0, 1);
            app.paused = paused;
            app.show_stats = !paused;
            app.cmd.active = paused;
            strcpy(app.cmd.buf, "stats off");
            app.generation = 77;
            for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
                uint8_t before[40 * 120];
                int old_w = app.world.w, old_h = app.world.h;
                memcpy(before, app.world.cells, (size_t)old_w * (size_t)old_h);
                terminal_w = sizes[i][0]; terminal_h = sizes[i][1];
                term_resize_pending = 1;
                app_handle_resize(&app);
                CHECK(!term_resize_pending && !app.world_resize_pending);
                CHECK(app.resized && app.needs_redraw);
                CHECK(app.world.w == (app.layout.grid.w > 0 ? app.layout.grid.w : 1));
                CHECK(app.world.h == (app.layout.grid.h > 0 ? app.layout.grid.h : 1));
                CHECK(overlap_matches(&app, before, old_w, old_h));
                CHECK(app.world.toroidal == toroidal && app.generation == 77);
                CHECK(app.paused == paused && app.cmd.active == paused && app.show_stats == !paused);
                CHECK(strcmp(app.cmd.buf, "stats off") == 0);
                CHECK(redraw_complete(&app));
                if (app.terminal_too_small || paused) {
                    app_update(&app);
                    CHECK(app.generation == 77);
                }
                app.resized = app.needs_redraw = 0;
            }
            /* Shrinking to the fallback retains only (0,0); growing cannot
               resurrect discarded cells. The isolated survivor dies on resume. */
            CHECK(app.last_step.live_cells == 1);
            if (!paused) {
                app_update(&app);
                CHECK(app.generation == 78 && app.last_step.live_cells == 0);
                CHECK(app.last_step.deaths == 1 && app.last_step.changed == 1);
            }
            app_destroy(&app);
        }
    }
    passed = 1;
cleanup:
    app_destroy(&app);
    return passed;
}

static int resize_noop(void) {
    StartupConfig cfg = {.width = 8, .height = 6, .seed = 42, .delay_ms = 100, .toroidal = 1};
    App app = {0};
    int passed = 0;
    terminal_w = 80; terminal_h = 24;
    CHECK(app_init(&app, &cfg) == 0);
    uint8_t before[48];
    memcpy(before, app.world.cells, sizeof(before));
    app.resized = app.needs_redraw = 0;
    app.last_step.births = 7;
    term_resize_pending = 0;
    app_handle_resize(&app);
    term_resize_pending = 1;
    app_handle_resize(&app);
    CHECK(!app.resized && !app.needs_redraw && !term_resize_pending);
    CHECK(app.world.w == 8 && app.world.h == 6);
    CHECK(memcmp(before, app.world.cells, sizeof(before)) == 0 && app.last_step.births == 7);
    terminal_w = 81;
    term_resize_pending = 1;
    app_handle_resize(&app);
    CHECK(app.world.w == 50 && app.world.h == 21);
    CHECK(overlap_matches(&app, before, 8, 6));
    /* Collapsing stats can leave exactly the same world geometry. */
    uint8_t *cells = app.world.cells;
    app.last_step.births = 7;
    terminal_w = 50;
    term_resize_pending = 1;
    app_handle_resize(&app);
    CHECK(app.resized && app.needs_redraw && !app.layout.stats_visible);
    CHECK(app.world.cells == cells && app.last_step.births == 7);
    CHECK(app.world.w == 50 && app.world.h == 21 && redraw_complete(&app));
    passed = 1;
cleanup:
    app_destroy(&app);
    return passed;
}

static int resize_failures(void) {
    App app = {0};
    StartupConfig cfg = {.seed = 42, .delay_ms = 100};
    int passed = 0;
    terminal_w = 80; terminal_h = 24;
    CHECK(app_init(&app, &cfg) == 0);
    uint8_t before[40 * 120];
    int old_w = app.world.w, old_h = app.world.h;
    memcpy(before, app.world.cells, (size_t)old_w * (size_t)old_h);
    uint8_t *cells = app.world.cells, *next = app.world.next;
    terminal_w = 59; terminal_h = 13;
    size_error = term_resize_pending = 1;
    app.resized = app.needs_redraw = 0;
    app_handle_resize(&app);
    CHECK(term_resize_pending && app.layout.screen_w == 80);
    CHECK(app.world.cells == cells && !app.resized && !app.needs_redraw);
    app_update(&app);
    CHECK(app.generation == 0);
    size_error = 0;
    for (int failure = 1; failure <= 2; ++failure) {
        allocation_call = 0; fail_allocation = failure;
        app_handle_resize(&app);
        CHECK(app.world_resize_pending && app.resized && app.needs_redraw);
        CHECK(app.world.cells == cells && app.world.next == next);
        CHECK(app.world.w == old_w && app.world.h == old_h);
        CHECK(memcmp(before, app.world.cells, (size_t)old_w * (size_t)old_h) == 0);
        CHECK(redraw_complete(&app));
        app_update(&app);
        CHECK(app.generation == 0);
        app.cmd.message[0] = '\0';
        app.paused = 1;
        app_handle_key(&app, 'n');
        CHECK(app.generation == 0);
    }
    fail_allocation = 0;
    app_handle_resize(&app);
    CHECK(!app.world_resize_pending && !term_resize_pending);
    CHECK(overlap_matches(&app, before, old_w, old_h));
    CHECK(redraw_complete(&app));
    /* A failed resize can also be cancelled by returning to the retained
       geometry, without a new allocation or lost cell state. */
    cells = app.world.cells;
    old_w = app.world.w; old_h = app.world.h;
    memcpy(before, cells, (size_t)old_w * (size_t)old_h);
    terminal_w = 80; terminal_h = 24;
    term_resize_pending = 1;
    allocation_call = 0; fail_allocation = 1;
    app_handle_resize(&app);
    CHECK(app.world_resize_pending);
    terminal_w = 59; terminal_h = 13;
    term_resize_pending = 1;
    fail_allocation = 0;
    app_handle_resize(&app);
    CHECK(!app.world_resize_pending && app.cmd.message[0] == '\0');
    CHECK(app.world.cells == cells && app.world.w == old_w && app.world.h == old_h);
    CHECK(memcmp(before, cells, (size_t)old_w * (size_t)old_h) == 0);
    CHECK(redraw_complete(&app));
    passed = 1;
cleanup:
    size_error = fail_allocation = term_resize_pending = 0;
    app_destroy(&app);
    return passed;
}

int main(int argc, char **argv) {
    const char *selection = argc == 2 ? argv[1] : "all";
    const struct { const char *name; int (*run)(void); } cases[] = {
        {"startup-order", startup_order},
        {"command-help", command_help},
        {"startup-patterns", startup_patterns},
        {"startup-files", startup_files},
        {"startup-failures", startup_failures},
        {"command-routing", command_routing},
        {"stats-lifecycle", stats_lifecycle},
        {"resize-transitions", resize_transitions},
        {"resize-noop", resize_noop},
        {"resize-failures", resize_failures},
    };
    int selected = 0, failed = 0;
    if (argc > 2) return 2;
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        if (strcmp(selection, "all") && strcmp(selection, cases[i].name)) continue;
        ++selected;
        if (cases[i].run()) printf("PASS %s\n", cases[i].name);
        else {
            ++failed;
            fprintf(stderr, "FAIL %s; reproduce: make test-integration TEST_CASE=%s\n",
                    cases[i].name, cases[i].name);
        }
    }
    if (!selected) {
        fprintf(stderr, "Unknown/empty resize case: '%s'\n", selection);
        return 2;
    }
    printf("Resize tests: %d selected, %d passed, %d failed\n", selected, selected - failed, failed);
    return failed ? 1 : 0;
}
