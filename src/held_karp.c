// src/held_karp.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "held_karp.h"
#include "graph.h"

int held_karp_tour(const Graph *g, int *tour_out, double time_limit_s) {
    int n = g->n;
    if (n > 25) {
        return INF;
    }

    int maxMask = 1 << n;
    const int INF_COST = INT_MAX / 2;

    int *dp = malloc(sizeof(int) * maxMask * n);
    if (!dp) return INF;

    int *parent = malloc(sizeof(int) * maxMask * n);
    if (!parent) {
        free(dp);
        return INF;
    }

    for (int mask = 0; mask < maxMask; mask++) {
        for (int i = 0; i < n; i++) {
            dp[mask * n + i] = INF_COST;
            parent[mask * n + i] = -1;
        }
    }

    dp[(1 << 0) * n + 0] = 0;

    clock_t start_clock = clock();

    for (int mask = 0; mask < maxMask; mask++) {
        if (!(mask & 1)) continue;

        if (time_limit_s > 0) {
            double elapsed = (double)(clock() - start_clock) / CLOCKS_PER_SEC;
            if (elapsed > time_limit_s) {
                free(dp);
                free(parent);
                return INF;
            }
        }

        for (int u = 0; u < n; u++) {
            if (!(mask & (1 << u))) continue;
            int current_cost = dp[mask * n + u];
            if (current_cost >= INF_COST) continue;

            for (int v = 0; v < n; v++) {
                if (mask & (1 << v)) continue;
                int w = graph_get_weight(g, u, v);
                if (w == INF) continue;
                int nextMask = mask | (1 << v);
                int newCost = current_cost + w;
                int idx = nextMask * n + v;
                if (newCost < dp[idx]) {
                    dp[idx]      = newCost;
                    parent[idx]  = u;
                }
            }
        }
    }

    int fullMask = maxMask - 1;
    int bestCost = INF_COST;
    int bestEnd = -1;
    for (int u = 1; u < n; u++) {
        int c = dp[fullMask * n + u];
        int returnCost = graph_get_weight(g, u, 0);
        if (c < INF_COST && returnCost < INF) {
            int total = c + returnCost;
            if (total < bestCost) {
                bestCost = total;
                bestEnd = u;
            }
        }
    }
    if (bestEnd < 0) {
        free(dp);
        free(parent);
        return INF;
    }

    int *path = malloc(sizeof(int) * (n + 1));
    if (!path) {
        free(dp);
        free(parent);
        return INF;
    }
    path[n]     = 0;
    path[n - 1] = bestEnd;

    int mask = fullMask;
    for (int idx = n - 2; idx >= 0; idx--) {
        int u = path[idx + 1];
        int p = parent[mask * n + u];
        path[idx] = p;
        mask = mask ^ (1 << u);
    }
 
    for (int i = 0; i <= n; i++) {
        tour_out[i] = path[i];
    }

    free(path);
    free(dp);
    free(parent);
    return bestCost;
}