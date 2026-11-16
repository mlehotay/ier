#define _POSIX_C_SOURCE 200809L

#include "cli.h"
#include "app.h"
#include "term.h"

#include <errno.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv) {
    StartupConfig startup;
    App app;

    cli_startup_defaults(&startup);

    int rc = cli_parse_args(argc, argv, &startup, stderr);
    if (rc > 0) return 0;
    if (rc < 0) return 1;

    if (app_init(&app, &startup) != 0) {
        return 1;
    }

    if (term_enable_raw_mode() != 0) {
        app_destroy(&app);
        return 1;
    }

    term_enter_alt_screen();
    term_hide_cursor();
    term_install_signal_handlers();

    uint64_t next_step_ms = 0;
    int previous_delay = app.startup.delay_ms;
    int previous_pause = app.paused;
    int result = 0;
    while (app.running) {
        app_handle_resize(&app);
        app_poll_input(&app);
        if (!app.running) break;

        struct timespec now;
        if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
            result = 1;
            break;
        }
        uint64_t now_ms = (uint64_t)now.tv_sec * 1000 + (uint64_t)now.tv_nsec / 1000000;
        if (app.paused != previous_pause || app.startup.delay_ms != previous_delay) {
            next_step_ms = now_ms + (uint64_t)app.startup.delay_ms;
        }
        previous_delay = app.startup.delay_ms;
        previous_pause = app.paused;
        if (!app.paused && now_ms >= next_step_ms) {
            app_update(&app);
            next_step_ms = now_ms + (uint64_t)app.startup.delay_ms;
        }

        /* A failed size query leaves the old layout unverified. Retry before
           drawing so an already-shrunken terminal is not given stale bounds. */
        if (!term_resize_pending && (app.needs_redraw || app.resized)) {
            ui_draw_frame(&app);
            app.needs_redraw = 0;
            app.resized = 0;
        }

        /* Input and resize processing must not wait for a potentially huge
           simulation delay. Input wakeups do not cause extra clock ticks. */
        int wait_ms = 20;
        if (!app.paused && next_step_ms > now_ms && next_step_ms - now_ms < (uint64_t)wait_ms) {
            wait_ms = (int)(next_step_ms - now_ms);
        }
        struct pollfd input = {.fd = 0, .events = POLLIN};
        int ready = poll(&input, 1, wait_ms);
        if ((ready < 0 && errno != EINTR) || (ready > 0 && (input.revents & (POLLERR | POLLNVAL)))) {
            result = 1;
            break;
        }
        if (ready > 0 && (input.revents & POLLHUP)) break;
    }

    term_restore();
    app_destroy(&app);
    if (result) fprintf(stderr, "error: terminal input or monotonic clock failed\n");
    return result;
}
