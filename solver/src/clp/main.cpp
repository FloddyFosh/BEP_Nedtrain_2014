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

#include "constraints.h"
#include "flexibility.h"

#include <iostream>

using namespace std;

int main () {
    int n_constraints;
    cin >> n_constraints; 
        
    Constraints constraints;
    
    // read the variable name for task 0
    string task0;
    cin >> task0;
    constraints.addVariable(task0);
   
    // read the constraints
    for(int i = 0; i < n_constraints; i++) {
        // [ti] - [tj] <= [c]
        string task1, task2;
        int c;
        cin >> task1 >> task2 >> c;
        constraints.addConstraint(task1, task2, c);
    }
    cout << endl;
    cout << constraints.size() << " constraints" << endl;
    cout << constraints.getAmountOfVariables() << " tasks" << endl;
    cout << endl;

    pair<double, map<string, double> > output = useClpToSolve(constraints);
    map<string, double> vars = output.second;

    // print solution
    cout << endl << "### SOLUTION ###" << endl;
    map<string, double>::iterator iter = vars.begin();
    while(iter != vars.end()) {
        cout << iter->first << " = " << iter->second << endl;
        iter++;
    }
    cout << "flexibility = " << output.first << endl;
    cout << endl;
    return 0;
}
