#ifndef CHAINING_H
#define CHAINING_H

#include <list>

#include "alles.h"

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
chainId selectFirstChain(int tr, int act, int res); //returns id of the first feasible chain that is found
chainId selectRandomChain(int tr, int act, int res); //returns id of a random feasible chain
void pushToBestChains(int tr, int act, int res); //Uses heuristic found in [Generating Robust Partial Order Schedules, Policella 2004]
void pushToChain(activity* act, chainId* id); //Adds activity to chain and posts contraint act < last(chain)

//Creates a frame of the current state of the program to be displayed in the GUI.
//This frame includes EST, LST of every activity/group and each precedence constraint posted after the previous frame.
void add_frame();

//Prints each activity present in chain with chainId (i,j) to be parsed by the planner.
void print_chain(int i, int j);

//returns true if process was successful
bool chaining();

#endif
