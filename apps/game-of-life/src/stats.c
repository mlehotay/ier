#include "stats.h"

#include "app.h"

#include <string.h>

void stats_collect(const struct App *app, StatsSnapshot *out) {
    if (!app || !out) return;

    memset(out, 0, sizeof(*out));

    out->generation = app->generation;
    out->live_cells = app->last_step.live_cells;
    out->births = app->last_step.births;
    out->deaths = app->last_step.deaths;
    out->changed = app->last_step.changed;

    out->world_w = app->world.w;
    out->world_h = app->world.h;

    const double area = (double)app->world.w * (double)app->world.h;
    out->density = (area > 0.0) ? ((double)out->live_cells / area) : 0.0;

    out->paused = app->paused;
    out->delay_ms = app->startup.delay_ms;
    out->toroidal = app->world.toroidal;
}
