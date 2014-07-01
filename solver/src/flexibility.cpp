/*  Find flexibilty of an STP in two steps:
    1) Find minflex
    2) Find an interval set I_S which maximizes flex(S,I_S)

    earliest starting time  [est] --> t^-
    latest starting time    [lst] --> t^+

1)  maximize:       minflex
    constraints:1)  0 <= [lst] - [est] - [minflex] <= \infty \forall t
                2)  [lst] - [est'] <= c \forall (t - t' <= c) \in C

2)  maximize:       the sum of ([lst] - [est]) for each t \in T
    constraints:1)  [minflex] <= [lst] - [est] <= \infty \forall t
                2)  [lst] - [est'] <= c \forall (t - t' <= c) \in C

    Find all [est] and [lst]
    
    Only difference with the two steps is the objective function
    and the first constraint.
*/

#include <sstream>

#include "alles.h"
#include "output.h"
#include "tmsp.h"
#include "chaining.h"
#include "flexibility.h"

#define epsilon 0.001

using namespace std;

int flextotaal = 0;

int getFlexibility(){
    return flextotaal;
}

void setObjective(ClpInterior* model, int n_cols, Constraints* constraints) {
    // -COIN_DBL_MAX = -inf and COIN_DBL_MAX = +inf
    for(int i = 0; i < n_cols; i++) {
        model->setObjectiveCoefficient(i, ((i + 1) % 2) * 2.0 - 1.0);
        model->setColumnLower(i, constraints->getLowerLimit(i/2));
        model->setColumnUpper(i, constraints->getUpperLimit(i/2));
        model->setInteger(i);
    }
}

void addType1Constraints(ClpInterior* model, Constraints* constraints, int n_cols) {
    // add constraints: 0 <= [lst] - [est] - [minflex] <= \infty \forall t
    for(int i = 0; i < n_cols; i+=2) {
        // latest starting time is [i]
        // earliest starting time is [i+1]
        int cols[] = {i, i+1};
        double cfc[] = {1.0, -1.0}; // coefficients

        if(constraints->getLocked(i/2)) {
            model->addRow(2, cols, cfc, 0, 0);
        } else {
            model->addRow(2, cols, cfc, 0, COIN_DBL_MAX);
       }
    }
}

void addType2Constraints(ClpInterior* model, Constraints* constraints) {
    // add constraints: [lst] - [est'] <= c \forall (t - t' <= c) \in C
    for(int i = 0; i < constraints->size(); i++) {
        // [lst]  = constrain.t1 * 2    
        // [est'] = constrain.t2 * 2 + 1
        Constraint constrain = (*constraints)[i];
        int cols[] = {constrain.t1 * 2, constrain.t2 * 2 + 1};
        double cfc[] = {1.0, -1.0}; // coefficients
        model->addRow(2, cols, cfc, -COIN_DBL_MAX, constrain.c);
    }
}

map<string, int> useClpToSolve (Constraints* constraints) {
    int n_cols = constraints->getAmountOfVariables() * 2;    
    ClpInterior model; // child of ClpInterior
    model.setOptimizationDirection(-1); // maximize instead of minimize.
    model.resize(0, n_cols); 
    model.setLogLevel(0); // turns off all output of Clp
    
    setObjective(&model, n_cols, constraints);
    addType1Constraints(&model, constraints, n_cols);
    addType2Constraints(&model, constraints);
    
    // solve the problem for step 1
    model.primalDual();

    // get solution
    const double* sol = model.primalColumnSolution();
    flextotaal = (int) (model.objectiveValue() + epsilon);

    // return solution
    map<string, int> output;   
    for(int i = 0; i < n_cols; i+=2) {
        string varname = constraints->getVariableName(i/2);
        output[varname + " +"] = (int) (sol[i] + constraints->getFlex(i/2) + epsilon);
        output[varname + " -"] = (int) (sol[i+1] + epsilon);
    }
    return output;
}

void addConstraints(Constraints* constraints) {
    // a^+ - b^- <= -d_a
    for(int i = 0; i < (int) tmsp->precedences.size(); i++) {
        int i1 = P(i)->i1; // a^+
        int j1 = P(i)->j1; // a^+
        int i2 = P(i)->i2; // b^-
        int j2 = P(i)->j2; // b^-
        stringstream ss1; 
        stringstream ss2; 
        ss1 << i1 << ' ' << j1;
        ss2 << i2 << ' ' << j2;
        string var1(ss1.str());
        string var2(ss2.str());
        constraints->addConstraint(var1.c_str() , var2.c_str() , -1 * (D(i1, j1) + A(i1, j1)->flex));
    }
}

void addLimits(Constraints* constraints) {
    // set upper limits to the variables
    for(int i = 0; i < (int) tmsp->trains.size(); i++) {
        if(tmsp->trains[i] != NULL) {
            vector<activity* > activities = tmsp->trains[i]->activities;
            for(int k = 0; k < (int) activities.size(); k++) {
                if(!activities[k]) continue;
                stringstream ss1;
                ss1 << activities[k]->i << ' ' << activities[k]->j;
                string var(ss1.str());

                constraints->setUpperLimit(var.c_str(), activities[k]->lst);
                constraints->setLowerLimit(var.c_str(), activities[k]->est);

                constraints->setLocked(var.c_str(), activities[k]->est == activities[k]->lst);
                constraints->setFlex(var.c_str(), activities[k]->flex);
            }
        }
    }
}

void printSolution(map<string, int>* solution) {
    output("FLEX: ");
    output("%d ", getFlexibility());

    cdebug("Total Instance Flexibility: %d\n", getFlexibility());
    map<string, int>::iterator iter = solution->begin();
    while(iter != solution->end()) {
        output("%s %d ", iter->first.c_str(), iter->second);
        debug("[%s] = %d\n", iter->first.c_str(), iter->second);
        ++iter;
    }
	output("-1\n");
}

int flexibility() {
    Constraints constraints;
    addConstraints(&constraints);
    addLimits(&constraints);
    map<string, int> solution;
    solution = useClpToSolve(&constraints);
    debug(LINE, 0);
    printSolution(&solution);
    debug(LINE, 0);
    return 1;
}
