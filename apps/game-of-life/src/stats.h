#ifndef STATS_H
#define STATS_H

struct App;

typedef struct StatsSnapshot {
    unsigned long generation;
    unsigned long live_cells;
    double density;
    unsigned long births;
    unsigned long deaths;
    unsigned long changed;

    int paused;
    int delay_ms;
    int toroidal;
    int world_w;
    int world_h;
} StatsSnapshot;

void stats_collect(const struct App *app, StatsSnapshot *out);

#endif
