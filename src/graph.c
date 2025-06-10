// src/graph.c

#include <stdlib.h>
#include <limits.h>
#include "graph.h"

Graph *graph_create(int n) {
    Graph *g = malloc(sizeof(Graph));
    if (!g) return NULL;

    g->n = n;

    g->dist = malloc(sizeof(int*) * n);
    if (!g->dist) { free(g); return NULL; }
    for (int i = 0; i < n; i++) {
        g->dist[i] = malloc(sizeof(int) * n);
        if (!g->dist[i]) {
            for (int j = 0; j < i; j++) free(g->dist[j]);
            free(g->dist);
            free(g);
            return NULL;
        }
    }

    g->coords = malloc(sizeof(double*) * n);
    if (!g->coords) {
        for (int i = 0; i < n; i++) free(g->dist[i]);
        free(g->dist);
        free(g);
        return NULL;
    }
    for (int i = 0; i < n; i++) {
        g->coords[i] = malloc(sizeof(double) * 2);
        if (!g->coords[i]) {
            for (int j = 0; j < i; j++) free(g->coords[j]);
            free(g->coords);
            for (int j = 0; j < n; j++) free(g->dist[j]);
            free(g->dist);
            free(g);
            return NULL;
        }
    }

    return g;
}

void graph_free(Graph *g) {
    if (!g) return;

    for (int i = 0; i < g->n; i++) {
        free(g->dist[i]);
    }
    free(g->dist);

    for (int i = 0; i < g->n; i++) {
        free(g->coords[i]);
    }
    free(g->coords);

    free(g);
}

void graph_set_edge(Graph *g, int u, int v, int w) {
    if (!g) return;
    if (u < 0 || u >= g->n || v < 0 || v >= g->n) return;
    g->dist[u][v] = w;
}

int graph_get_weight(const Graph *g, int u, int v) {
    if (!g)        return INT_MAX;
    if (u < 0 || u >= g->n ||
        v < 0 || v >= g->n) return INT_MAX;
    return g->dist[u][v];
}

void graph_get_xy(const Graph *g, int v, double *x, double *y) {
    if (!g || v < 0 || v >= g->n) {
        *x = *y = 0.0;
    } else {
        *x = g->coords[v][0];
        *y = g->coords[v][1];
    }
}