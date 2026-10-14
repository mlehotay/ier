#include "command.h"

#include "app.h"
#include "cells.h"
#include "number.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>

static void reply(CommandLine *cmd, int success, const char *format, ...) {
    char detail[CMD_BUF_CAP];
    va_list args;
    va_start(args, format);
    vsnprintf(detail, sizeof(detail), format, args);
    va_end(args);
    /* Terminal-bound messages must never contain control bytes from paths. */
    for (char *p = detail; *p; ++p) {
        if ((unsigned char)*p < 32 || (unsigned char)*p >= 127) *p = '?';
    }
    snprintf(cmd->message, sizeof(cmd->message), "%s%.248s", success ? "OK: " : "Error: ", detail);
}

static const char *usage(const char *name) {
    if (!strcmp(name, "help") || !strcmp(name, "h")) return "help";
    if (!strcmp(name, "stats")) return "stats on|off";
    if (!strcmp(name, "torus")) return "torus on|off";
    if (!strcmp(name, "bounded")) return "bounded";
    if (!strcmp(name, "seed")) return "seed N";
    if (!strcmp(name, "delay")) return "delay MS";
    if (!strcmp(name, "clear")) return "clear";
    if (!strcmp(name, "randomize")) return "randomize";
    return NULL;
}

void cmd_init(CommandLine *cmd) {
    if (!cmd) return;
    memset(cmd, 0, sizeof(*cmd));
}

void cmd_clear(CommandLine *cmd) {
    if (!cmd) return;
    cmd->buf[0] = '\0';
    cmd->len = 0;
    cmd->cursor = 0;
    cmd->overflow = 0;
}

void cmd_set_message(CommandLine *cmd, const char *msg) {
    if (!cmd) return;
    snprintf(cmd->message, sizeof(cmd->message), "%s", msg ? msg : "");
}

void cmd_enter(CommandLine *cmd) {
    if (!cmd) return;
    cmd->active = 1;
    cmd_clear(cmd);
    cmd->message[0] = '\0';
}

void cmd_cancel(CommandLine *cmd) {
    if (!cmd) return;
    cmd->active = 0;
    cmd_clear(cmd);
}

void cmd_insert_char(CommandLine *cmd, int ch) {
    if (!cmd || !cmd->active) return;
    if (cmd->len >= CMD_BUF_CAP - 1) {
        cmd->overflow = 1;
        return;
    }

    cmd->buf[cmd->len++] = (char)ch;
    cmd->buf[cmd->len] = '\0';
    cmd->cursor = cmd->len;
}

void cmd_backspace(CommandLine *cmd) {
    if (!cmd || !cmd->active || cmd->len <= 0) return;
    cmd->buf[--cmd->len] = '\0';
    cmd->cursor = cmd->len;
}

/* Tokens may be wholly single/double quoted; no escaping or shell expansion. */
static int tokenize(char *text, char **args, int capacity) {
    int count = 0;
    char *p = text;
    while (*p) {
        while (isspace((unsigned char)*p)) ++p;
        if (!*p) break;
        if (count == capacity) return -1;
        if (*p == '\'' || *p == '"') {
            char quote = *p++;
            args[count++] = p;
            while (*p && *p != quote) ++p;
            if (!*p) return -1;
            *p++ = '\0';
            if (*p && !isspace((unsigned char)*p)) return -1;
        } else {
            args[count++] = p;
            while (*p && !isspace((unsigned char)*p)) {
                if (*p == '\'' || *p == '"') return -1;
                ++p;
            }
            if (*p) *p++ = '\0';
        }
    }
    return count;
}

static void apply_pattern(App *app, int count, char **args, int load) {
    PlacementConfig placement = {.centered = 1};
    if (count < 2 || !args[1][0] ||
        (count != 2 && count != 4 && !(count == 3 && strcmp(args[2], "center") == 0))) {
        reply(&app->cmd, 0, "Usage: %s", load ? "load FILE [center|X Y]." : "pattern NAME [center|X Y].");
        return;
    }
    if (count == 4) {
        unsigned long x, y;
        if (number_parse(args[2], 0, INT_MAX, &x) || number_parse(args[3], 0, INT_MAX, &y)) {
            reply(&app->cmd, 0, "X and Y must be decimal integers from 0 to %d.", INT_MAX);
            return;
        }
        placement.has_xy = 1;
        placement.centered = 0;
        placement.x = (int)x;
        placement.y = (int)y;
    }
    CellsPattern owned = {0};
    Pattern view;
    const Pattern *pattern;
    if (load) {
        CellsError error;
        if (cells_load(args[1], &owned, &error) != CELLS_OK) {
            char detail[640];
            cells_format_error(args[1], &error, detail, sizeof(detail));
            reply(&app->cmd, 0, "Load failed: %s", detail);
            cells_pattern_destroy(&owned);
            return;
        }
        view = (Pattern){args[1], owned.cells, owned.count, owned.width, owned.height};
        pattern = &view;
    } else {
        pattern = pattern_lookup(args[1]);
        if (!pattern) {
            reply(&app->cmd, 0, "Unknown pattern '%.64s'; use block, blinker, toad, beacon, glider, lwss or gosper.", args[1]);
            return;
        }
    }
    PatternPlaceResult result = pattern_apply(&app->world, pattern, &placement);
    if (result != PATTERN_PLACE_OK) {
        reply(&app->cmd, 0, "%s World is %dx%d; pattern is %dx%d. Source '%.64s'.",
              pattern_place_message(result), app->world.w, app->world.h, pattern->width, pattern->height, args[1]);
    } else {
        world_recompute_stats(&app->world, &app->last_step);
        int x = placement.has_xy ? placement.x : (app->world.w - pattern->width) / 2;
        int y = placement.has_xy ? placement.y : (app->world.h - pattern->height) / 2;
        reply(&app->cmd, 1, "%s at %d,%d; %lu live cells.",
              load ? "File pattern placed" : "Pattern placed", x, y, app->last_step.live_cells);
    }
    cells_pattern_destroy(&owned);
}

