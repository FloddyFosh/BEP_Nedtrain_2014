/* First try to use the Clp library. I use the following example: 
    
    # short example
     maximize obj: 0.6 * x1 + 0.5 * x2;
     s.t. c_1:     x1 + 2 * x2 <= 1;
     s.t. c_2: 3 * x1 +     x2 <= 2;

    # solution
     x_1 = 0.6 
     x_2 = 0.2
*/

#include "ClpSimplex.hpp"
#include "CoinBuild.hpp"
#include "CoinModel.hpp"

#include <iostream>

using namespace std;

int main(int argc, const char *argv[]) {
    ClpSimplex model; // child of ClpModel

    int n_cols = 2;
    model.resize(0,2);

    // define objective coefficients
    // define upper- and lowerbounds
    double objec[] = { -0.6, -0.5};
    double lower[] = {-10.0, -10.0};    
    double upper[] = { 10.0,  10.0};

    // set objective coefficients
    // set upper- and lowerbounds
    for(int i = 0; i < n_cols; i++) {
        model.setObjectiveCoefficient(i, objec[i]);
        model.setColumnLower(i, lower[i]);
        model.setColumnUpper(i, upper[i]);
    }    
    
    // define coefficients of the constraints
    int cols_1[] = {0, 1};
    int cols_2[] = {0, 1};        
    double c_1[] = {1.0, 2.0};
    double c_2[] = {3.0, 1.0};

    // set coefficients of the constraints
    // (wss in row = 0 de objective)
    model.addRow(2, cols_1, c_1, -100.0, 1.0);
    model.addRow(2, cols_2, c_2, -100.0, 2.0);

    // solve
    model.initialSolve();
    
    // get solution
    int numberCols = model.numberColumns();
    const double* sol1 = model.primalColumnSolution();

    // print solution
    cout << endl << "### SOLUTION ###" << endl;
    cout << "status     = " << model.status() << endl;
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