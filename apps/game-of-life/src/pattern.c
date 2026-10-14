#include "pattern.h"

#include <string.h>
#include <limits.h>

/* Fixed normalized phases; provenance and orientations: docs/patterns.md. */

static const PatternCell block_cells[] = {
    {0, 0}, {1, 0},
    {0, 1}, {1, 1},
};

static const PatternCell blinker_cells[] = {
    {0, 0}, {1, 0}, {2, 0},
};

static const PatternCell toad_cells[] = {
    {1, 0}, {2, 0}, {3, 0},
    {0, 1}, {1, 1}, {2, 1},
};

static const PatternCell beacon_cells[] = {
    {0, 0}, {1, 0},
    {0, 1}, {1, 1},
    {2, 2}, {3, 2},
    {2, 3}, {3, 3},
};

static const PatternCell glider_cells[] = {
    {1, 0},
    {2, 1},
    {0, 2}, {1, 2}, {2, 2},
};

static const PatternCell lwss_cells[] = {
    {1, 0}, {4, 0},
    {0, 1},
    {0, 2}, {4, 2},
    {0, 3}, {1, 3}, {2, 3}, {3, 3},
};

static const PatternCell gosper_cells[] = {
    {24, 0},
    {22, 1}, {24, 1},
    {12, 2}, {13, 2}, {20, 2}, {21, 2}, {34, 2}, {35, 2},
    {11, 3}, {15, 3}, {20, 3}, {21, 3}, {34, 3}, {35, 3},
    {0, 4}, {1, 4}, {10, 4}, {16, 4}, {20, 4}, {21, 4},
    {0, 5}, {1, 5}, {10, 5}, {14, 5}, {16, 5}, {17, 5}, {22, 5}, {24, 5},
    {10, 6}, {16, 6}, {24, 6},
    {11, 7}, {15, 7},
    {12, 8}, {13, 8},
};

static const Pattern patterns[] = {
    {"block", block_cells, sizeof(block_cells) / sizeof(block_cells[0]), 2, 2},
    {"blinker", blinker_cells, sizeof(blinker_cells) / sizeof(blinker_cells[0]), 3, 1},
    {"toad", toad_cells, sizeof(toad_cells) / sizeof(toad_cells[0]), 4, 2},
    {"beacon", beacon_cells, sizeof(beacon_cells) / sizeof(beacon_cells[0]), 4, 4},
    {"glider", glider_cells, sizeof(glider_cells) / sizeof(glider_cells[0]), 3, 3},
    {"lwss", lwss_cells, sizeof(lwss_cells) / sizeof(lwss_cells[0]), 5, 4},
    {"gosper", gosper_cells, sizeof(gosper_cells) / sizeof(gosper_cells[0]), 36, 9},
};

size_t pattern_count(void) {
    return sizeof(patterns) / sizeof(patterns[0]);
}

const Pattern *pattern_at(size_t index) {
    return index < pattern_count() ? &patterns[index] : NULL;
}

const Pattern *pattern_lookup(const char *name) {
    if (!name) return NULL;
    for (size_t i = 0; i < pattern_count(); ++i) {
        if (strcmp(name, patterns[i].name) == 0) return &patterns[i];
    }
    return NULL;
}

int pattern_dimensions(const Pattern *pattern, int *width, int *height) {
    if (!pattern || !pattern->cells || !pattern->count ||
        pattern->width <= 0 || pattern->height <= 0) return -1;
    if (width) *width = pattern->width;
    if (height) *height = pattern->height;
    return 0;
}

static PatternPlaceResult place(World *world, const Pattern *pattern, int x, int y, int centered) {
    if (!world || !pattern || !world->cells || !world->next ||
        world->w <= 0 || world->h <= 0 || world->w > INT_MAX / world->h) {
        return PATTERN_PLACE_INVALID_ARGUMENT;
    }
    int width, height;
    if (pattern_dimensions(pattern, &width, &height) != 0) return PATTERN_PLACE_INVALID_PATTERN;
    if (width > world->w || height > world->h) return PATTERN_PLACE_OUT_OF_BOUNDS;
    if (centered) {
        x = (world->w - width) / 2;
        y = (world->h - height) / 2;
    }
    /* Subtraction avoids overflow in x+width or y+height. */
    if (x < 0 || y < 0 || x > world->w - width || y > world->h - height) {
        return PATTERN_PLACE_OUT_OF_BOUNDS;
    }
    for (size_t i = 0; i < pattern->count; ++i) {
        const PatternCell *cell = &pattern->cells[i];
        if (cell->dx < 0 || cell->dy < 0 || cell->dx >= width || cell->dy >= height) {
            return PATTERN_PLACE_INVALID_PATTERN;
        }
    }
    for (size_t i = 0; i < pattern->count; ++i) {
        world_set(world, x + pattern->cells[i].dx, y + pattern->cells[i].dy, 1);
    }
    return PATTERN_PLACE_OK;
}

PatternPlaceResult pattern_place(World *world, const Pattern *pattern, int x, int y) {
    return place(world, pattern, x, y, 0);
}

PatternPlaceResult pattern_place_centered(World *world, const Pattern *pattern) {
    return place(world, pattern, 0, 0, 1);
}

PatternPlaceResult pattern_apply(World *world, const Pattern *pattern, const PlacementConfig *placement) {
    if (placement && placement->has_xy) return pattern_place(world, pattern, placement->x, placement->y);
    return pattern_place_centered(world, pattern);
}

const char *pattern_place_message(PatternPlaceResult result) {
    switch (result) {
        case PATTERN_PLACE_OK: return "Pattern placed.";
        case PATTERN_PLACE_INVALID_ARGUMENT: return "Invalid world or pattern.";
        case PATTERN_PLACE_INVALID_PATTERN: return "Invalid pattern data.";
        case PATTERN_PLACE_OUT_OF_BOUNDS: return "Pattern placement is outside the world.";
    }
    return "Unknown placement error.";
}