void cmd_execute(struct App *app) {
    if (!app) return;
    if (app->cmd.overflow) {
        reply(&app->cmd, 0, "Command exceeds %d characters; re-enter it.", CMD_BUF_CAP - 1);
        cmd_cancel(&app->cmd);
        app->needs_redraw = 1;
        return;
    }
    char text[CMD_BUF_CAP], *args[5];
    memcpy(text, app->cmd.buf, sizeof(text));
    text[sizeof(text) - 1] = '\0';
    int count = tokenize(text, args, 5);
    const char *s = count > 0 ? args[0] : "";
    if (count < 0) {
        reply(&app->cmd, 0, "Malformed quoting or too many arguments. Type 'help'.");
    } else if (count == 0) {
        reply(&app->cmd, 0, "Empty command. Type 'help'.");
    } else if (!strcmp(s, "pattern") || !strcmp(s, "load")) {
        apply_pattern(app, count, args, !strcmp(s, "load"));
    } else if ((!strcmp(s, "help") || !strcmp(s, "h")) && count == 1) {
        reply(&app->cmd, 1, "Commands: help/h, stats on/off, torus on/off, bounded, seed N, delay MS, pattern NAME [center|X Y], load FILE [center|X Y], clear, randomize.");
    } else if ((!strcmp(s, "stats") || !strcmp(s, "torus")) && count == 2 &&
               (!strcmp(args[1], "on") || !strcmp(args[1], "off"))) {
        int enabled = !strcmp(args[1], "on");
        if (!strcmp(s, "stats")) {
            app->show_stats = enabled;
            reply(&app->cmd, 1, "Stats pane %s.", enabled ? "enabled" : "disabled");
        } else {
            app->world.toroidal = enabled;
            world_recompute_stats(&app->world, &app->last_step);
            reply(&app->cmd, 1, "Topology set to %s.", enabled ? "toroidal" : "bounded");
        }
    } else if (!strcmp(s, "bounded") && count == 1) {
        app->world.toroidal = 0;
        world_recompute_stats(&app->world, &app->last_step);
        reply(&app->cmd, 1, "Topology set to bounded.");
    } else if ((!strcmp(s, "seed") || !strcmp(s, "delay")) && count == 2) {
        int seed = !strcmp(s, "seed");
        unsigned long value;
        if (number_parse(args[1], seed ? 0 : 1, seed ? UINT_MAX : INT_MAX, &value)) {
            reply(&app->cmd, 0, "%s must be a decimal integer from %d to %lu.",
                     s, seed ? 0 : 1, seed ? (unsigned long)UINT_MAX : (unsigned long)INT_MAX);
        } else if (seed) {
            app->startup.seed = (unsigned int)value;
            reply(&app->cmd, 1, "Seed set to %u; use randomize to apply.", app->startup.seed);
        } else {
            app->startup.delay_ms = (int)value;
            reply(&app->cmd, 1, "Delay set to %dms.", app->startup.delay_ms);
        }
    } else if ((!strcmp(s, "clear") || !strcmp(s, "randomize")) && count == 1) {
        if (!strcmp(s, "clear")) world_clear(&app->world);
        else world_randomize(&app->world, app->startup.seed);
        app->generation = 0;
        world_recompute_stats(&app->world, &app->last_step);
        reply(&app->cmd, 1, "%s %lu live cells; generation reset.",
              !strcmp(s, "clear") ? "World cleared." : "World randomized.", app->last_step.live_cells);
    } else {
        const char *syntax = usage(s);
        if (syntax) reply(&app->cmd, 0, "Usage: %s.", syntax);
        else reply(&app->cmd, 0, "Unknown command '%.64s'. Type 'help'.", s);
    }
    cmd_cancel(&app->cmd);
    app->needs_redraw = 1;
}
