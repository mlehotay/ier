#define _POSIX_C_SOURCE 200809L

#include "ui.h"

#include "app.h"
#include "stats.h"
#include "term.h"

#include <stdio.h>
#include <string.h>

#define CMD_PROMPT "(h=help):"

/* Leave the bottom-right cell untouched: writing it can scroll a terminal
   with immediate wrap, or leave delayed wrap pending for the next write. */
static int ui_safe_width(const Layout *L, int x, int y, int width) {
    if (x < 0 || y < 0 || x >= L->screen_w || y >= L->screen_h || width <= 0) return 0;
    int available = L->screen_w - x;
    if (y == L->screen_h - 1) --available;
    return width < available ? width : available;
}

static void ui_write_padded(const Layout *L, int x, int y, int width, const char *text) {
    width = ui_safe_width(L, x, y, width);
    if (width <= 0) return;
    char buf[4096];
    size_t length = text ? strlen(text) : 0;
    for (int offset = 0; offset < width;) {
        int n = width - offset;
        if (n > (int)sizeof(buf)) n = (int)sizeof(buf);
        memset(buf, ' ', (size_t)n);
        if ((size_t)offset < length) {
            size_t copy = length - (size_t)offset;
            if (copy > (size_t)n) copy = (size_t)n;
            memcpy(buf, text + offset, copy);
        }
        term_move_cursor(x + offset, y);
        term_write_n(buf, (size_t)n);
        offset += n;
    }
}

void ui_layout_compute(Layout *L, int term_w, int term_h) {
    if (!L) return;

    memset(L, 0, sizeof(*L));
    L->screen_w = term_w > 0 ? term_w : 0;
    L->screen_h = term_h > 0 ? term_h : 0;
    term_w = L->screen_w;
    term_h = L->screen_h;
    L->cmd_height = 2;
    L->stats_width = 30;
    L->stats_visible = 0;
    L->draw_borders = 0;

    if (term_w < 20 || term_h < 5) {
        L->cmd_height = term_h < 2 ? term_h : 2;
        L->cmd = (Rect){0, 0, term_w, L->cmd_height};
        return;
    }

    {
        const int sep_y = L->cmd_height;
        /* The complete stats pane needs eleven rows below the command area. */
        const int stats_w = (term_w >= 60 && term_h >= 14) ? L->stats_width : 0;
        L->draw_borders = 1;
        const int sep_x = term_w - stats_w - (stats_w > 0 ? 1 : 0);

        L->cmd = (Rect){0, 0, term_w, L->cmd_height};

        if (stats_w > 0) {
            L->grid = (Rect){0, sep_y + 1, sep_x, term_h - (sep_y + 1)};
            L->stats = (Rect){sep_x + 1, sep_y + 1, stats_w, term_h - (sep_y + 1)};
            L->stats_visible = 1;
        } else {
            L->grid = (Rect){0, sep_y + 1, term_w, term_h - (sep_y + 1)};
            L->stats = (Rect){0, 0, 0, 0};
            L->stats_visible = 0;
        }
    }
}

void ui_draw_borders(const Layout *L) {
    if (!L || !L->draw_borders) return;
    const int sep_y = L->cmd.y + L->cmd.h;
    int width = ui_safe_width(L, 0, sep_y, L->screen_w);
    char rowbuf[4096];
    memset(rowbuf, '-', sizeof(rowbuf));
    for (int x = 0; x < width;) {
        int n = width - x;
        if (n > (int)sizeof(rowbuf)) n = (int)sizeof(rowbuf);
        term_move_cursor(x, sep_y);
        term_write_n(rowbuf, (size_t)n);
        x += n;
    }
    if (L->stats_visible) {
        const int sep_x = L->stats.x - 1;
        for (int y = L->grid.y; y < L->screen_h; ++y) {
            ui_write_padded(L, sep_x, y, 1, "|");
        }
    }
}

void ui_draw_command_pane(const struct App *app, const Rect *r) {
    if (!app || !r || r->w <= 0 || r->h <= 0) return;

    char line[1024];

    snprintf(line, sizeof(line),
             "[%s] gen=%lu delay=%dms seed=%u world=%dx%d %s",
             app->paused ? "PAUSED" : "RUNNING",
             app->generation,
             app->startup.delay_ms,
             app->startup.seed,
             app->world.w,
             app->world.h,
             app->world.toroidal ? "toroidal" : "bounded");
    if (app->terminal_too_small || app->layout.grid.w <= 0 || app->layout.grid.h <= 0) {
        snprintf(line, sizeof(line), "Terminal too small.");
    }
    if (r->h == 1 && app->cmd.active) {
        snprintf(line, sizeof(line), "%s%s", CMD_PROMPT, app->cmd.buf);
        if (app->cmd.overflow) snprintf(line, sizeof(line), "Error: command too long; Esc cancels.");
    }
    ui_write_padded(&app->layout, r->x, r->y, r->w, line);

    if (r->h >= 2) {
        if (app->cmd.active) {
            snprintf(line, sizeof(line), "%s%s", CMD_PROMPT, app->cmd.buf);
            if (app->cmd.overflow) snprintf(line, sizeof(line), "Error: command too long; Esc cancels.");
            ui_write_padded(&app->layout, r->x, r->y + 1, r->w, line);
        } else if (app->cmd.message[0]) {
            ui_write_padded(&app->layout, r->x, r->y + 1, r->w, app->cmd.message);
        } else {
            ui_write_padded(&app->layout, r->x, r->y + 1, r->w,
                            "q quit | space pause | n step | r randomize | c clear | : help");
        }
    }
}

