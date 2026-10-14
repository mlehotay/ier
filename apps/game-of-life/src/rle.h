#ifndef RLE_H
#define RLE_H

#include "cells.h"

/* Same owned normalized output, statuses and error locations as .cells.
   Initialize output to {0}; failure preserves it. Parsing never closes stream.
   Loading closes its file and commits output only after a successful close. */
CellsResult rle_parse(FILE *stream, CellsPattern *out, CellsError *error);
CellsResult rle_load(const char *path, CellsPattern *out, CellsError *error);

#endif
