// src/parser.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "parser.h"
#include "graph.h"

void parse_args(int argc, char **argv, Mode *out_mode, char **out_input) {
    if (argc != 5) {
        fprintf(stderr,
            "Usage: %s --mode [mst|held-karp|myalgo] --input <file.tsp>\n",
            argv[0]);
        exit(1);
    }
    *out_mode = MODE_MST;
    *out_input = NULL;

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "--mode") == 0) {
            if (strcmp(argv[i+1], "mst") == 0) {
                *out_mode = MODE_MST;
            } else if (strcmp(argv[i+1], "held-karp") == 0) {
                *out_mode = MODE_HELD_KARP;
            } else if (strcmp(argv[i+1], "myalgo") == 0) {
                *out_mode = MODE_MYALGO;
            } else {
                fprintf(stderr, "Unknown mode: %s\n", argv[i+1]);
                exit(1);
            }
        } else if (strcmp(argv[i], "--input") == 0) {
            *out_input = argv[i+1];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            exit(1);
        }
    }
    if (!*out_input) {
        fprintf(stderr, "--input option is required\n");
        exit(1);
    }
}

Graph *parse_tsp_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;

    int n = 0;
    char line[256];
    while (fgets(line, sizeof line, f)) {
        if (strncmp(line, "DIMENSION", 9) == 0) {
            char *p = strchr(line, ':');
            if (p) {
                n = atoi(p+1);
                break;
            }
        }
    }
    if (n <= 0) {
        fclose(f);
        return NULL;
    }

    Graph *g = graph_create(n);

    rewind(f);
    while (fgets(line, sizeof line, f)) {
        if (strncmp(line, "NODE_COORD_SECTION", 18) == 0) break;
    }

    int id;
    double x,y;
    for (int i = 0; i < n; i++) {
        if (fscanf(f, "%d %lf %lf", &id, &x, &y) != 3) {
            graph_free(g);
            fclose(f);
            return NULL;
        }
        if (id < 1 || id > n) {
            graph_free(g);
            fclose(f);
            return NULL;
        }
        g->coords[id-1][0] = x;
        g->coords[id-1][1] = y;
    }
    fclose(f);

    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            if (u == v) continue;
            double dx = g->coords[u][0] - g->coords[v][0];
            double dy = g->coords[u][1] - g->coords[v][1];
            int w = (int)(sqrt(dx*dx + dy*dy) + 0.5);
            graph_set_edge(g, u, v, w);
        }
    }

    return g;
}