#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include "pattern.h"
#include "cli.h"
#include "number.h"

static int cli_error(FILE *err, const char *fmt, ...) {
    va_list ap;
    if (!err) err = stderr;
    char detail[1024];
    va_start(ap, fmt);
    vsnprintf(detail, sizeof(detail), fmt, ap);
    va_end(ap);
    for (char *p = detail; *p; ++p) {
        if ((unsigned char)*p < 32 || (unsigned char)*p >= 127) *p = '?';
    }
    fprintf(err, "error: %s\n", detail);
    return -1;
}

void cli_startup_defaults(StartupConfig *cfg) {
    memset(cfg, 0, sizeof(*cfg));
    cfg->width = 0;
    cfg->height = 0;
    cfg->seed = 42;
    cfg->delay_ms = 100;
    cfg->toroidal = 1;
    cfg->mode = STARTUP_RANDOM;
    cfg->placement.centered = 1;
}

static int parse_int(const char *flag, const char *s, int minimum, int *out, FILE *err) {
    unsigned long v;
    if (number_parse(s, (unsigned long)minimum, INT_MAX, &v) != 0) {
        return cli_error(err, "%s must be a decimal integer from %d to %d", flag, minimum, INT_MAX);
    }
    *out = (int)v;
    return 0;
}

static int parse_unsigned(const char *flag, const char *s, unsigned int *out, FILE *err) {
    unsigned long v;
    if (number_parse(s, 0, UINT_MAX, &v) != 0) {
        return cli_error(err, "%s must be a decimal integer from 0 to %u", flag, UINT_MAX);
    }
    *out = (unsigned int)v;
    return 0;
}

static int copy_option(const char *flag, const char *value, char *out, size_t size, FILE *err) {
    if (!value[0]) return cli_error(err, "%s requires a nonempty value", flag);
    if (strlen(value) >= size) return cli_error(err, "%s value exceeds %zu bytes", flag, size - 1);
    strcpy(out, value);
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
        "  --pattern NAME Select a built-in pattern\n"
        "  --load FILE    Select a .cells file\n"
        "  --x N          Placement x coordinate (requires --y)\n"
        "  --y N          Placement y coordinate (requires --x)\n"
        "  --center       Center the pattern (default; coordinates override)\n"
        "  --bounded      Use bounded topology\n"
        "  --toroidal     Use toroidal topology\n"
        "  --help         Show this help\n",
        prog
    );
}

int cli_parse_args(int argc, char **argv, StartupConfig *cfg, FILE *err) {
    if (!cfg || !argv || argc < 1 || !argv[0]) return cli_error(err, "invalid CLI arguments");
    StartupConfig parsed = *cfg;
    int saw_pattern = 0, saw_load = 0, saw_x = 0, saw_y = 0, saw_center = 0;
    int saw_bounded = 0;
    int saw_toroidal = 0;

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        if (!arg) return cli_error(err, "missing argument");

        if (strcmp(arg, "--help") == 0) {
            cli_print_usage(stdout, argv[0]);
            return 1;
        }
        else if (strcmp(arg, "--width") == 0) {
            if (++i >= argc) return cli_error(err, "--width requires a value");
            if (parse_int("--width", argv[i], 1, &parsed.width, err) < 0) return -1;
        }
        else if (strcmp(arg, "--height") == 0) {
            if (++i >= argc) return cli_error(err, "--height requires a value");
            if (parse_int("--height", argv[i], 1, &parsed.height, err) < 0) return -1;
        }
        else if (strcmp(arg, "--delay") == 0) {
            if (++i >= argc) return cli_error(err, "--delay requires a value");
            if (parse_int("--delay", argv[i], 1, &parsed.delay_ms, err) < 0) return -1;
        }
        else if (strcmp(arg, "--seed") == 0) {
            if (++i >= argc) return cli_error(err, "--seed requires a value");
            if (parse_unsigned("--seed", argv[i], &parsed.seed, err) < 0) return -1;
        }
        else if (strcmp(arg, "--pattern") == 0) {
            if (++i >= argc || !argv[i] || strncmp(argv[i], "--", 2) == 0) {
                return cli_error(err, "--pattern requires a name");
            }
            if (!pattern_lookup(argv[i])) return cli_error(err, "unknown pattern '%s'; use block, blinker, toad, beacon, glider, lwss or gosper", argv[i]);
            if (copy_option("--pattern", argv[i], parsed.pattern_name, sizeof(parsed.pattern_name), err) < 0) return -1;
            parsed.mode = STARTUP_PATTERN;
            saw_pattern = 1;
        }
        else if (strcmp(arg, "--load") == 0) {
            if (++i >= argc || !argv[i] || strncmp(argv[i], "--", 2) == 0) {
                return cli_error(err, "--load requires a file");
            }
            if (copy_option("--load", argv[i], parsed.load_path, sizeof(parsed.load_path), err) < 0) return -1;
            parsed.mode = STARTUP_LOAD;
            saw_load = 1;
        }
        else if (strcmp(arg, "--x") == 0 || strcmp(arg, "--y") == 0) {
            int is_x = strcmp(arg, "--x") == 0;
            if (++i >= argc || !argv[i]) return cli_error(err, "%s requires a value", arg);
            int *value = is_x ? &parsed.placement.x : &parsed.placement.y;
            if (parse_int(arg, argv[i], 0, value, err) < 0) return -1;
            if (is_x) saw_x = 1;
            else saw_y = 1;
        }
        else if (strcmp(arg, "--center") == 0) {
            saw_center = 1;
        }
        else if (strcmp(arg, "--bounded") == 0) {
            saw_bounded = 1;
        }
        else if (strcmp(arg, "--toroidal") == 0) {
            saw_toroidal = 1;
        }
        else {
            return cli_error(err, "unknown option '%s'; use --help for usage", arg);
        }
    }

    if (saw_pattern && saw_load) return cli_error(err, "--pattern and --load are mutually exclusive");
    if (saw_x != saw_y) return cli_error(err, "--x and --y must be supplied together");
    if ((saw_x || saw_y || saw_center) && parsed.mode == STARTUP_RANDOM) {
        return cli_error(err, "placement options require --pattern or --load");
    }
    if (saw_x && saw_y) {
        parsed.placement.has_xy = 1;
        parsed.placement.centered = 0;
    } else if (saw_center || saw_pattern || saw_load) {
        parsed.placement.has_xy = 0;
        parsed.placement.centered = 1;
    }
    if (parsed.mode == STARTUP_PATTERN) parsed.load_path[0] = '\0';
    if (parsed.mode == STARTUP_LOAD) parsed.pattern_name[0] = '\0';
    if (saw_bounded && saw_toroidal) {
        return cli_error(err, "--bounded and --toroidal are mutually exclusive");
    }

    if (saw_bounded) parsed.toroidal = 0;
    if (saw_toroidal) parsed.toroidal = 1;
    *cfg = parsed;

    return 0;
}
