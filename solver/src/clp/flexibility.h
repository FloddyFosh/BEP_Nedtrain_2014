#ifndef FLEXIBILITY_H
#define FLEXIBILITY_H

#include "ClpSimplex.hpp"
#include "constraints.h"

#include <assert.h>  

#include <set>
#include <vector>
#include <string>
#include <utility>

pair<double, map<string, double> > useClpToSolve (Constraints constraints);

#endif
