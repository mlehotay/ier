#ifndef TERM_H
#define TERM_H

#include <stddef.h>
#include <signal.h>

int  term_enable_raw_mode(void);
void term_restore(void);

int  term_get_size(int *w, int *h);
int  term_read_key_nonblocking(void);

void term_hide_cursor(void);
void term_show_cursor(void);

void term_clear_screen(void);
void term_move_cursor(int x, int y);   /* 0-based logical coords */
void term_write(const char *s);
void term_write_n(const char *s, size_t n);

void term_enter_alt_screen(void);
void term_leave_alt_screen(void);

extern volatile sig_atomic_t term_resize_pending;
void term_install_signal_handlers(void);

#endif
