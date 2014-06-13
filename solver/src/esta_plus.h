#ifndef __ESTA_PLUS_H
#define __ESTA_PLUS_H

#include "alles.h"

extern int leveling_constraints_before_chaining;

typedef struct peak_t {
	int i; // train
	int j; // activity
	int r; // resource
} peak_t;

typedef struct conflict_t {
    node_t *n1, *n2; // was: n, m
	int type;
} conflict_t;

typedef struct peak2_t {
	int time, resource, capacity;
	vector<node_t *> activities;
} peak2_t;

int esta_plus(int merge_bandwidth, int add_mutexes);

/* Returns true iff a2's earliest possible run is running at a1.start.earliest. */
#define P_ik(i1,j1,i2,j2)	((i1 == i2 && j1 == j2) || \
                             (acts[i2][j2]->est <= acts[i1][j1]->est && acts[i1][j1]->est < (acts[i2][j2]->est + acts[i2][j2]->len)))
#endif
