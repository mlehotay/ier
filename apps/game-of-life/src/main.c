#include "cli.h"
#include "app.h"
#include "term.h"

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

    while (app.running) {
        app_handle_resize(&app);
        app_poll_input(&app);
        app_update(&app);

        if (app.needs_redraw || app.resized) {
            ui_draw_frame(&app);
            app.needs_redraw = 0;
            app.resized = 0;
        }

        app_sleep(&app);
    }

    term_restore();
    app_destroy(&app);
    return 0;
}
