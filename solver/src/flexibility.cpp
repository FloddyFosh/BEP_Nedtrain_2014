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

using namespace std;

int minflex = 0, flextotaal = 0;

int getFlexibility(){
    return flextotaal;
}

int getMinFlex(){
    return minflex;
}

void setObjective(ClpSimplex* model, int n_cols, Constraints* constraints) {
    // -DBL_MAX = -inf and DBL_MAX = +inf
    // set coefficients
    for(int i = 0; i < n_cols; i++) {
        model->setObjectiveCoefficient(i, 0.0);
        model->setColumnLower(i, constraints->getLowerLimit(i/2));
        model->setColumnUpper(i, constraints->getUpperLimit(i/2)); 
    }
    model->setObjectiveCoefficient(n_cols, 1.0);
    model->setColumnLower(n_cols, 0);
    model->setColumnUpper(n_cols, DBL_MAX);
}

void addType1Constraints(ClpSimplex* model, int n_cols) {
    // add constraints: 0 <= [lst] - [est] - [minflex] <= \infty \forall t
    for(int i = 0; i < n_cols; i+=2) {
        // latest starting time is [i]
        // earliest starting time is [i+1]
        int cols[] = {i, i+1, n_cols};
        double cfc[] = {1.0, -1.0, -1.0}; // coefficients
        model->addRow(3, cols, cfc, 0.0, DBL_MAX);
    }
}

void addType2Constraints(ClpSimplex* model, Constraints* constraints) {
    // add constraints: [lst] - [est'] <= c \forall (t - t' <= c) \in C
    for(int i = 0; i < constraints->size(); i++) {
        // [lst]  = constrain.t1 * 2    
        // [est'] = constrain.t2 * 2 + 1
        Constraint constrain = (*constraints)[i];
        int cols[] = {constrain.t1 * 2, constrain.t2 * 2 + 1};
        double cfc[] = {1.0, -1.0}; // coefficients
        model->addRow(2, cols, cfc, -DBL_MAX, constrain.c);
    }
}

void changeObjective(ClpSimplex* model, int n_cols) {
    // change the objective functions
    for(int i = 0; i < n_cols; i++) {
        // set coefficients alternating between -1 and 1
        model->setObjectiveCoefficient(i, ((i + 1) % 2) * 2.0 - 1.0);
    }
    model->setObjectiveCoefficient(n_cols, 0.0);
}

void changeType1Constraints(ClpSimplex* model, int n_cols) {
    // remove constraints of type 1
    int* deleteWhich = new int[n_cols];
    for(int i = 0; i < n_cols/2; i++) {
        deleteWhich[i] = i;
    }
    model->deleteRows(n_cols/2, deleteWhich);
    delete[] deleteWhich;

    // add constraints: 0 <= [lst] - [est] - [minflex] <= \infty \forall t
    for(int i = 0; i < n_cols; i+=2) {
        // latest starting time is [i]
        // earliest starting time is [i+1]
        int cols[] = {i, i+1};
        double cfc[] = {1.0, -1.0}; // coefficients
        model->addRow(2, cols, cfc, minflex, DBL_MAX);
    }
}

map<string, int> useClpToSolve (Constraints* constraints) {
    int n_cols = constraints->getAmountOfVariables() * 2;    
    ClpSimplex model; // child of ClpModel
    model.setOptimizationDirection(-1); // maximize instead of minflex.
    model.resize(0, n_cols + 1); // the last variable is [minflex]
    model.setLogLevel(0); // turns off all output of Clp
    
    setObjective(&model, n_cols, constraints);
    addType1Constraints(&model, n_cols);
    addType2Constraints(&model, constraints);
    
    // solve the problem for step 1
    model.initialSolve();
    minflex = (int) model.objectiveValue();

    changeObjective(&model, n_cols);
    changeType1Constraints(&model, n_cols);

    // solve the problem for step 2
    model.initialSolve();

    // get solution
    const double* sol = model.primalColumnSolution();
    flextotaal = (int) model.objectiveValue();

    // return solution
    map<string, int> output;   
    for(int i = 0; i < n_cols; i+=2) {
        string varname = constraints->getVariableName(i/2);
        output[varname + " +"] = (int) sol[i];
        output[varname + " -"] = (int) sol[i+1];
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
        constraints->addConstraint(var1.c_str() , var2.c_str() , -1 * D(i1, j1));
    }
}

void addLimits(Constraints* constraints) {
    // set upper limits to the variables
    for(int i = 0; i < (int) tmsp->trains.size(); i++) {
        if(tmsp->trains[i] != NULL) {
            int due = tmsp->trains[i]->due_date;
            int release = tmsp->trains[i]->release_date;
            vector<activity* > activities = tmsp->trains[i]->activities;
            for(int k = 0; k < (int) activities.size(); k++) {
                stringstream ss1;
                ss1 << activities[k]->i << ' ' << activities[k]->j;
                string var1(ss1.str());
                constraints->setUpperLimit(var1.c_str(), due - D(activities[k]->i, activities[k]->j));
                constraints->setLowerLimit(var1.c_str(), release);
            }
        }
    }
}

void printMSE(map<string, int>* solution) {
    double mse = 0.0;
    double mean = ((double) getFlexibility()) / (double) solution->size() * 2;
    map<string, int>::iterator iter = solution->begin();
    while(iter != solution->end()) {
        double lst = (double) iter->second;        
        ++iter;
        double est = (double) iter->second;
        ++iter;
        mse += (lst - est - mean) * (lst - est - mean);
    }
    cout << "MSE = " << mse << endl;
}

void printSolution(map<string, int>* solution) {
    output("FLEX: ");
    output("%d ", getMinFlex());
    output("%d ", getFlexibility());
    cout << "flexibility = " << getFlexibility() << endl;
    cout << "minflex = " << getMinFlex() << endl;
    cdebug("minflex = %d\n", getMinFlex());
    cdebug("flexibility = %d\n", getFlexibility());
    map<string, int>::iterator iter = solution->begin();
    while(iter != solution->end()) {
        output("%s %d ", iter->first.c_str(), iter->second);
        cdebug("[%s] = %d\n", iter->first.c_str(), iter->second);
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
    printSolution(&solution);
    printMSE(&solution);
    return 1;
}
