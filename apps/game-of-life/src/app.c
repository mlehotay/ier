#define _POSIX_C_SOURCE 200809L

#include "app.h"

#include "term.h"

#include <stdio.h>
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

    app->startup.width = 0;
    app->startup.height = 0;
    app->startup.seed = 42u;
    app->startup.delay_ms = 100;
    app->startup.toroidal = 1;
    app->startup.mode = STARTUP_RANDOM;
    app->startup.pattern_name[0] = '\0';
    app->startup.load_path[0] = '\0';

    app->placement.centered = 1;
    app->placement.has_xy = 0;
    app->placement.x = 0;
    app->placement.y = 0;

    app->paused = 0;
    app->running = 1;
    app->show_stats = 1;
    app->show_help = 0;
    app->needs_redraw = 1;
    app->resized = 0;
    app->terminal_too_small = 0;
    app->generation = 0;

    memset(&app->last_step, 0, sizeof(app->last_step));
}

static int app_apply_startup_initializer(App *app) {
    if (!app) return -1;

    /* gol-01/gol-02 policy: random startup remains the only active
       initializer. Pattern/load modes are represented in state now and wired
       later. Recompute stats after any non-step mutation of world state. */
    world_clear(&app->world);

    if (app->startup.mode == STARTUP_RANDOM) {
        world_randomize(&app->world, app->startup.seed);
    }

    app->generation = 0;
    app_recompute_world_stats(app);
    return 0;
}

int app_init(App *app, const StartupConfig *startup) {
    if (!app) return -1;
    memset(app, 0, sizeof(*app));

    app_init_defaults(app);

    if (startup) {
        app->startup = *startup;
    }

    cmd_init(&app->cmd);

    int term_w = 80;
    int term_h = 24;
    term_get_size(&term_w, &term_h);
    ui_layout_compute(&app->layout, term_w, term_h);
    app_update_terminal_small_flag(app);

    /* gol-01 policy: the world is the visible board. There is no off-screen
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

    if (world_init(&app->world, world_w, world_h, app->startup.toroidal) != 0) {
        return -1;
    }

    if (app_apply_startup_initializer(app) != 0) {
        world_destroy(&app->world);
        return -1;
    }

    return 0;
}

void app_destroy(App *app) {
    if (!app) return;
    world_destroy(&app->world);
}

void app_handle_resize(App *app) {
    if (!app) return;
    if (!term_resize_pending) return;

    term_resize_pending = 0;

    int term_w = 0;
    int term_h = 0;
    if (term_get_size(&term_w, &term_h) != 0) return;

    ui_layout_compute(&app->layout, term_w, term_h);
    app_update_terminal_small_flag(app);
    app->resized = 1;

    const int new_w = app_grid_world_width(app);
    const int new_h = app_grid_world_height(app);

    if (new_w != app->world.w || new_h != app->world.h) {
        if (world_resize(&app->world, new_w, new_h) != 0) {
            cmd_set_message(&app->cmd, "Resize failed.");
        } else {
            app_recompute_world_stats(app);
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
            if (app->paused) {
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
            app->startup.delay_ms += 10;
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
