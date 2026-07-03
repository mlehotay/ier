#define _POSIX_C_SOURCE 200809L

#include "ui.h"

#include "app.h"
#include "stats.h"
#include "term.h"

#include <stdio.h>
#include <string.h>

#define CMD_PROMPT "(h=help):"

static void ui_write_padded(int x, int y, int width, const char *text) {
    if (width <= 0) return;

    char buf[4096];
    if (width > (int)sizeof(buf)) width = (int)sizeof(buf);

    int i = 0;
    if (text) {
        while (text[i] && i < width) {
            buf[i] = text[i];
            i++;
        }
    }
    while (i < width) {
        buf[i++] = ' ';
    }

    term_move_cursor(x, y);
    term_write_n(buf, (size_t)width);
}

void ui_layout_compute(Layout *L, int term_w, int term_h) {
    if (!L) return;

    memset(L, 0, sizeof(*L));
    L->screen_w = term_w;
    L->screen_h = term_h;
    L->cmd_height = 2;
    L->stats_width = 30;
    L->stats_visible = 1;
    L->draw_borders = 1;

    if (term_w < 20 || term_h < 5) {
        L->cmd = (Rect){0, 0, term_w, term_h};
        return;
    }

    {
        const int sep_y = L->cmd_height;
        const int stats_w = (term_w >= 60) ? L->stats_width : 0;
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

    char rowbuf[4096];

    {
        const int sep_y = L->cmd.y + L->cmd.h;
        if (sep_y < L->screen_h && L->screen_w > 0) {
            int n = L->screen_w;
            if (n > (int)sizeof(rowbuf)) n = (int)sizeof(rowbuf);
            memset(rowbuf, '-', (size_t)n);
            term_move_cursor(0, sep_y);
            term_write_n(rowbuf, (size_t)n);
        }
    }

    if (L->stats_visible) {
        const int sep_x = L->stats.x - 1;
        for (int y = L->grid.y; y < L->screen_h; ++y) {
            term_move_cursor(sep_x, y);
            term_write("|");
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
    ui_write_padded(r->x, r->y, r->w, line);

    if (r->h >= 2) {
        if (app->cmd.active) {
            snprintf(line, sizeof(line), "%s%s", CMD_PROMPT, app->cmd.buf);
            ui_write_padded(r->x, r->y + 1, r->w, line);
        } else if (app->cmd.message[0]) {
            ui_write_padded(r->x, r->y + 1, r->w, app->cmd.message);
        } else {
            ui_write_padded(r->x, r->y + 1, r->w,
                            "q quit | space pause | n step | r randomize | c clear | : command");
        }
    }
}

void ui_draw_grid_pane(const struct App *app, const Rect *r) {
    if (!app || !r || r->w <= 0 || r->h <= 0) return;

    char rowbuf[4096];
    if (r->w > (int)sizeof(rowbuf)) return;

    for (int gy = 0; gy < r->h; ++gy) {
        const int wy = gy;

        for (int gx = 0; gx < r->w; ++gx) {
            const int wx = gx;
            rowbuf[gx] = world_get(&app->world, wx, wy) ? 'O' : ' ';
        }

        term_move_cursor(r->x, r->y + gy);
        term_write_n(rowbuf, (size_t)r->w);
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
            ui_write_padded(r->x, r->y + row, r->w, buf); \
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
        ui_write_padded(r->x, r->y + row, r->w, "");
        row++;
    }

#undef PUTLN
}

void ui_draw_frame(const struct App *app) {
    if (!app) return;

    if (app->terminal_too_small || app->layout.screen_w < 20 || app->layout.screen_h < 5) {
        term_hide_cursor();
        ui_write_padded(0, 0, app->layout.screen_w > 0 ? app->layout.screen_w : 20,
                        "Terminal too small.");
        term_move_cursor(0, 0);
        return;
    }

    ui_draw_command_pane(app, &app->layout.cmd);
    ui_draw_borders(&app->layout);
    ui_draw_grid_pane(app, &app->layout.grid);

    if (app->layout.stats_visible && app->show_stats) {
        ui_draw_stats_pane(app, &app->layout.stats);
    } else if (app->layout.stats_visible) {
        for (int row = 0; row < app->layout.stats.h; ++row) {
            ui_write_padded(app->layout.stats.x, app->layout.stats.y + row,
                            app->layout.stats.w, "");
        }
    }

    if (app->cmd.active) {
        int cursor_x = app->layout.cmd.x + (int)strlen(CMD_PROMPT) + app->cmd.cursor;
        int cursor_y = app->layout.cmd.y + 1;
        int right_edge = app->layout.cmd.x + app->layout.cmd.w - 1;

        if (cursor_x > right_edge) cursor_x = right_edge;
        if (cursor_x < app->layout.cmd.x) cursor_x = app->layout.cmd.x;

        term_show_cursor();
        term_move_cursor(cursor_x, cursor_y);
    } else {
        term_hide_cursor();
        term_move_cursor(0, 0);
    }
}
