#include "cli.h"
#include "pattern.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#define ARGC(args) ((int)(sizeof(args) / sizeof(args[0])))
#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s: failed: %s\n", __FILE__, __LINE__, __func__, #condition); \
        return 0; \
    } \
} while (0)

static int parse(int argc, char **argv, StartupConfig *cfg, char *message, size_t size) {
    FILE *err = tmpfile();
    if (!err) return -2;
    int result = cli_parse_args(argc, argv, cfg, err);
    if (fseek(err, 0, SEEK_SET) != 0) { fclose(err); return -2; }
    size_t n = fread(message, 1, size - 1, err);
    message[n] = '\0';
    int read_error = ferror(err), close_error = fclose(err);
    return read_error || close_error ? -2 : result;
}

static int config_equal(const StartupConfig *a, const StartupConfig *b) {
    return a->width == b->width && a->height == b->height &&
           a->delay_ms == b->delay_ms && a->seed == b->seed &&
           a->toroidal == b->toroidal && a->mode == b->mode &&
           strcmp(a->pattern_name, b->pattern_name) == 0 &&
           strcmp(a->load_path, b->load_path) == 0 &&
           a->placement.centered == b->placement.centered &&
           a->placement.has_xy == b->placement.has_xy &&
           a->placement.x == b->placement.x && a->placement.y == b->placement.y;
}

static int cli_selection(void) {
    StartupConfig cfg;
    char message[512];
    cli_startup_defaults(&cfg);
    char *empty[] = {"gol"};
    CHECK(parse(ARGC(empty), empty, &cfg, message, sizeof(message)) == 0);
    CHECK(cfg.mode == STARTUP_RANDOM && cfg.placement.centered && !cfg.placement.has_xy);
    CHECK(!cfg.width && !cfg.height && cfg.delay_ms == 100 && cfg.seed == 42 && cfg.toroidal);
    for (size_t i = 0; i < pattern_count(); ++i) {
        cli_startup_defaults(&cfg);
        char *args[] = {"gol", "--pattern", (char *)pattern_at(i)->name};
        CHECK(parse(ARGC(args), args, &cfg, message, sizeof(message)) == 0 && !message[0]);
        CHECK(cfg.mode == STARTUP_PATTERN && strcmp(cfg.pattern_name, pattern_at(i)->name) == 0);
        CHECK(!cfg.load_path[0] && cfg.placement.centered && !cfg.placement.has_xy);
    }
    char *file[] = {"gol", "--load", "file with spaces.cells", "--seed", "5",
                    "--bounded", "--width", "40", "--height", "10"};
    cli_startup_defaults(&cfg);
    CHECK(parse(ARGC(file), file, &cfg, message, sizeof(message)) == 0);
    CHECK(cfg.mode == STARTUP_LOAD && strcmp(cfg.load_path, "file with spaces.cells") == 0);
    CHECK(!cfg.pattern_name[0] && cfg.placement.centered && cfg.seed == 5 && !cfg.toroidal);
    CHECK(cfg.width == 40 && cfg.height == 10);
    char *repeat[] = {"gol", "--pattern", "glider", "--pattern", "block"};
    CHECK(parse(ARGC(repeat), repeat, &cfg, message, sizeof(message)) == 0);
    CHECK(cfg.mode == STARTUP_PATTERN && strcmp(cfg.pattern_name, "block") == 0 && !cfg.load_path[0]);
    char path[256]; memset(path, 'p', sizeof(path) - 1); path[255] = '\0';
    char *boundary[] = {"gol", "--load", path};
    CHECK(parse(ARGC(boundary), boundary, &cfg, message, sizeof(message)) == 0);
    CHECK(strlen(cfg.load_path) == 255 && strcmp(cfg.load_path, path) == 0 && !cfg.pattern_name[0]);
    return 1;
}

