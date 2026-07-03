#include "world.h"

#include <stdlib.h>
#include <string.h>

static size_t world_bytes(int w, int h) {
    return (size_t)w * (size_t)h * sizeof(uint8_t);
}

int world_init(World *w, int width, int height, int toroidal) {
    if (!w || width <= 0 || height <= 0) return -1;

    w->w = width;
    w->h = height;
    w->toroidal = toroidal;
    w->cells = calloc((size_t)width * (size_t)height, sizeof(uint8_t));
    w->next  = calloc((size_t)width * (size_t)height, sizeof(uint8_t));

    if (!w->cells || !w->next) {
        free(w->cells);
        free(w->next);
        memset(w, 0, sizeof(*w));
        return -1;
    }

    return 0;
}

void world_destroy(World *w) {
    if (!w) return;
    free(w->cells);
    free(w->next);
    memset(w, 0, sizeof(*w));
}

int world_resize(World *w, int new_w, int new_h) {
    if (!w || new_w <= 0 || new_h <= 0) return -1;
    if (new_w == w->w && new_h == w->h) return 0;

    uint8_t *new_cells = calloc((size_t)new_w * (size_t)new_h, sizeof(uint8_t));
    uint8_t *new_next  = calloc((size_t)new_w * (size_t)new_h, sizeof(uint8_t));
    if (!new_cells || !new_next) {
        free(new_cells);
        free(new_next);
        return -1;
    }

    const int copy_w = (new_w < w->w) ? new_w : w->w;
    const int copy_h = (new_h < w->h) ? new_h : w->h;

    for (int y = 0; y < copy_h; ++y) {
        memcpy(&new_cells[y * new_w],
               &w->cells[y * w->w],
               (size_t)copy_w * sizeof(uint8_t));
    }

    free(w->cells);
    free(w->next);

    w->cells = new_cells;
    w->next = new_next;
    w->w = new_w;
    w->h = new_h;

    return 0;
}

void world_clear(World *w) {
    if (!w || !w->cells || !w->next) return;
    memset(w->cells, 0, world_bytes(w->w, w->h));
    memset(w->next,  0, world_bytes(w->w, w->h));
}

void world_randomize(World *w, unsigned int seed) {
    if (!w || !w->cells || !w->next) return;
    srand(seed);
    for (int y = 0; y < w->h; ++y) {
        for (int x = 0; x < w->w; ++x) {
            w->cells[y * w->w + x] = (uint8_t)(rand() & 1u);
            w->next[y * w->w + x] = 0u;
        }
    }
}

void world_recompute_stats(const World *w, StepStats *out) {
    if (!out) return;

    memset(out, 0, sizeof(*out));
    if (!w || !w->cells) return;

    for (int y = 0; y < w->h; ++y) {
        for (int x = 0; x < w->w; ++x) {
            if (w->cells[y * w->w + x] != 0) {
                out->live_cells++;
            }
        }
    }
}

int world_get(const World *w, int x, int y) {
    if (!w || !w->cells) return 0;

    if (w->toroidal) {
        if (w->w <= 0 || w->h <= 0) return 0;
        x = (x % w->w + w->w) % w->w;
        y = (y % w->h + w->h) % w->h;
        return w->cells[y * w->w + x] != 0;
    }

    if (x < 0 || y < 0 || x >= w->w || y >= w->h) return 0;
    return w->cells[y * w->w + x] != 0;
}

void world_set(World *w, int x, int y, int alive) {
    if (!w || !w->cells) return;
    if (x < 0 || y < 0 || x >= w->w || y >= w->h) return;
    w->cells[y * w->w + x] = alive ? 1u : 0u;
}

int world_count_neighbors(const World *w, int x, int y) {
    int n = 0;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            n += world_get(w, x + dx, y + dy);
        }
    }
    return n;
}

void world_step(World *w, StepStats *out) {
    if (!w || !w->cells || !w->next) return;

    StepStats stats = {0};

    for (int y = 0; y < w->h; ++y) {
        for (int x = 0; x < w->w; ++x) {
            const int idx = y * w->w + x;
            const int alive = w->cells[idx] != 0;
            const int n = world_count_neighbors(w, x, y);

            int next_alive = 0;
            if (alive) {
                next_alive = (n == 2 || n == 3);
            } else {
                next_alive = (n == 3);
            }

            w->next[idx] = (uint8_t)next_alive;

            if (next_alive) stats.live_cells++;
            if (!alive && next_alive) stats.births++;
            if (alive && !next_alive) stats.deaths++;
            if (alive && next_alive)  stats.survivors++;
            if (alive != next_alive)  stats.changed++;
        }
    }

    uint8_t *tmp = w->cells;
    w->cells = w->next;
    w->next = tmp;

    if (out) *out = stats;
}
