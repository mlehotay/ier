#include "command.h"

#include "app.h"

#include <stdio.h>
#include <string.h>

void cmd_init(CommandLine *cmd) {
    if (!cmd) return;
    memset(cmd, 0, sizeof(*cmd));
}

void cmd_clear(CommandLine *cmd) {
    if (!cmd) return;
    cmd->buf[0] = '\0';
    cmd->len = 0;
    cmd->cursor = 0;
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
    if (cmd->len >= CMD_BUF_CAP - 1) return;

    cmd->buf[cmd->len++] = (char)ch;
    cmd->buf[cmd->len] = '\0';
    cmd->cursor = cmd->len;
}

void cmd_backspace(CommandLine *cmd) {
    if (!cmd || !cmd->active || cmd->len <= 0) return;
    cmd->buf[--cmd->len] = '\0';
    cmd->cursor = cmd->len;
}

void cmd_execute(struct App *app) {
    if (!app) return;

    const char *s = app->cmd.buf;

    if (strcmp(s, "help") == 0 || strcmp(s, "h") == 0) {
        cmd_set_message(&app->cmd,
                        "Commands: h/help, stats on, stats off, torus on, torus off");
    } else if (strcmp(s, "stats on") == 0) {
        app->show_stats = 1;
        cmd_set_message(&app->cmd, "Stats pane enabled.");
    } else if (strcmp(s, "stats off") == 0) {
        app->show_stats = 0;
        cmd_set_message(&app->cmd, "Stats pane disabled.");
    } else if (strcmp(s, "torus on") == 0) {
        app->world.toroidal = 1;
        world_recompute_stats(&app->world, &app->last_step);
        cmd_set_message(&app->cmd, "Toroidal mode enabled.");
    } else if (strcmp(s, "torus off") == 0) {
        app->world.toroidal = 0;
        world_recompute_stats(&app->world, &app->last_step);
        cmd_set_message(&app->cmd, "Toroidal mode disabled.");
    } else {
        cmd_set_message(&app->cmd, "Unknown command. Type 'h' for help.");
    }

    cmd_cancel(&app->cmd);
    app->needs_redraw = 1;
}
