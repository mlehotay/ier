#define _POSIX_C_SOURCE 200809L
#include "app.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s: failed: %s\n", __FILE__, __LINE__, __func__, #condition); \
        goto cleanup; \
    } \
} while (0)

static void execute(App *app, const char *text) {
    cmd_enter(&app->cmd);
    snprintf(app->cmd.buf, sizeof(app->cmd.buf), "%s", text);
    app->needs_redraw = 0;
    cmd_execute(app);
}

static int finished(const App *app) {
    return !app->cmd.active && !app->cmd.len && !app->cmd.cursor && !app->cmd.buf[0] &&
           app->needs_redraw && app->cmd.message[0];
}

static int rejects_unchanged(App *app, const char *text) {
    App before = *app;
    uint8_t cells[600], next[600];
    size_t area = (size_t)app->world.w * (size_t)app->world.h;
    if (area > sizeof(cells)) return 0;
    memcpy(cells, app->world.cells, area);
    memcpy(next, app->world.next, area);
    execute(app, text);
    if (!finished(app) || strncmp(app->cmd.message, "Error:", 6)) return 0;
    App after = *app;
    after.cmd = before.cmd;
    after.needs_redraw = before.needs_redraw;
    return memcmp(&before, &after, sizeof(before)) == 0 &&
           memcmp(cells, app->world.cells, area) == 0 && memcmp(next, app->world.next, area) == 0;
}

static int snapshot_reset(const App *app, unsigned long live) {
    return app->last_step.live_cells == live && !app->last_step.births &&
           !app->last_step.deaths && !app->last_step.changed && !app->last_step.survivors;
}

static int command_controls(void) {
    App app = {0};
    World expected = {0};
    int passed = 0;
    CHECK(world_init(&app.world, 8, 6, 1) == 0);
    CHECK(world_init(&expected, 8, 6, 1) == 0);
    app.paused = 1;
    app.generation = 77;
    app.last_step.births = 5;
    app.startup.delay_ms = 100;
    app.startup.seed = 42;
    execute(&app, "  help  ");
    CHECK(finished(&app) && strstr(app.cmd.message, "randomize") && strstr(app.cmd.message, "load FILE"));
    execute(&app, "h");
    CHECK(finished(&app) && strstr(app.cmd.message, "seed N"));
    execute(&app, "stats on");
    CHECK(app.show_stats && app.last_step.births == 5 && app.generation == 77);
    execute(&app, "stats\toff");
    CHECK(!app.show_stats && app.last_step.births == 5);
    execute(&app, "bounded");
    CHECK(!app.world.toroidal && snapshot_reset(&app, 0) && app.generation == 77);
    execute(&app, "torus on");
    CHECK(app.world.toroidal);
    execute(&app, "torus off");
    CHECK(!app.world.toroidal);
    world_set(&app.world, 2, 3, 1);
    app.last_step.live_cells = 1;
    app.last_step.births = 7;
    memset(app.world.next, 1, 48);
    uint8_t before[48];
    memcpy(before, app.world.cells, sizeof(before));
    char text[64];
    snprintf(text, sizeof(text), "seed %u", UINT_MAX);
    execute(&app, text);
    CHECK(app.startup.seed == UINT_MAX && app.generation == 77 && app.paused);
    execute(&app, "seed 0");
    CHECK(app.startup.seed == 0 && app.generation == 77);
    snprintf(text, sizeof(text), "delay %d", INT_MAX);
    execute(&app, text);
    CHECK(app.startup.delay_ms == INT_MAX && app.generation == 77);
    execute(&app, "delay 1");
    CHECK(app.startup.delay_ms == 1);
    CHECK(memcmp(before, app.world.cells, sizeof(before)) == 0);
    CHECK(app.last_step.live_cells == 1 && app.last_step.births == 7);
    for (int i = 0; i < 48; ++i) CHECK(app.world.next[i] == 1);
    execute(&app, "randomize");
    world_randomize(&expected, 0);
    StepStats stats;
    world_recompute_stats(&expected, &stats);
    CHECK(memcmp(app.world.cells, expected.cells, 48) == 0 && app.generation == 0);
    CHECK(snapshot_reset(&app, stats.live_cells) && app.paused && !app.world.toroidal);
    app.generation = 9;
    app.last_step.births = 8;
    execute(&app, "clear");
    CHECK(snapshot_reset(&app, 0) && !app.generation && app.paused);
    for (int i = 0; i < 48; ++i) CHECK(!app.world.cells[i] && !app.world.next[i]);
    CHECK(finished(&app));
    passed = 1;
cleanup:
    world_destroy(&expected);
    world_destroy(&app.world);
    return passed;
}