static int cli_placement(void) {
    const struct { char *args[12]; int x, y; } explicit[] = {
        {{"gol", "--pattern", "glider", "--x", "0", "--y", "7"}, 0, 7},
        {{"gol", "--center", "--y", "7", "--x", "0", "--pattern", "glider"}, 0, 7},
        {{"gol", "--load", "board.cells", "--x", "0", "--y", "7", "--center"}, 0, 7},
        {{"gol", "--pattern", "block", "--x", "0", "--x", "2", "--y", "7"}, 2, 7},
    };
    StartupConfig cfg;
    char message[512];
    for (size_t i = 0; i < sizeof(explicit) / sizeof(explicit[0]); ++i) {
        int argc = 0; while (explicit[i].args[argc]) ++argc;
        cli_startup_defaults(&cfg);
        CHECK(parse(argc, (char **)explicit[i].args, &cfg, message, sizeof(message)) == 0);
        CHECK(cfg.placement.has_xy && !cfg.placement.centered);
        CHECK(cfg.placement.x == explicit[i].x && cfg.placement.y == explicit[i].y);
    }
    cli_startup_defaults(&cfg);
    char *center[] = {"gol", "--load", "board.cells", "--center"};
    CHECK(parse(ARGC(center), center, &cfg, message, sizeof(message)) == 0);
    CHECK(cfg.placement.centered && !cfg.placement.has_xy);
    char max_int[64], max_seed[64];
    snprintf(max_int, sizeof(max_int), "%d", INT_MAX);
    snprintf(max_seed, sizeof(max_seed), "%u", UINT_MAX);
    char *maximum[] = {"gol", "--pattern", "glider", "--x", max_int, "--y", "0", "--seed", max_seed};
    CHECK(parse(ARGC(maximum), maximum, &cfg, message, sizeof(message)) == 0);
    CHECK(cfg.placement.x == INT_MAX && cfg.placement.y == 0 && cfg.seed == UINT_MAX);
    return 1;
}

static int cli_numbers(void) {
    const char *flags[] = {"--width", "--height", "--delay", "--seed", "--x", "--y"};
    const char *bad[] = {"", "-1", "+1", "1x", "1.0", "1e3", " 1", "1 ", "\t1",
                         "18446744073709551616"};
    StartupConfig cfg;
    char message[512], maximum[64], overflow[64];
    for (size_t f = 0; f < sizeof(flags)/sizeof(flags[0]); ++f) {
        int coordinate = f >= 4;
        unsigned long long limit = f == 3 ? UINT_MAX : INT_MAX;
        snprintf(maximum, sizeof(maximum), "%llu", limit);
        snprintf(overflow, sizeof(overflow), "%llu", limit + 1);
        char *args[] = {"gol", (char *)flags[f], NULL, "--pattern", "block",
                        f == 4 ? "--y" : "--x", "0"};
        int argc = coordinate ? ARGC(args) : 3;
        for (size_t v = 0; v < sizeof(bad)/sizeof(bad[0]) + 2; ++v) {
            args[2] = v < sizeof(bad)/sizeof(bad[0]) ? (char *)bad[v] :
                      v == sizeof(bad)/sizeof(bad[0]) ? overflow : NULL;
            cli_startup_defaults(&cfg);
            cfg.width = 23;
            StartupConfig before = cfg;
            CHECK(parse(argc, args, &cfg, message, sizeof(message)) == -1);
            CHECK(strstr(message, flags[f]) && config_equal(&cfg, &before));
        }
        args[2] = f < 3 ? "1" : "0";
        cli_startup_defaults(&cfg);
        CHECK(parse(argc, args, &cfg, message, sizeof(message)) == 0 && !message[0]);
        args[2] = maximum;
        CHECK(parse(argc, args, &cfg, message, sizeof(message)) == 0 && !message[0]);
        if (f < 3) {
            args[2] = "0";
            CHECK(parse(argc, args, &cfg, message, sizeof(message)) == -1);
        }
        char *missing[] = {"gol", (char *)flags[f]};
        CHECK(parse(ARGC(missing), missing, &cfg, message, sizeof(message)) == -1);
        CHECK(strstr(message, flags[f]));
    }
    char *null_program[] = {NULL, "--help"};
    CHECK(parse(ARGC(null_program), null_program, &cfg, message, sizeof(message)) == -1);
    char *null_option[] = {"gol", NULL};
    CHECK(parse(ARGC(null_option), null_option, &cfg, message, sizeof(message)) == -1);
    char *control[] = {"gol", "--bad\033option"};
    CHECK(parse(ARGC(control), control, &cfg, message, sizeof(message)) == -1);
    CHECK(!strchr(message, '\033') && strstr(message, "--help"));
    return 1;
}

