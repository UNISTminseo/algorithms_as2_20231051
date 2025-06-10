// src/main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "graph.h"
#include "parser.h"
#include "mst_approx.h"
#include "held_karp.h"
#include "my_algo.h"

int main(int argc, char *argv[]) {
    Mode mode;
    char *input_filename;
    parse_args(argc, argv, &mode, &input_filename);

    Graph *g = parse_tsp_file(input_filename);
    if (!g) {
        fprintf(stderr, "Failed to parse TSP file: %s\n", input_filename);
        return 1;
    }

    int n = g->n;
    int *tour = malloc(sizeof(int)*(n+1));
    if (!tour) {
        graph_free(g);
        return 1;
    }

    clock_t t0 = clock();
    int cost = INF;

    if (mode == MODE_MST) {
        cost = mst_approx_tour(g, tour);
    }
    else if (mode == MODE_HELD_KARP) {
        cost = held_karp_tour(g, tour, 3600.0);
    }
    else if (mode == MODE_MYALGO) {
        cost = my_algo_tour(g, tour);
    }

    double elapsed = (double)(clock() - t0) / CLOCKS_PER_SEC;

    printf("Tour cost: %d\n", cost);
    printf("Tour sequence:");
    for (int i = 0; i <= n; i++) {
        printf(" %d", tour[i]);
    }
    printf("\n");
    printf("Elapsed time: %.3f seconds\n", elapsed);

    printf("\n=== Final Summary ===\n");
    printf("Final cost: %d\n", cost);
    printf("Total time: %.3f seconds\n", elapsed);

    free(tour);
    graph_free(g);
    return 0;
}