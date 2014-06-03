/*  Find flexibilty of an STP in two steps:
    1) Find min
    2) Find an interval set I_S which maximizes flex(S,I_S)

    earliest starting time  [est] --> t^-
    latest starting time    [lst] --> t^+

1)  maximize:       min
    constraints:1)  0 <= [lst] - [est] - [min] <= \infty \forall t
                2)  [lst] - [est'] <= c \forall (t - t' <= c) \in C
                3)  [lst_0] = 0
                3)  [est_0] = 0
        (so 3 types of constraints)

2)  maximize:       the sum of ([lst] - [est]) for each t \in T
    constraints:1)  [min] <= [lst] - [est] <= \infty \forall t
                2)  [lst] - [est'] <= c \forall (t - t' <= c) \in C
                3)  [lst_0] = 0
                3)  [est_0] = 0

    Find all [est] and [lst]
    
    Only difference with the two steps is the objective function
    and the first constraint.
*/

#include <sstream>

#include "tmsp.h"
#include "chaining.h"
#include "flexibility.h"

using namespace std;

pair<double, map<string, double> > useClpToSolve (Constraints constraints) {
    int n_cols = constraints.getAmountOfVariables() * 2;    
    ClpSimplex model; // child of ClpModel
    model.setOptimizationDirection(-1); // maximize instead of min.
    model.resize(0, n_cols + 1); // the last variable is [min]
    // model.setLogLevel(0); // turns off all output of Clp
    
    // -DBL_MAX = -inf en DBL_MAX = +inf
    // set coefficients
    for(int i = 0; i< n_cols; i++) {
        // set coefficients alternating between -1 and 1
        model.setObjectiveCoefficient(i, 0.0);
        model.setColumnLower(i, 0);       
        model.setColumnUpper(i, constraints.getUpperLimit(i/2)); 
    }
    model.setObjectiveCoefficient(n_cols, 1.0);
    
    // add constraints: 0 <= [lst] - [est] - [min] <= \infty \forall t
    for(int i = 0; i < n_cols; i+=2) {
        // latest starting time is [i]
        // earliest starting time is [i+1]
        int cols[] = {i, i+1, n_cols};
        double cfc[] = {1.0, -1.0, -1.0}; // coefficients
        model.addRow(3, cols, cfc, 0.0, DBL_MAX);
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

    // solve the problem for step 1
    cout << "step 1" << endl;
    model.initialSolve();
    double min = model.objectiveValue();
    cout << "min = " << min << endl;

    cout << "step 2" << endl;
    // change the objective functions
    int* deleteWhich = new int[n_cols];
    for(int i = 0; i < n_cols; i++) {
        model.setObjectiveCoefficient(i, ((i + 1) % 2) * 2.0 - 1.0);
        deleteWhich[i] = i;
    }
    model.setObjectiveCoefficient(n_cols, 0.0);
    cout << "hoi" << endl;
    // change the constraints of type 1
    // remove constraints of type 1
    // add constraints: 0 <= [lst] - [est] - [min] <= \infty \forall t
    
    for(int i = 0; i < n_cols; i+=2) {
        // latest starting time is [i]
        // earliest starting time is [i+1]
        int cols[] = {i, i+1};
        double cfc[] = {1.0, -1.0}; // coefficients
        model.addRow(2, cols, cfc, min, DBL_MAX);
    }
    model.deleteRows(n_cols, deleteWhich);

    // solve the problem for step 2
    model.initialSolve();

    // get solution
    const double* sol = model.primalColumnSolution();

    // return solution
    map<string, double> vars;
    double flexibility = 0.0;
    for(int i = 0; i < n_cols; i+=2) {
        string varname = constraints.getVariableName(i/2);
        vars[varname + "^+"] = sol[i];
        vars[varname + "^-"] = sol[i+1];
        flexibility += sol[i] - sol[i+1];
    }

    pair<double, map<string, double> > output;
    output.first = flexibility;
    output.second = vars;

    delete deleteWhich;
    return output;
}

int flexibility() {
    Constraints constraints;

    // a^+ - b^- <= -d_a
    for(int i = 0; i < (int) tmsp->precedences.size(); i++) {
        int i1 = P(i)->i1; // a^+
        int j1 = P(i)->j1; // a^+
        int i2 = P(i)->i2; // b^-
        int j2 = P(i)->j2; // b^-
        stringstream ss1; 
        stringstream ss2; 
        ss1 << i1 << '-' << j1;
        ss2 << i2 << '-' << j2;
        string var1(ss1.str());
        string var2(ss2.str());
        constraints.addConstraint(var1.c_str() , var2.c_str() , -1 * D(i1, j1));
    }

    // set upper limits to the variables
    for(int i = 0; i < (int) tmsp->trains.size(); i++) {
        int due = tmsp->trains[i]->due_date;
        vector<activity* > activities = tmsp->trains[i]->activities;
        for(int k = 0; k < (int) activities.size(); k++) {
            stringstream ss1;
            ss1 << activities[k]->i << '-' << activities[k]->j;
            string var1(ss1.str());
            constraints.setUpperLimit(var1.c_str(), due - D(activities[k]->i, activities[k]->j));
        }
    }

    pair<double, map<string, double> > output;
    output = useClpToSolve(constraints);
    
    cout << endl << "### flexibility (start) ###" << endl;
    cout << "flexibility = " << output.first << endl;
    map<string, double>::iterator iter = output.second.begin();
    while(iter != output.second.end()) {
        cout << iter->first << " = " << iter->second << endl;
        iter++;
    }
    cout << "### flexibility (end) ###" << endl << endl;
	return 1;
}
