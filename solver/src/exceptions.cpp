#include "exceptions.h"

#include <stdio.h>
#include "output.h"

NoChainFoundException::NoChainFoundException() : msg("No chain could be found.") {}

void NoChainFoundException::showErrorMessage() {
    debug("%s\n", msg.c_str());
}

NoSuchVariableException::NoSuchVariableException(int id) {
    this->id = id;
}

void NoSuchVariableException::showErrorMessage() {
    debug("Variable id %d doesn't exist\n", id);
}
