#ifndef UI_H
#define UI_H

struct App;
struct StatsSnapshot;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} Rect;

typedef struct {
    int screen_w;
    int screen_h;

    Rect cmd;
    Rect grid;
    Rect stats;

    int stats_visible;
    int stats_width;
    int cmd_height;
    int draw_borders;
} Layout;

void ui_layout_compute(Layout *L, int term_w, int term_h);

void ui_draw_frame(const struct App *app);
void ui_draw_borders(const Layout *L);
void ui_draw_command_pane(const struct App *app, const Rect *r);
void ui_draw_grid_pane(const struct App *app, const Rect *r);
void ui_draw_stats_pane(const struct App *app, const Rect *r);

#endif
