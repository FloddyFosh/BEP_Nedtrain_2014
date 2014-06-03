#ifndef CHAINING_H
#define CHAINING_H

#include <list>

#include "alles.h"

#include "tmsp.h"
#include "stjn.h"

void initializeActivities();
bool compareEST(const activity* a, const activity* b);

void initializeChains();
pair<int,int> selectChain(int tr, int act, int res);
void pushToChain(activity* act, pair<int,int>* chainId);

void add_frame();
void print_chain();

bool chaining();

#endif
