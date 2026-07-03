#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include "app.h"

void cli_startup_defaults(StartupConfig *cfg);

int cli_parse_args(
    int argc,
    char **argv,
    StartupConfig *cfg,
    FILE *err
);

void cli_print_usage(FILE *out, const char *progname);

#endif
