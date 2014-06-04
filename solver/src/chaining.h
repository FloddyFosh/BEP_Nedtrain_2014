#ifndef CHAINING_H
#define CHAINING_H

#include <list>

#include "alles.h"

#include "tmsp.h"
#include "stjn.h"

typedef struct chainId{
    int resource;
    int unit;
} chainid;

typedef struct chain{
    list<activity*> activities;
} chain;

void initializeActivities();
vector<activity*> getActivities();
bool compareEST(const activity* a, const activity* b);
bool operator<(const chainId& a, const chainId& b);

void initializeChains();
chainId selectChain(int tr, int act, int res);
void pushToChain(activity* act, chainId* id);

void add_frame();
void print_chain(int i, int j);

bool chaining();

#endif
