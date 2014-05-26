/*  Find flexibilty of an STP:
    1) Find an interval set I_S of S
    2) Find an interval set I_S which maximizes flex(S,I_S)

    earliest starting time  [est] --> t^-
    latest starting time    [lst] --> t^+

    maximize:       the sum of ([lst] - [est]) for each t \in T
    constraints:1)  0 <= [lst] - [est] <= \infty \forall t
                2)  [lst] - [est'] <= c \forall (t - t' <= c) \in C
                3)  [lst_0] = 0
                3)  [est_0] = 0
        (so 3 types of constraints)

    Find all [est] and [lst]
    
*/

#include "flexibility.h"

using namespace std;

pair<double, map<string, double> > useClpToSolve (Constraints constraints) {
    int n_cols = constraints.getAmountOfVariables() * 2;    
    ClpSimplex model; // child of ClpModel
    model.setOptimizationDirection(-1); // maximize instead of min.
    model.resize(0, n_cols);
    model.setLogLevel(0); // turns off all output of Clp

    // set coefficients
    for(int i = 0; i< n_cols; i++) {
        // set coefficients alternating between -1 and 1
        model.setObjectiveCoefficient(i, ((i + 1) % 2) * 2.0 - 1.0);
        model.setColumnLower(i, 0); // -DBL_MAX = -inf
        model.setColumnUpper(i, DBL_MAX); //  DBL_MAX = +inf
    }
    
    // add constraints: 0 <= [lst] - [est] <= \infty \forall t
    for(int i = 0; i < n_cols; i+=2) {
        // latest starting time is [i]
        // earliest starting time is [i+1]
        int cols[] = {i, i+1};
        double cfc[] = {1.0, -1.0}; // coefficients
        model.addRow(2, cols, cfc, 0.0, DBL_MAX);
    }
    
    // add constraints: [lst] - [est'] <= c \forall (t - t' <= c) \in C
    for(int i = 0; i < constraints.size(); i++) {
        // [lst]  = constrain.t1 * 2    
        // [est'] = constrain.t2 * 2 + 1
        Constraint constrain = constraints[i];
        int cols[] = {constrain.t1 * 2, constrain.t2 * 2 + 1};
        double cfc[] = {1.0, -1.0}; // coefficients
        model.addRow(2, cols, cfc, -DBL_MAX, constrain.c);
    }
    
    // add constraints: [lst_0] = 0 && [est_0] = 0
    model.setColumnLower(0, 0.0); 
    model.setColumnUpper(0, 0.0);    
    model.setColumnLower(1, 0.0); 
    model.setColumnUpper(1, 0.0); 

    // solve the problem
    model.initialSolve();
    
    // get solution
    int numberCols = model.numberColumns();
    assert(n_cols == numberCols);
    const double* sol = model.primalColumnSolution();

    // return solution
    map<string, double> vars;
    double flexibility = 0.0;
    for(int i = 0; i < numberCols; i+=2) {
        string varname = constraints.getVariableName(i/2);
        vars[varname + "^+"] = sol[i];
        vars[varname + "^-"] = sol[i+1];
        flexibility += sol[i] - sol[i+1];
    }

    pair<double, map<string, double> > output;
    output.first = flexibility;
    output.second = vars;
    return output;
}
