#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "cli.h"

static int cli_error(FILE *err, const char *fmt, ...) {
    va_list ap;
    fprintf(err, "error: ");
    va_start(ap, fmt);
    vfprintf(err, fmt, ap);
    va_end(ap);
    fprintf(err, "\n");
    return -1;
}

void cli_startup_defaults(StartupConfig *cfg) {
    memset(cfg, 0, sizeof(*cfg));
    cfg->width = 0;
    cfg->height = 0;
    cfg->seed = 42;
    cfg->delay_ms = 100;
    cfg->toroidal = 1;
}

static int parse_positive_int(const char *flag, const char *s, int *out, FILE *err) {
    char *end;
    long v = strtol(s, &end, 10);
    if (*end != '\0' || v <= 0) {
        return cli_error(err, "%s must be a positive integer", flag);
    }
    *out = (int)v;
    return 0;
}

static int parse_unsigned(const char *flag, const char *s, unsigned int *out, FILE *err) {
    char *end;
    unsigned long v = strtoul(s, &end, 10);
    if (*end != '\0') {
        return cli_error(err, "%s must be a non-negative integer", flag);
    }
    *out = (unsigned int)v;
    return 0;
}

void cli_print_usage(FILE *out, const char *prog) {
    fprintf(out,
        "Usage: %s [options]\n\n"
        "Options:\n"
        "  --width N      Set startup world width\n"
        "  --height N     Set startup world height\n"
        "  --delay MS     Set simulation delay in milliseconds\n"
        "  --seed N       Set deterministic random seed\n"
        "  --bounded      Use bounded topology\n"
        "  --toroidal     Use toroidal topology\n"
        "  --help         Show this help\n",
        prog
    );
}

int cli_parse_args(int argc, char **argv, StartupConfig *cfg, FILE *err) {
    int saw_bounded = 0;
    int saw_toroidal = 0;

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "--help") == 0) {
            cli_print_usage(stdout, argv[0]);
            return 1;
        }
        else if (strcmp(arg, "--width") == 0) {
            if (++i >= argc) return cli_error(err, "--width requires a value");
            if (parse_positive_int("--width", argv[i], &cfg->width, err) < 0) return -1;
        }
        else if (strcmp(arg, "--height") == 0) {
            if (++i >= argc) return cli_error(err, "--height requires a value");
            if (parse_positive_int("--height", argv[i], &cfg->height, err) < 0) return -1;
        }
        else if (strcmp(arg, "--delay") == 0) {
            if (++i >= argc) return cli_error(err, "--delay requires a value");
            if (parse_positive_int("--delay", argv[i], &cfg->delay_ms, err) < 0) return -1;
        }
        else if (strcmp(arg, "--seed") == 0) {
            if (++i >= argc) return cli_error(err, "--seed requires a value");
            if (parse_unsigned("--seed", argv[i], &cfg->seed, err) < 0) return -1;
        }
        else if (strcmp(arg, "--bounded") == 0) {
            saw_bounded = 1;
        }
        else if (strcmp(arg, "--toroidal") == 0) {
            saw_toroidal = 1;
        }
        else {
            return cli_error(err, "unknown option '%s'", arg);
        }
    }

    if (saw_bounded && saw_toroidal) {
        return cli_error(err, "--bounded and --toroidal are mutually exclusive");
    }

    if (saw_bounded) cfg->toroidal = 0;
    if (saw_toroidal) cfg->toroidal = 1;

    return 0;
}
