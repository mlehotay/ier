#define _POSIX_C_SOURCE 200809L

#include "term.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

volatile sig_atomic_t term_resize_pending = 0;

static struct termios g_saved_termios;
static int g_raw_enabled = 0;

static void on_sigwinch(int signo) {
    (void)signo;
    term_resize_pending = 1;
}

void term_install_signal_handlers(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_sigwinch;
    sigaction(SIGWINCH, &sa, NULL);
}

int term_enable_raw_mode(void) {
    if (g_raw_enabled) return 0;

    struct termios raw;
    if (tcgetattr(STDIN_FILENO, &g_saved_termios) < 0) return -1;
    raw = g_saved_termios;

    raw.c_lflag &= (tcflag_t) ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_iflag &= (tcflag_t) ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= (tcflag_t) ~(OPOST);
    raw.c_cflag |= (tcflag_t) (CS8);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) return -1;

    g_raw_enabled = 1;
    return 0;
}

void term_restore(void) {
    if (g_raw_enabled) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_saved_termios);
        g_raw_enabled = 0;
    }
    term_show_cursor();
    term_leave_alt_screen();
}

int term_get_size(int *w, int *h) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) < 0) return -1;
    if (w) *w = ws.ws_col;
    if (h) *h = ws.ws_row;
    return 0;
}

int term_read_key_nonblocking(void) {
    unsigned char c;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    return (n == 1) ? (int)c : -1;
}

void term_hide_cursor(void) { term_write("\033[?25l"); }
void term_show_cursor(void) { term_write("\033[?25h"); }

void term_clear_screen(void) { term_write("\033[2J"); }

void term_move_cursor(int x, int y) {
    char buf[64];
    snprintf(buf, sizeof(buf), "\033[%d;%dH", y + 1, x + 1);
    term_write(buf);
}

void term_write(const char *s) {
    if (!s) return;
    (void)!write(STDOUT_FILENO, s, strlen(s));
}

void term_write_n(const char *s, size_t n) {
    if (!s || n == 0) return;
    (void)!write(STDOUT_FILENO, s, n);
}

void term_enter_alt_screen(void) { term_write("\033[?1049h"); }
void term_leave_alt_screen(void) { term_write("\033[?1049l"); }
