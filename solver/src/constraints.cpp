#include "constraints.h"

#include <assert.h>  

Constraints::Constraints() {
}

int Constraints::addVariable(string name) {
    if (variablesId.count(name)) {
        return variablesId[name];
    } else {
        int id = getAmountOfVariables();
        variablesId[name] = id;
        variablesName[id] = name;
        return id;
    }
}

void Constraints::addConstraint(string name1, string name2, int c) {
    int t1 = addVariable(name1);
    int t2 = addVariable(name2);
    Constraint constrain = {t1, t2, c};
    constraintVector.push_back(constrain);
}

int Constraints::getAmountOfVariables() {
    assert(variablesId.size() == variablesName.size());
    return (int) variablesId.size();
}

int Constraints::size() {
    return (int) constraintVector.size();
}

Constraint Constraints::operator [](int i) const {
    return constraintVector[i];
}

string Constraints::getVariableName(int id) {
    if(variablesName.count(id)) {
        return variablesName[id];
    }
    throw NoSuchVariableException(id);   
}

int Constraints::getUpperLimit(int id) {
    if(variablesName.count(id)) {
        return upperLimits[id];
    }
    throw NoSuchVariableException(id); 
}

void Constraints::setUpperLimit(string name, int upperLimit) {
    int id = addVariable(name);
    upperLimits[id] = upperLimit;
}

int Constraints::getLowerLimit(int id) {
    if(variablesName.count(id)) {
        return lowerLimits[id];
    }
    throw NoSuchVariableException(id);
}

void Constraints::setLowerLimit(string name, int lowerLimit) {
    int id = addVariable(name);
    lowerLimits[id] = lowerLimit;
}

bool Constraints::getLocked(int id) {
    if(locks.count(id)) {
        return locks[id];
    }
    throw NoSuchVariableException(id);
}

void Constraints::setLocked(string name, bool locked) {
    int id = addVariable(name);
    locks[id] = locked;
}
