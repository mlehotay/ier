#ifndef NUMBER_H
#define NUMBER_H

#include <errno.h>
#include <stdlib.h>

/* Strict decimal input shared by startup and runtime commands. */
static inline int number_parse(const char *text, unsigned long minimum,
                               unsigned long maximum, unsigned long *out) {
    if (!text || !*text || !out) return -1;
    for (const char *p = text; *p; ++p) {
        if (*p < '0' || *p > '9') return -1;
    }
    errno = 0;
    char *end;
    unsigned long value = strtoul(text, &end, 10);
    if (errno == ERANGE || *end || value < minimum || value > maximum) return -1;
    *out = value;
    return 0;
}

#endif
