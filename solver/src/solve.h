#ifndef SOLVE_H
#define SOLVE_H

int solve(int merge_bandwidth, int add_mutexes);

int solveSTJN();
int solveESTA(int add_mutexes);
int solveChaining(int add_mutexes);
int solveLP();

#endif // SOLVE_H
