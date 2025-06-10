// src/parser.h

#ifndef PARSER_H
#define PARSER_H

#include "graph.h"

typedef enum {
    MODE_MST,
    MODE_HELD_KARP,
    MODE_MYALGO
} Mode;

void parse_args(int argc, char **argv, Mode *out_mode, char **out_input);

Graph *parse_tsp_file(const char *filename);

#endif