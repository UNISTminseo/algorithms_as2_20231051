// src/graph.h
#ifndef GRAPH_H
#define GRAPH_H
#include <limits.h>
#define INF INT_MAX
#include <stddef.h>

typedef struct Graph {
    int   n;
    int **dist;
    double **coords;
} Graph;

Graph *graph_create(int n);
void   graph_free(Graph *g);

void graph_set_edge(Graph *g, int u, int v, int w);
int  graph_get_weight(const Graph *g, int u, int v);

void graph_get_xy(const Graph *g, int v, double *x, double *y);

#endif