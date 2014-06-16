#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

#include "exceptions.h" 

#include <map>
#include <vector>
#include <string>

using namespace std;

typedef struct Constraint {
    // [t1] - [t2] <= [c]
    int t1; // task id 1
    int t2; // task id 2
    int c;  // constant
} Constraint;

/*  List of Constraints

*/
class Constraints {
    private:
        map<string, int> variablesId;
        map<int, string> variablesName;
        map<int, int> upperLimits;
        map<int, int> lowerLimits;
        map<int, bool> locks;
        vector<Constraint> constraintVector;

    public:
        Constraints();
        void addConstraint(string name1, string name2, int c);
        int getAmountOfVariables();
        int size();
        Constraint operator [](int i) const;
        string getVariableName(int id);
        int addVariable(string name);
        int getUpperLimit(int id);
        void setUpperLimit(string name, int upperLimit);
        int getLowerLimit(int id);
        void setLowerLimit(string name, int lowerLimit);
        bool getLocked(int id);
        void setLocked(string name, bool locked);
};

#endif