void ui_draw_grid_pane(const struct App *app, const Rect *r) {
    if (!app || !r || r->w <= 0 || r->h <= 0) return;

    char rowbuf[4096];
    for (int gy = 0; gy < r->h && gy < app->layout.screen_h - r->y; ++gy) {
        const int width = ui_safe_width(&app->layout, r->x, r->y + gy, r->w);
        for (int gx = 0; gx < width;) {
            int n = width - gx;
            if (n > (int)sizeof(rowbuf)) n = (int)sizeof(rowbuf);
            for (int i = 0; i < n; ++i) {
                /* Toroidal lookup belongs to simulation, not pane padding. */
                const int wx = gx + i;
                rowbuf[i] = wx < app->world.w && gy < app->world.h &&
                            world_get(&app->world, wx, gy) ? 'O' : ' ';
            }
            term_move_cursor(r->x + gx, r->y + gy);
            term_write_n(rowbuf, (size_t)n);
            gx += n;
        }
    }
}

void ui_draw_stats_pane(const struct App *app, const Rect *r) {
    if (!app || !r || r->w <= 0 || r->h <= 0) return;

    StatsSnapshot s;
    stats_collect(app, &s);

    char buf[256];
    int row = 0;

#define PUTLN(...)                         \
    do {                                   \
        if (row < r->h) {                  \
            snprintf(buf, sizeof(buf), __VA_ARGS__); \
            ui_write_padded(&app->layout, r->x, r->y + row, r->w, buf); \
            row++;                         \
        }                                  \
    } while (0)

    PUTLN("%s", "Stats");
    PUTLN("gen: %lu", s.generation);
    PUTLN("live: %lu", s.live_cells);
    PUTLN("density: %.4f", s.density);
    PUTLN("births: %lu", s.births);
    PUTLN("deaths: %lu", s.deaths);
    PUTLN("changed: %lu", s.changed);
    PUTLN("mode: %s", s.paused ? "paused" : "running");
    PUTLN("delay: %dms", s.delay_ms);
    PUTLN("world: %dx%d", s.world_w, s.world_h);
    PUTLN("topology: %s", s.toroidal ? "toroidal" : "bounded");

    while (row < r->h) {
        ui_write_padded(&app->layout, r->x, r->y + row, r->w, "");
        row++;
    }

#undef PUTLN
}

void ui_draw_frame(const struct App *app) {
    if (!app) return;

    if (app->layout.screen_w <= 0 || app->layout.screen_h <= 0) return;
    if (app->terminal_too_small || app->layout.grid.w <= 0 || app->layout.grid.h <= 0) {
        for (int y = 0; y < app->layout.screen_h; ++y) {
            ui_write_padded(&app->layout, 0, y, app->layout.screen_w, "");
        }
    }
    ui_draw_command_pane(app, &app->layout.cmd);
    ui_draw_borders(&app->layout);
    ui_draw_grid_pane(app, &app->layout.grid);

    if (app->layout.stats_visible && app->show_stats) {
        ui_draw_stats_pane(app, &app->layout.stats);
    } else if (app->layout.stats_visible) {
        for (int row = 0; row < app->layout.stats.h; ++row) {
            ui_write_padded(&app->layout, app->layout.stats.x, app->layout.stats.y + row,
                            app->layout.stats.w, "");
        }
    }

    if (app->cmd.active) {
        int cursor_x = app->layout.cmd.x + (int)strlen(CMD_PROMPT) + app->cmd.cursor;
        int cursor_y = app->layout.cmd.y + (app->layout.cmd.h >= 2 ? 1 : 0);
        int width = ui_safe_width(&app->layout, app->layout.cmd.x, cursor_y,
                                  app->layout.cmd.w);
        int right_edge = app->layout.cmd.x + (width > 0 ? width - 1 : 0);

        if (cursor_x > right_edge) cursor_x = right_edge;
        if (cursor_x < app->layout.cmd.x) cursor_x = app->layout.cmd.x;

        term_show_cursor();
        term_move_cursor(cursor_x, cursor_y);
    } else {
        term_hide_cursor();
        term_move_cursor(0, 0);
    }
}