static int command_patterns(void) {
    App app = {0};
    int passed = 0;
    for (int toroidal = 0; toroidal <= 1; ++toroidal) {
        CHECK(world_init(&app.world, 40, 15, toroidal) == 0);
        for (size_t i = 0; i < pattern_count(); ++i) {
            const Pattern *pattern = pattern_at(i);
            for (int placement = 0; placement < 3; ++placement) {
                world_clear(&app.world);
                world_set(&app.world, 39, 14, 1);
                memset(app.world.next, 1, 600);
                app.generation = 77;
                app.paused = 1;
                app.last_step.births = 3;
                char text[64];
                snprintf(text, sizeof(text), "pattern %s%s", pattern->name,
                         placement == 0 ? "" : placement == 1 ? " center" : " 0 0");
                execute(&app, text);
                CHECK(finished(&app) && strstr(app.cmd.message, "OK: Pattern placed"));
                int ox = placement == 2 ? 0 : (40 - pattern->width) / 2;
                int oy = placement == 2 ? 0 : (15 - pattern->height) / 2;
                for (int y = 0; y < 15; ++y) {
                    for (int x = 0; x < 40; ++x) {
                        int live = x == 39 && y == 14;
                        for (size_t c = 0; c < pattern->count; ++c) {
                            if (x == ox + pattern->cells[c].dx && y == oy + pattern->cells[c].dy) live = 1;
                        }
                        CHECK(world_get(&app.world, x, y) == live);
                        CHECK(app.world.next[y * 40 + x] == 1);
                    }
                }
                CHECK(snapshot_reset(&app, (unsigned long)pattern->count + 1));
                CHECK(app.generation == 77 && app.paused && app.world.toroidal == toroidal);
            }
        }
        CHECK(rejects_unchanged(&app, "pattern glider 39 14"));
        CHECK(rejects_unchanged(&app, "pattern glider 2147483647 0"));
        world_destroy(&app.world);
    }
    passed = 1;
cleanup:
    world_destroy(&app.world);
    return passed;
}

static int write_fixture(const char *path, const char *text) {
    FILE *file = fopen(path, "wb");
    if (!file) return 0;
    int ok = fputs(text, file) >= 0;
    return fclose(file) == 0 && ok;
}

static int command_files(void) {
    App app = {0};
    int passed = 0;
    char directory[] = "/tmp/gol-commands-XXXXXX", path[128] = "", text[256];
    CHECK(mkdtemp(directory) != NULL);
    snprintf(path, sizeof(path), "%s/with spaces.cells", directory);
    CHECK(world_init(&app.world, 8, 6, 1) == 0);
    CHECK(write_fixture(path, "! original test fixture\n..O\n...\nO..\n"));
    world_set(&app.world, 7, 5, 1);
    app.generation = 77;
    snprintf(text, sizeof(text), "load \"%s\"", path);
    execute(&app, text);
    CHECK(finished(&app) && strstr(app.cmd.message, "OK: File pattern placed"));
    CHECK(world_get(&app.world, 4, 1) && world_get(&app.world, 2, 3) && world_get(&app.world, 7, 5));
    CHECK(snapshot_reset(&app, 3) && app.generation == 77);
    snprintf(text, sizeof(text), "load '%s' 0 0", path);
    execute(&app, text);
    CHECK(world_get(&app.world, 2, 0) && world_get(&app.world, 0, 2));
    CHECK(snapshot_reset(&app, 5));
    snprintf(text, sizeof(text), "load '%s' center", path);
    execute(&app, text);
    CHECK(snapshot_reset(&app, 5));
    snprintf(text, sizeof(text), "load '%s' 7 5", path);
    CHECK(rejects_unchanged(&app, text));
    snprintf(text, sizeof(text), "load '%s'", path);
    CHECK(write_fixture(path, "O.X\n"));
    CHECK(rejects_unchanged(&app, text) && strstr(app.cmd.message, "Invalid .cells format") &&
          strstr(app.cmd.message, "line 1, column 3") && strstr(app.cmd.message, "with spaces.cells"));
    CHECK(write_fixture(path, "! empty\n...\n"));
    CHECK(rejects_unchanged(&app, text) && strstr(app.cmd.message, "Empty pattern"));
    CHECK(write_fixture(path, "OOOOOOOOO\n"));
    CHECK(rejects_unchanged(&app, text));
    CHECK(unlink(path) == 0);
    CHECK(rejects_unchanged(&app, text) && strstr(app.cmd.message, "I/O error") &&
          strstr(app.cmd.message, "Cannot open") && strstr(app.cmd.message, "with spaces.cells"));
    passed = 1;
cleanup:
    if (path[0]) unlink(path);
    rmdir(directory);
    world_destroy(&app.world);
    return passed;
}

