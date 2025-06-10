// src/my_algo.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>

#include "my_algo.h"
#include "graph.h"

#define DELTA         5
#define RUNS_SMALL    50
#define RUNS_MEDIUM   30
#define TIME_BOUND 60.0

static long long global_start_ms, global_time_bound_ms;
static long long now_ms(void) {
    struct timeval tv; gettimeofday(&tv, NULL);
    return tv.tv_sec*1000LL + tv.tv_usec/1000;
}
static inline int time_exceeded(void) {
    return now_ms() - global_start_ms >= global_time_bound_ms;
}

static int  tour_cost(const Graph*, const int*);
static void greedy_nn(const Graph*, int*, int start);
static void full_two_opt(const Graph*, int*, int);
static int  run_fixed(const Graph*, int*, int);
static int  run_timed(const Graph*, int*, double);

int my_algo_tour(const Graph *g, int *tour_out) {
    int n = g->n;
    if (n <= 5000)        return run_fixed(g, tour_out, RUNS_SMALL);
    else if (n <= 20000)  return run_fixed(g, tour_out, RUNS_MEDIUM);
    else                  return run_timed(g, tour_out, TIME_BOUND);
}

static int run_fixed(const Graph *g, int *tour_out, int runs) {
    int n = g->n;
    int *best = malloc((n+1)*sizeof(int));
    int *tmp  = malloc((n+1)*sizeof(int));
    if (!best || !tmp) return INF;
    int best_c = INT_MAX;

    srand((unsigned)time(NULL));
    for (int r = 0; r < runs; r++) {
        int start = rand() % n;
        greedy_nn(g, tmp, start);
        full_two_opt(g, tmp, DELTA);
        int c = tour_cost(g, tmp);
        if (c < best_c) {
            best_c = c;
            memcpy(best, tmp, (n+1)*sizeof(int));
        }
    }

    memcpy(tour_out, best, (n+1)*sizeof(int));
    free(best); free(tmp);
    return best_c;
}

static int run_timed(const Graph *g, int *tour_out, double bound_sec) {
    int n = g->n;
    int *best = malloc((n+1)*sizeof(int));
    if (!best) return INF;
    int *tmp = malloc((n+1)*sizeof(int));
    if (!tmp) { free(best); return INF; }
    int best_c = INT_MAX;

    global_start_ms      = now_ms();
    global_time_bound_ms = (long long)(bound_sec * 1000);

    srand((unsigned)time(NULL));
    while (!time_exceeded()) {
        int start = rand() % n;
        greedy_nn(g, tmp, start);
        full_two_opt(g, tmp, DELTA);
        int c = tour_cost(g, tmp);
        if (c < best_c) {
            best_c = c;
            memcpy(best, tmp, (n+1)*sizeof(int));
        }
    }

    memcpy(tour_out, best, (n+1)*sizeof(int));
    free(best); free(tmp);
    return best_c;
}

static void greedy_nn(const Graph *g, int *tour, int start) {
    int n = g->n;
    char *vis = calloc(n,1);
    tour[0] = start;
    vis[start] = 1;
    for (int i = 1; i < n; i++) {
        int prev = tour[i-1];
        int best_v = -1, bd = INT_MAX;
        for (int v = 0; v < n; v++) {
            if (vis[v]) continue;
            int d = graph_get_weight(g, prev, v);
            if (d < bd) {
                bd = d;
                best_v = v;
            }
        }
        tour[i] = best_v;
        vis[best_v] = 1;
    }
    tour[n] = tour[0];
    free(vis);
}

static void full_two_opt(const Graph *g, int *tour, int max_delta) {
    int n = g->n, improved = 1;
    while (improved) {
        improved = 0;
        for (int i = 1; i < n-1; i++) {
            int end = (i + max_delta < n-1) ? i + max_delta : n-1;
            for (int j = i+1; j <= end; j++) {
                int a = tour[i-1], b = tour[i], c = tour[j], d = tour[j+1];
                int delta = graph_get_weight(g,a,c)
                          + graph_get_weight(g,b,d)
                          - graph_get_weight(g,a,b)
                          - graph_get_weight(g,c,d);
                if (delta < 0) {
                    for (int p = i, q = j; p < q; p++, q--) {
                        int t = tour[p]; tour[p] = tour[q]; tour[q] = t;
                    }
                    improved = 1;
                    break;
                }
            }
            if (improved) break;
        }
    }
}

static int tour_cost(const Graph *g, const int *tour) {
    long long sum = 0;
    for (int i = 0; i < g->n; i++) {
        sum += graph_get_weight(g, tour[i], tour[i+1]);
    }
    return (int)sum;
}