#include "exceptions.h"

#include <stdio.h>
#include "debug.h"

NoChainFoundException::NoChainFoundException(){
    msg = "No chain could be found.";
}

void NoChainFoundException::showErrorMessage() {
    debug("%s\n", msg.c_str());
}
