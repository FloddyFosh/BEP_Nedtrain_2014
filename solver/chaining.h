#ifndef CHAINING_H
#define CHAINING_H

#include <list>

#include "alles.h"

#include "tmsp.h"
#include "stjn.h"

bool compareActivities(const activity*, const activity*);

list<activity*>* selectChain(int, int, int);

int chaining();

#endif
