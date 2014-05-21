/*  Find flexibilty of an STP:
    1) Find an interval set I_S of S
    2) Find an interval set I_S which maximizes flex(S,I_S)

    earliest starting time  [est] --> t^-
    latest starting time    [lst] --> t^-

    maximize:       the sum of ([lst] - [est]) for each t \in T
    constraints:1)  0 <= [lst] - [est] <= \infty \forall t
                2)  [lst] - [est'] <= c \forall (t - t' <= c) \in C
                3)  [lst_0] = 0
                3)  [est_0] = 0
        (so 3 types of constraints)

    Find all [est] and [lst]
    
*/

#include "ClpSimplex.hpp"
#include "constraints.h"

#include <set>
#include <vector>
#include <string>
#include <iostream>

using namespace std;


int main () {
    int n_constraints;
    cin >> n_constraints; 
        
    // TODO: Have to add something to read the first task,
    //       because we need to know what [lst_0] and [est_0]
    //       is.
    //       Now take the first variable mentioned

    // now read [n_constraints] constraints
    Constraints constraints;

    for(int i = 0; i < n_constraints; i++) {
        // [ti] - [tj] <= [c]
        string task1, task2;
        int c;
        cin >> task1 >> task2 >> c;
        constraints.addConstraint(task1, task2, c);
    }

    // print the input
    cout << endl;
    for(int i = 0; i < n_constraints; i++) {
        Constraint t = constraints[i];
        cout << t.t1 << "\t" << t.t2 << "\t" << t.c << endl;
    }
    
    cout << constraints.size() << " constraints" << endl;
    cout << constraints.getAmountOfVariables() << " tasks" << endl;
    
    // make a model with [n_cols] variables to optimize
    // LST, EST, LST, ..., EST
    // maximize {1, -1, 1, -1, ...}.size = [n_cols]
    // minimize {-1, 1, -1, 1, ...}.size = [n_cols]
    // each variable in range [-inf, inf]
    int n_cols = constraints.getAmountOfVariables() * 2;
    ClpSimplex model; // child of ClpModel
    model.resize(0, n_cols);

    // set coefficients
    for(int i = 0; i< n_cols; i++) {
        // set coefficients alternating between -1 and 1
        model.setObjectiveCoefficient(i, (i % 2) * 2.0 - 1.0);
        model.setColumnLower(i, -DBL_MAX); // -DBL_MAX = -inf
        model.setColumnUpper(i,  DBL_MAX); //  DBL_MAX = +inf
    }
    
    // add constraints: 0 <= [lst] - [est] <= \infty \forall t
    for(int i = 0; i < n_cols / 2; i++) {
        vector<int> cols;
        vector<double> cfc; // coefficients
        // latest starting time :
        cols.push_back(i * 2);     
        cfc.push_back(1.0);
        // earliest starting time:
        cols.push_back(i * 2 + 1);
        cfc.push_back(-1.0);
        
        model.addRow(2, &cols[0], &cfc[0], 0.0, DBL_MAX);
    }
    
    // add constraints: [lst] - t' <= c \forall (t - t' <= c) \in C
    for(int i = 0; i < constraints.size(); i++) {
        Constraint constrain = constraints[i];
        vector<int> cols;
        vector<double> cfc;
        // latest starting time :
        cols.push_back(constrain.t1 * 2);     
        cfc.push_back(1.0);
        // earliest starting time:
        cols.push_back(constrain.t2 * 2 + 1);
        cfc.push_back(-1.0);
        model.addRow(2, &cols[0], &cfc[0], -DBL_MAX, constrain.c);
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
    const double* sol = model.primalColumnSolution();

    // print solution
    cout << endl << "### SOLUTION ###" << endl;
    cout << "status     = " << model.status() << endl;
    for(int i = 0; i < numberCols / 2; i++) {
        cout << constraints.getVariableName(i) << "^+ = " << sol[i] << endl;
        cout << constraints.getVariableName(i+1) << "^- = " << sol[i+1] << endl;
    }
    cout << endl;

    return 0;
}
