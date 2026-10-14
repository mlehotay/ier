#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include "app.h"

void cli_startup_defaults(StartupConfig *cfg);

/* Parse into an initialized config. Return 0 on success, 1 for help, -1 on
   error. Help/errors leave the config unchanged. Placement coordinates override
   centering; app_init applies the selected source after successful parsing. */
int cli_parse_args(
    int argc,
    char **argv,
    StartupConfig *cfg,
    FILE *err
);

void cli_print_usage(FILE *out, const char *progname);

#endif
