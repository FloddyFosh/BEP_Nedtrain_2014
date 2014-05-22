#ifndef CHAINING_H
#define CHAINING_H

#include <list>

#include "alles.h"

#include "tmsp.h"
#include "stjn.h"

bool compareEST(const activity*, const activity*);

pair<int,int> selectChain(int, int, int);

bool chaining();

#endif
