#ifndef __STJN_H
#define __STJN_H

#include "alles.h"

#include "tmsp.h"

typedef struct stjn_t {
    int n_jobs;
    int n_total_jobs;
    //List *startnodes, *endnodes;
} stjn_t;

typedef struct node_t {
    int len, est, lst, i, j, order;
    int flex; // HACK, sorry
    int old_est;
    int flag;
    vector<node_t *> prev, next;
    vector<activity *> group;
    
    bool canmerge;
    
    node_t * node_pred /*est*/, * node_succ /*lst*/;
} node_t;

struct orderLT { bool operator()(node_t * x, node_t * y) const { return x->order < y->order; } };
struct orderGT { bool operator()(node_t * x, node_t * y) const { return x->order > y->order; } };

extern node_t * cycle_node; // deze wordt geset door de calc/update (est,lst) routines om aan te geven bij welke node EST>LST het eerst gebeurde. dan kun je twee kanten op tracen om het totale conflict te achterhalen. definitie in stjn.cpp

extern node_t ***acts;

int stjn_add_precedence(node_t *from, node_t *to);
node_t *stjn_merge_init(node_t *parent, node_t *child);
node_t *stjn_merge(node_t *n, node_t *m);
int stjn_construct();
void print_est_schedule();
void insertToTmsp();

#define BEFORE(a, b)   (b->lst >= (a->est + a->len))
#define SLACK(a, b)     (b->lst - a->est - a->len)
#endif
