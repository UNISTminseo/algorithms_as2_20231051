// src/mst_approx.c

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mst_approx.h"
#include "graph.h"

static int *prim_mst_parents(const Graph *g) {
    int n = g->n;
    int *parent = malloc(sizeof(int) * n);
    int *key    = malloc(sizeof(int) * n);
    int *inMST  = malloc(sizeof(int) * n);
    if (!parent || !key || !inMST) {
        free(parent); free(key); free(inMST);
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        key[i]    = INT_MAX;
        inMST[i]  = 0;
        parent[i] = -1;
    }
    key[0] = 0;

    for (int count = 0; count < n - 1; count++) {
        int u = -1, minVal = INT_MAX;
        for (int v = 0; v < n; v++) {
            if (!inMST[v] && key[v] < minVal) {
                minVal = key[v];
                u = v;
            }
        }
        if (u < 0) break;
        inMST[u] = 1;

        for (int v = 0; v < n; v++) {
            int w = graph_get_weight(g, u, v);
            if (!inMST[v] && w < key[v]) {
                key[v]    = w;
                parent[v] = u;
            }
        }
    }

    free(key);
    free(inMST);
    return parent;
}

typedef struct AdjNode {
    int v;
    struct AdjNode *next;
} AdjNode;

typedef struct {
    AdjNode **heads;
    int n;
} AdjList;

static AdjList *adjlist_create(int n) {
    AdjList *al = malloc(sizeof(AdjList));
    if (!al) return NULL;
    al->n = n;
    al->heads = malloc(sizeof(AdjNode*) * n);
    if (!al->heads) {
        free(al);
        return NULL;
    }
    for (int i = 0; i < n; i++) {
        al->heads[i] = NULL;
    }
    return al;
}

static void adjlist_add_edge(AdjList *al, int u, int v) {
    AdjNode *node = malloc(sizeof(AdjNode));
    if (!node) return;
    node->v = v;
    node->next = al->heads[u];
    al->heads[u] = node;
}

static void adjlist_free(AdjList *al) {
    if (!al) return;
    for (int i = 0; i < al->n; i++) {
        AdjNode *cur = al->heads[i];
        while (cur) {
            AdjNode *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
    free(al->heads);
    free(al);
}

static void dfs_preorder(AdjList *al, int u, int *visited, int *tour_nodes, int *idx) {
    visited[u] = 1;
    tour_nodes[(*idx)++] = u;
    for (AdjNode *nbr = al->heads[u]; nbr; nbr = nbr->next) {
        if (!visited[nbr->v]) {
            dfs_preorder(al, nbr->v, visited, tour_nodes, idx);
        }
    }
}

int mst_approx_tour(const Graph *g, int *tour_out) {
    int n = g->n;

    int *parent = prim_mst_parents(g);
    if (!parent) {
        fprintf(stderr, "Error: prim_mst_parents failed\n");
        return INF;
    }

    AdjList *al = adjlist_create(n);
    if (!al) {
        free(parent);
        fprintf(stderr, "Error: adjlist_create failed\n");
        return INF;
    }
    for (int v = 1; v < n; v++) {
        int u = parent[v];
        if (u >= 0) {
            adjlist_add_edge(al, u, v);
            adjlist_add_edge(al, v, u);
        }
    }
    free(parent);

    int *visited = calloc(n, sizeof(int));
    int *preorder = malloc(sizeof(int) * n);
    if (!visited || !preorder) {
        adjlist_free(al);
        free(visited);
        free(preorder);
        fprintf(stderr, "Error: Memory allocation failed in dfs_preorder\n");
        return INF;
    }
    int idx = 0;
    dfs_preorder(al, 0, visited, preorder, &idx);

    int total_cost = 0;
    for (int i = 0; i < n - 1; i++) {
        int u = preorder[i];
        int v = preorder[i + 1];
        total_cost += graph_get_weight(g, u, v);
        tour_out[i] = u;
    }

    tour_out[n - 1] = preorder[n - 1];
    total_cost += graph_get_weight(g, preorder[n - 1], 0);
    tour_out[n] = 0;

    adjlist_free(al);
    free(visited);
    free(preorder);

    return total_cost;
}