#ifndef COMMAND_H
#define COMMAND_H

#define CMD_BUF_CAP 256

struct App;

typedef struct {
    int active;
    char buf[CMD_BUF_CAP];
    int len;
    int cursor;
    char message[CMD_BUF_CAP];
} CommandLine;

void cmd_init(CommandLine *cmd);
void cmd_clear(CommandLine *cmd);
void cmd_set_message(CommandLine *cmd, const char *msg);

void cmd_enter(CommandLine *cmd);
void cmd_cancel(CommandLine *cmd);

void cmd_insert_char(CommandLine *cmd, int ch);
void cmd_backspace(CommandLine *cmd);

void cmd_execute(struct App *app);

#endif