static int cli_invalid(void) {
    char overflow[64], long_path[257];
    snprintf(overflow, sizeof(overflow), "%llu", (unsigned long long)INT_MAX + 1);
    memset(long_path, 'p', sizeof(long_path) - 1); long_path[256] = '\0';
    const struct { char *args[12]; const char *error; } invalid[] = {
        {{"gol", "--pattern"}, "--pattern requires"},
        {{"gol", "--pattern", "--center"}, "--pattern requires"},
        {{"gol", "--pattern", "unknown"}, "unknown pattern"},
        {{"gol", "--pattern", "Glider"}, "unknown pattern"},
        {{"gol", "--pattern", ""}, "unknown pattern"},
        {{"gol", "--load"}, "--load requires"},
        {{"gol", "--load", "--center"}, "--load requires"},
        {{"gol", "--load", ""}, "nonempty"},
        {{"gol", "--load", long_path}, "exceeds 255"},
        {{"gol", "--pattern", "block", "--load", "file.cells"}, "mutually exclusive"},
        {{"gol", "--load", "file.cells", "--pattern", "block"}, "mutually exclusive"},
        {{"gol", "--pattern", "block", "--x", "0"}, "supplied together"},
        {{"gol", "--pattern", "block", "--y", "0"}, "supplied together"},
        {{"gol", "--x", "0", "--y", "1"}, "require --pattern or --load"},
        {{"gol", "--center"}, "require --pattern or --load"},
        {{"gol", "--x"}, "--x requires"},
        {{"gol", "--pattern", "block", "--x", "-1", "--y", "0"}, "--x"},
        {{"gol", "--pattern", "block", "--x", "", "--y", "0"}, "--x"},
        {{"gol", "--pattern", "block", "--x", "1x", "--y", "0"}, "--x"},
        {{"gol", "--pattern", "block", "--x", " 1", "--y", "0"}, "--x"},
        {{"gol", "--pattern", "block", "--x", "+1", "--y", "0"}, "--x"},
        {{"gol", "--pattern", "block", "--x", overflow, "--y", "0"}, "--x"},
        {{"gol", "--seed", "-1"}, "--seed"},
        {{"gol", "--seed", "99999999999999999999999999"}, "--seed"},
        {{"gol", "--width", overflow}, "--width"},
        {{"gol", "--height", "0"}, "--height"},
        {{"gol", "--delay", "0"}, "--delay"},
        {{"gol", "--bounded", "--toroidal"}, "mutually exclusive"},
        {{"gol", "--toroidal", "--bounded"}, "mutually exclusive"},
        {{"gol", "--unknown"}, "unknown option"},
    };
    StartupConfig cfg;
    char message[512];
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        int argc = 0; while (invalid[i].args[argc]) ++argc;
        cli_startup_defaults(&cfg);
        cfg.width = 23;
        StartupConfig before = cfg;
        CHECK(parse(argc, (char **)invalid[i].args, &cfg, message, sizeof(message)) == -1);
        CHECK(strstr(message, invalid[i].error) != NULL);
        CHECK(config_equal(&cfg, &before));
    }
    CHECK(parse(1, NULL, &cfg, message, sizeof(message)) == -1);
    char *args[] = {"gol"};
    CHECK(parse(1, args, NULL, message, sizeof(message)) == -1);
    return 1;
}

int main(int argc, char **argv) {
    const char *selection = argc == 2 ? argv[1] : "all";
    const struct { const char *name; int (*run)(void); } cases[] = {
        {"cli-selection", cli_selection}, {"cli-placement", cli_placement}, {"cli-invalid", cli_invalid},
        {"cli-numbers", cli_numbers},
    };
    int selected = 0, failed = 0;
    if (argc > 2) return 2;
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        if (strcmp(selection, "all") && strcmp(selection, cases[i].name)) continue;
        ++selected;
        if (cases[i].run()) printf("PASS %s\n", cases[i].name);
        else {
            ++failed;
            fprintf(stderr, "FAIL %s; reproduce: make test-cli TEST_CASE=%s\n", cases[i].name, cases[i].name);
        }
    }
    if (!selected) { fprintf(stderr, "Unknown/empty CLI case: '%s'\n", selection); return 2; }
    printf("CLI tests: %d selected, %d passed, %d failed\n", selected, selected - failed, failed);
    return failed ? 1 : 0;
}
