#ifndef FLEXIBILITY_H
#define FLEXIBILITY_H

#include "ClpSimplex.hpp"
#include "constraints.h"

#include <assert.h>  

#include <set>
#include <vector>
#include <string>
#include <utility>

void setObjective(ClpSimplex* model, int n_cols, Constraints* constraints);
void addType1Constraints(ClpSimplex* model, int n_cols);
void addType2Constraints(ClpSimplex* model, Constraints* constraints);
void changeObjective(ClpSimplex* model, int n_cols);
void changeType1Constraints(ClpSimplex* model, int n_cols);

pair<double, map<string, double> > useClpToSolve (Constraints &constraints);

void addConstraints(Constraints* constraints);
void addLimits(Constraints* constraints);
void printSolution(pair<double, map<string, double> >* solution);

int flexibility();

#endif