static int command_feedback(void) {
    App app = {0};
    int passed = 0;
    CHECK(world_init(&app.world, 8, 6, 1) == 0);
    const struct { const char *input, *expected; } cases[] = {
        {"", "Error: Empty command."}, {"'broken", "Error: Malformed quoting"},
        {"what", "Error: Unknown command 'what'. Type 'help'."},
        {"stats", "Error: Usage: stats on|off."}, {"torus maybe", "Error: Usage: torus on|off."},
        {"delay", "Error: Usage: delay MS."}, {"seed extra args", "Error: Usage: seed N."},
        {"clear args", "Error: Usage: clear."}, {"pattern", "Error: Usage: pattern NAME"},
        {"pattern unknown", "Error: Unknown pattern 'unknown'; use block"},
        {"pattern glider 7 5", "World is 8x6; pattern is 3x3."},
        {"stats on", "OK: Stats pane enabled."}, {"stats off", "OK: Stats pane disabled."},
        {"torus on", "OK: Topology set to toroidal."}, {"bounded", "OK: Topology set to bounded."},
        {"seed 17", "OK: Seed set to 17; use randomize to apply."},
        {"delay 123", "OK: Delay set to 123ms."},
        {"clear", "OK: World cleared. 0 live cells; generation reset."},
        {"pattern block", "OK: Pattern placed at 3,2; 4 live cells."},
        {"pattern block 0 0", "OK: Pattern placed at 0,0; 8 live cells."},
        {"randomize", "OK: World randomized."}, {"help", "OK: Commands: help/h"},
    };
    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
        execute(&app, cases[i].input);
        CHECK(finished(&app) && strstr(app.cmd.message, cases[i].expected));
    }
    /* Untrusted tokens cannot inject terminal control bytes into feedback. */
    execute(&app, "'bad\033name'");
    CHECK(strstr(app.cmd.message, "bad?name") && !strchr(app.cmd.message, '\033'));
    for (size_t i = 0; app.cmd.message[i]; ++i) CHECK((unsigned char)app.cmd.message[i] >= 32);
    /* A full, otherwise-valid command must be rejected if input overflowed. */
    cmd_enter(&app.cmd);
    for (int i = 0; i < CMD_BUF_CAP - 6; ++i) cmd_insert_char(&app.cmd, ' ');
    const char *suffix = "clear";
    for (const char *p = suffix; *p; ++p) cmd_insert_char(&app.cmd, *p);
    CHECK(!app.cmd.overflow && app.cmd.len == CMD_BUF_CAP - 1);
    cmd_insert_char(&app.cmd, 'x');
    CHECK(app.cmd.overflow);
    uint8_t cells[48];
    memcpy(cells, app.world.cells, sizeof(cells));
    unsigned long gen = app.generation;
    cmd_execute(&app);
    CHECK(finished(&app) && strstr(app.cmd.message, "Error: Command exceeds 255"));
    CHECK(memcmp(cells, app.world.cells, sizeof(cells)) == 0 && app.generation == gen);
    CHECK(!app.cmd.overflow);
    passed = 1;
cleanup:
    world_destroy(&app.world);
    return passed;
}

static int command_invalid(void) {
    App app = {0};
    int passed = 0;
    CHECK(world_init(&app.world, 8, 6, 1) == 0);
    world_randomize(&app.world, 42);
    memset(app.world.next, 1, 48);
    app.generation = 77;
    app.paused = 1;
    app.startup.seed = 42;
    app.startup.delay_ms = 100;
    app.last_step = (StepStats){.live_cells = 12, .births = 2, .deaths = 3, .changed = 5};
    const char *bad[] = {"", "  ", "bogus", "help extra", "h extra", "stats", "stats maybe",
        "stats on extra", "torus", "torus maybe", "torus off extra", "bounded extra", "seed", "seed -1",
        "seed +1", "seed 1x", "seed 18446744073709551616", "seed 1 extra", "delay", "delay 0",
        "delay -1", "delay +1", "delay 2147483648", "delay 1x", "delay 2 extra",
        "pattern", "pattern unknown", "pattern Glider", "pattern glider 1", "pattern glider center extra",
        "pattern glider 0 -1", "pattern glider +1 0", "pattern glider 0 1x", "pattern glider 0 0 extra",
        "load", "load ''", "load \"unterminated", "load 'file'junk", "load a'b", "load file 1",
        "clear extra", "randomize extra", "a b c d e f"};
    for (size_t i = 0; i < sizeof(bad) / sizeof(bad[0]); ++i) CHECK(rejects_unchanged(&app, bad[i]));
    char text[64];
    snprintf(text, sizeof(text), "seed %llu", (unsigned long long)UINT_MAX + 1);
    CHECK(rejects_unchanged(&app, text));
    passed = 1;
cleanup:
    world_destroy(&app.world);
    return passed;
}

int main(int argc, char **argv) {
    const char *selection = argc == 2 ? argv[1] : "all";
    const struct { const char *name; int (*run)(void); } cases[] = {
        {"command-controls", command_controls}, {"command-patterns", command_patterns},
        {"command-files", command_files}, {"command-invalid", command_invalid},
        {"command-feedback", command_feedback},
    };
    int selected = 0, failed = 0;
    if (argc > 2) return 2;
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        if (strcmp(selection, "all") && strcmp(selection, cases[i].name)) continue;
        ++selected;
        if (cases[i].run()) printf("PASS %s\n", cases[i].name);
        else { ++failed; fprintf(stderr, "FAIL %s\n", cases[i].name); }
    }
    if (!selected) { fprintf(stderr, "Unknown/empty command case: '%s'\n", selection); return 2; }
    printf("Command tests: %d selected, %d passed, %d failed\n", selected, selected - failed, failed);
    return failed ? 1 : 0;
}
