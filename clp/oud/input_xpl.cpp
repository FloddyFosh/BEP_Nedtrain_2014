#include "ClpSimplex.hpp"
#include "CoinBuild.hpp"
#include "CoinModel.hpp"

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main(int argc, const char *argv[]) {
    ClpSimplex model; // child of ClpModel

    // reading n_cols
    int n_cols;
    cin >> n_cols;

    // resize
    model.resize(0, n_cols);

    // reading objective coefficients
    // reading upper- and lowerbounds
    vector<double> lower(n_cols);
    vector<double> objec(n_cols);
    vector<double> upper(n_cols);
    for (int i = 0; i < n_cols; i++) {
        cin >> lower[i] >> objec[i] >> upper[i];
    }

    // set objective coefficients
    // set upper- and lowerbounds
    for(int i = 0; i < n_cols; i++) {
        model.setObjectiveCoefficient(i, objec[i]);
        model.setColumnLower(i, lower[i]);
        model.setColumnUpper(i, upper[i]);
    }    
    
    // reading number of rows
    int n_rows;
    cin >> n_rows;

    // setting col_indx
    vector<int> col_indx(n_cols);
    for (int i = 0; i < n_cols; i++) {
        col_indx[i] = i;
    }
    
    // add constraints to the model
    for (int i = 0; i < n_rows; i++) {
        // make variables for addRow(...)
        double low_bounds;
        vector<double> constraints(n_cols);
        double upr_bounds;

        // reading lowerbound
        // reading coefficients of the constraint
        // reading upperbound
        cin >> low_bounds;
        for(int k = 0; k < n_cols; k++) {
            double temp;
            cin >> temp;
            constraints[k] = temp;
        }
        cin >> upr_bounds;

        // add the constraint to the model
        model.addRow(n_cols, &col_indx[0], &constraints[0], low_bounds, upr_bounds);
    }
    
    // solve
    model.initialSolve();
    
    // get solution
    int numberCols = model.numberColumns(); // equal to n_cols
    const double* sol1 = model.primalColumnSolution();

    // print solution
    cout << endl << "### SOLUTION ###" << endl;
    for(int i = 0; i < numberCols; i++) {
        cout << "x_" << (i+1) << " = " << sol1[i] << endl;
    }
    cout << endl;

    // some checks
    if(model.isAbandoned())
        cout << "Numerical problems found" << endl;
    if(model.isProvenPrimalInfeasible())
        cout << "Primal Infeasible" << endl;
    if(model.isProvenDualInfeasible())
        cout << "Dual Infeasible" << endl;
    return 0;
}
