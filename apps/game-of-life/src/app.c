#define _POSIX_C_SOURCE 200809L

#include "app.h"

#include "term.h"
#include "cells.h"
#include "cli.h"

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static void sleep_ms(int ms) {
    if (ms <= 0) return;

    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

static int app_grid_world_width(const App *app) {
    if (!app) return 1;
    return (app->layout.grid.w > 0) ? app->layout.grid.w : 1;
}

static int app_grid_world_height(const App *app) {
    if (!app) return 1;
    return (app->layout.grid.h > 0) ? app->layout.grid.h : 1;
}

static void app_recompute_world_stats(App *app) {
    if (!app) return;
    world_recompute_stats(&app->world, &app->last_step);
}

static void app_update_terminal_small_flag(App *app) {
    if (!app) return;
    app->terminal_too_small =
        (app->layout.grid.w <= 0 || app->layout.grid.h <= 0);
}

static void app_init_defaults(App *app) {
    if (!app) return;

    cli_startup_defaults(&app->startup);

    app->paused = 0;
    app->running = 1;
    app->show_stats = 1;
    app->needs_redraw = 1;
    app->resized = 0;
    app->terminal_too_small = 0;
    app->generation = 0;

    memset(&app->last_step, 0, sizeof(app->last_step));
}

static int app_apply_startup_initializer(App *app) {
    if (!app) return -1;

    /* Exactly one source initializes a fresh board; seed applies only to random. */
    world_clear(&app->world);
    CellsPattern owned = {0};
    Pattern view;
    const Pattern *pattern = NULL;
    switch (app->startup.mode) {
        case STARTUP_RANDOM:
            world_randomize(&app->world, app->startup.seed);
            break;
        case STARTUP_PATTERN:
            pattern = pattern_lookup(app->startup.pattern_name);
            if (!pattern) {
                fprintf(stderr, "error: unknown pattern '%s'\n", app->startup.pattern_name);
                return -1;
            }
            break;
        case STARTUP_LOAD: {
            CellsError error;
            if (cells_load(app->startup.load_path, &owned, &error) != CELLS_OK) {
                char detail[640];
                cells_format_error(app->startup.load_path, &error, detail, sizeof(detail));
                fprintf(stderr, "error: Load failed: %s\n", detail);
                cells_pattern_destroy(&owned);
                return -1;
            }
            view = (Pattern){app->startup.load_path, owned.cells, owned.count, owned.width, owned.height};
            pattern = &view;
            break;
        }
        default:
            fprintf(stderr, "error: invalid startup mode\n");
            return -1;
    }
    if (pattern) {
        PatternPlaceResult result = pattern_apply(&app->world, pattern, &app->placement);
        cells_pattern_destroy(&owned);
        if (result != PATTERN_PLACE_OK) {
            char source[256];
            snprintf(source, sizeof(source), "%s", pattern->name);
            for (char *p = source; *p; ++p) {
                if ((unsigned char)*p < 32 || (unsigned char)*p >= 127) *p = '?';
            }
            fprintf(stderr, "error: %s World is %dx%d; pattern is %dx%d; source '%s'.\n",
                    pattern_place_message(result), app->world.w, app->world.h, pattern->width, pattern->height, source);
            return -1;
        }
    }

    return 0;
}

static int app_create_startup_world(App *app) {
    /* Resolve geometry before allocation; failed startup queries retain the
       established default geometry, independently of partial query outputs. */
    int term_w = 80;
    int term_h = 24;
    if (term_get_size(&term_w, &term_h) != 0) {
        term_w = 80;
        term_h = 24;
    }
    ui_layout_compute(&app->layout, term_w, term_h);

    /* gol-0001 policy: the world is the visible board. There is no off-screen
       world and no panning. Terminal resize changes world dimensions. */
    int world_w = app_grid_world_width(app);
    int world_h = app_grid_world_height(app);

    if (app->startup.width > 0) world_w = app->startup.width;
    if (app->startup.height > 0) world_h = app->startup.height;

    if (world_w > app_grid_world_width(app) ||
        world_h > app_grid_world_height(app)) {
        fprintf(stderr,
                "error: requested world size %dx%d exceeds visible grid %dx%d\n",
                world_w, world_h,
                app_grid_world_width(app), app_grid_world_height(app));
        return -1;
    }

    if (world_init(&app->world, world_w, world_h, 0) != 0) {
        fprintf(stderr, "error: cannot allocate startup world\n");
        return -1;
    }
    return 0;
}

int app_init(App *app, const StartupConfig *startup) {
    if (!app) return -1;
    memset(app, 0, sizeof(*app));
    app_init_defaults(app);
    if (startup) app->startup = *startup;
    app->placement = app->startup.placement;
    app->placement.centered = !app->placement.has_xy;
    cmd_init(&app->cmd);

    /* main parses CLI first; resolve/create, apply topology, select exactly
       one source, then finalize statistics and layout state before TTY setup. */
    if (app_create_startup_world(app) != 0) return -1;
    app->world.toroidal = app->startup.toroidal;
    if (app_apply_startup_initializer(app) != 0) {
        world_destroy(&app->world);
        return -1;
    }
    app->generation = 0;
    app_recompute_world_stats(app);
    app_update_terminal_small_flag(app);
    app->needs_redraw = 1;
    return 0;
}

void app_destroy(App *app) {
    if (!app) return;
    world_destroy(&app->world);
}

void app_handle_resize(App *app) {
    if (!app) return;
    if (!term_resize_pending && !app->world_resize_pending) return;

    term_resize_pending = 0;

    int term_w = 0;
    int term_h = 0;
    if (term_get_size(&term_w, &term_h) != 0) {
        term_resize_pending = 1;
        return;
    }

    /* SIGWINCH can arrive without a geometry change. In particular, leave an
       explicitly smaller startup world alone until the terminal really changes. */
    if (!app->world_resize_pending && term_w == app->layout.screen_w &&
        term_h == app->layout.screen_h) return;

    ui_layout_compute(&app->layout, term_w, term_h);
    app_update_terminal_small_flag(app);
    app->resized = 1;

    const int new_w = app_grid_world_width(app);
    const int new_h = app_grid_world_height(app);

    if (new_w != app->world.w || new_h != app->world.h) {
        if (world_resize(&app->world, new_w, new_h) != 0) {
            app->world_resize_pending = 1;
            cmd_set_message(&app->cmd, "Resize failed; retrying.");
        } else {
            app->world_resize_pending = 0;
            app_recompute_world_stats(app);
            if (strcmp(app->cmd.message, "Resize failed; retrying.") == 0) {
                app->cmd.message[0] = '\0';
            }
        }
    } else {
        /* A later window change can return to the retained world's dimensions. */
        app->world_resize_pending = 0;
        if (strcmp(app->cmd.message, "Resize failed; retrying.") == 0) {
            app->cmd.message[0] = '\0';
        }
    }

    app->needs_redraw = 1;
}

void app_handle_key(App *app, int ch) {
    if (!app || ch < 0) return;

    if (app->cmd.active) {
        if (ch == 27) {
            cmd_cancel(&app->cmd);
        } else if (ch == '\r' || ch == '\n') {
            cmd_execute(app);
        } else if (ch == 127 || ch == '\b') {
            cmd_backspace(&app->cmd);
        } else if (ch >= 32 && ch <= 126) {
            cmd_insert_char(&app->cmd, ch);
        }
        app->needs_redraw = 1;
        return;
    }

    if (app->cmd.message[0] != '\0') {
        if (ch == ':') {
            cmd_enter(&app->cmd);
            app->needs_redraw = 1;
            return;
        }

        app->cmd.message[0] = '\0';
        app->needs_redraw = 1;
        return;
    }

    switch (ch) {
        case 'q':
            app->running = 0;
            break;
        case ' ':
            app->paused = !app->paused;
            app->needs_redraw = 1;
            break;
        case 'n':
            if (app->paused && !app->world_resize_pending && !term_resize_pending) {
                world_step(&app->world, &app->last_step);
                app->generation++;
                app->needs_redraw = 1;
            }
            break;
        case 'r':
            world_randomize(&app->world, app->startup.seed);
            app->generation = 0;
            app_recompute_world_stats(app);
            app->needs_redraw = 1;
            break;
        case 'c':
            world_clear(&app->world);
            app->generation = 0;
            app_recompute_world_stats(app);
            app->needs_redraw = 1;
            break;
        case '+':
            if (app->startup.delay_ms > 10) app->startup.delay_ms -= 10;
            app->needs_redraw = 1;
            break;
        case '-':
            app->startup.delay_ms = app->startup.delay_ms > INT_MAX - 10 ?
                                    INT_MAX : app->startup.delay_ms + 10;
            app->needs_redraw = 1;
            break;
        case ':':
            cmd_enter(&app->cmd);
            app->needs_redraw = 1;
            break;
        case 's':
            app->show_stats = !app->show_stats;
            app->needs_redraw = 1;
            break;
        default:
            app->needs_redraw = 1;
            break;
    }
}

void app_poll_input(App *app) {
    if (!app) return;

    for (;;) {
        int ch = term_read_key_nonblocking();
        if (ch < 0) break;
        app_handle_key(app, ch);
    }
}

void app_update(App *app) {
    if (!app) return;
    if (app->terminal_too_small) return;
    if (app->world_resize_pending) return;
    if (term_resize_pending) return;

    if (!app->paused) {
        world_step(&app->world, &app->last_step);
        app->generation++;
        app->needs_redraw = 1;
    }
}

void app_sleep(const App *app) {
    if (!app) return;
    sleep_ms(app->startup.delay_ms);
}
