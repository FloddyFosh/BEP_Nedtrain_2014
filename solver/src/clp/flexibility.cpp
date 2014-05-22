/*  Find flexibilty of an STP:
    1) Find an interval set I_S of S
    2) Find an interval set I_S which maximizes flex(S,I_S)

    earliest starting time  [est] --> t^-
    latest starting time    [lst] --> t^-

    maximize:       the sum of ([lst] - [est]) for each t \in T
    constraints:1)  0 <= [lst] - [est] <= \infty \forall t
                2)  [lst] - t' <= c \forall (t - t' <= c) \in C
                3)  [lst_0] = 0
                3)  [est_0] = 0
        (so 3 types of constraints)

    Find all [est] and [lst]
    
*/

#include "ClpSimplex.hpp"

#include <set>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

typedef struct Constraint {
    // [t1] - [t2] <= [c]
    int t1; // task id 1
    int t2; // task id 2
    int c;  // constant
} Constraint;

int main () {
    int n_constraints;
    cin >> n_constraints; 
        
    // TODO: Have to add something to read the first task,
    //       because we need to know what [lst_0] and [est_0]
    //       is. 

    // now read [n_constraints] constraints
    vector< Constraint > constraints;

    map<int, string> taskIdMap; 
    map<string, int> taskNameMap;
    
int counter = 0;
    for(int i = 0; i < n_constraints; i++) {
        // [ti] - [tj] <= [c]
        string ti, tj;
        int c;
        cin >> ti >> tj >> c;

        
        Constraint constr = {ti, tj, c};
        constraints.push_back(constr);
    }

    // print the input
    cout << endl;
    for(int i = 0; i < n_constraints; i++) {
        Constraint t = constraints[i];
        cout << t.t1 << "\t" << t.t2 << "\t" << t.c << endl;
    }
    cout << variables.size() << " tasks" << endl;
    
    // make a model with [n_cols] variables to optimize
    // maximize {1, -1, 1, -1, ...}.size = [n_cols]
    // minimize {-1, 1, -1, 1, ...}.size = [n_cols]
    // each variable in range [-inf, inf]
    int n_cols = ((int) variables.size()) * 2;
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
        cols.push_back(i * 2);
        cfc.push_back(1.0);
        cols.push_back(i * 2 + 1);
        cfc.push_back(-1.0);
        model.addRow(2, &cols[0], &cfc[0], 0.0, DBL_MAX);
    }
    
    // add constraints: [lst] - t' <= c \forall (t - t' <= c) \in C
    // TODO: model.addRow(n_cols, cols, coefficients, low_b, up_b);

    // TODO: model.initialSolve();
    
    // TODO: get solution
/*  int numberCols = model.numberColumns();
    const double* sol1 = model.primalColumnSolution();
*/
    // TODO: print solution
    cout << endl << "### SOLUTION ###" << endl;
/*  cout << "status     = " << model.status() << endl;
    for(int i = 0; i < numberCols; i++) {
        cout << "x_" << (i+1) << " = " << sol1[i] << endl;
    }
*/
    cout << endl;

    return 0;
}
