#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>

typedef struct {
    int w;
    int h;
    uint8_t *cells;
    uint8_t *next;
    int toroidal;
} World;

typedef struct {
    unsigned long live_cells;
    unsigned long births;
    unsigned long deaths;
    unsigned long survivors;
    unsigned long changed;
} StepStats;

int  world_init(World *w, int width, int height, int toroidal);
void world_destroy(World *w);

int  world_resize(World *w, int new_w, int new_h); /* overlap preserved; new area dead */
void world_clear(World *w);
void world_randomize(World *w, unsigned int seed);
void world_recompute_stats(const World *w, StepStats *out);

int  world_get(const World *w, int x, int y);
void world_set(World *w, int x, int y, int alive);
int  world_count_neighbors(const World *w, int x, int y);

void world_step(World *w, StepStats *out);

#endif
