#ifndef PATTERN_H
#define PATTERN_H

#include <stddef.h>
#include "world.h"

typedef struct {
    int centered;            /* default when no explicit coordinates */
    int has_xy;
    int x, y;
} PlacementConfig;

typedef struct {
    int dx, dy;
} PatternCell;

/* A borrowed, read-only view. Built-ins and their cells have static lifetime. */
typedef struct {
    const char *name;
    const PatternCell *cells;
    size_t count;
    int width, height;
} Pattern;

const Pattern *pattern_lookup(const char *name); /* exact lowercase name, or NULL */
size_t pattern_count(void);
const Pattern *pattern_at(size_t index); /* stable order, NULL for invalid index */
/* Optional outputs; invalid pattern returns -1 without changing them. */
int pattern_dimensions(const Pattern *pattern, int *width, int *height);

typedef enum {
    PATTERN_PLACE_OK,
    PATTERN_PLACE_INVALID_ARGUMENT,
    PATTERN_PLACE_INVALID_PATTERN,
    PATTERN_PLACE_OUT_OF_BOUNDS
} PatternPlaceResult;

/* Add live cells only; validate the whole placement before mutation. Does not
   clear the world, wrap coordinates, change topology or recompute app stats. */
PatternPlaceResult pattern_place(World *world, const Pattern *pattern, int x, int y);
PatternPlaceResult pattern_place_centered(World *world, const Pattern *pattern);
/* NULL/no coordinates means centered. Explicit coordinates override centered. */
PatternPlaceResult pattern_apply(World *world, const Pattern *pattern, const PlacementConfig *placement);
const char *pattern_place_message(PatternPlaceResult result);

#endif
