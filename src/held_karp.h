// src/held_karp.h

#ifndef HELD_KARP_H
#define HELD_KARP_H

#include "graph.h"

int held_karp_tour(const Graph *g, int *tour_out, double time_limit_s);

#endif