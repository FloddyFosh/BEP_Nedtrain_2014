/*  Find flexibilty of an STP:
    1) Find an interval set I_S of S
    2) Find an interval set I_S which maximizes flex(S,I_S)

    earliest starting time  [est] --> t^-
    latest starting time    [lst] --> t^-

    maximize:       the sum of ([lst] - [est]) for each t \in T
    constraints:    0 <= [lst] - [est] <= \infty \forall t
                    [lst] - t' <= c \forall (t - t' <= c) \in C
                    [lst_0] = 0
                    [est_0] = 0
    
    Find [est] and [lst]
    
*/

#include "ClpSimplex.hpp"

#include <set>
#include <vector>
#include <iostream>

using namespace std;

typedef struct Constraint {
    // [t1] - [t2] <= [c]
    int t1; // task 1
    int t2; // task 2
    int c;  // constant
} Constraint;

int main () {
    int n_constraints;
    cin >> n_constraints; 
    
    // now read [n_constraints] constraints
    vector< Constraint > constraints;
    set<int> variables;
    for(int i = 0; i < n_constraints; i++) {
        // [ti] - [tj] <= [c]
        int ti, tj, c;
        cin >> ti >> tj >> c;
        variables.insert(ti);
        variables.insert(tj);
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
    int n_cols = variables.size() * 2;
    ClpSimplex model; // child of ClpModel
    model.resize(0, n_cols);

    // set coefficients
    for(int i = 0; i < n_cols; i++) {
        // set coefficients alternating between -1 and 1
        model.setObjectiveCoefficient(i, (i % 2) * 2.0 - 1.0);
        model.setColumnLower(i, -DBL_MAX); // -inf
        model.setColumnUpper(i,  DBL_MAX); // +inf
    } 
    
    return 0;
}
