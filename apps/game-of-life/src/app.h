#ifndef APP_H
#define APP_H

#include "command.h"
#include "ui.h"
#include "world.h"
#include "pattern.h"

typedef enum {
    STARTUP_RANDOM = 0,
    STARTUP_PATTERN,
    STARTUP_LOAD
} StartupMode;

typedef struct {
    int width;               /* 0 = derive from current grid pane */
    int height;              /* 0 = derive from current grid pane */
    unsigned int seed;
    int delay_ms;
    int toroidal;
    StartupMode mode;
    char pattern_name[64];
    char load_path[256];
    PlacementConfig placement;
} StartupConfig;

typedef struct App {
    World world;
    Layout layout;

    StartupConfig startup;
    PlacementConfig placement;

    unsigned long generation;
    int paused;
    int running;

    int needs_redraw;
    int resized;
    int terminal_too_small;
    int world_resize_pending; /* retry a failed allocation before stepping */

    int show_stats;

    StepStats last_step;
    CommandLine cmd;
} App;

int  app_init(App *app, const StartupConfig *startup);
void app_destroy(App *app);

void app_handle_resize(App *app);
void app_poll_input(App *app);
void app_handle_key(App *app, int ch);
void app_update(App *app);
void app_sleep(const App *app);

#endif
