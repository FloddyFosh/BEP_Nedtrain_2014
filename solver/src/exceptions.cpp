#include "exceptions.h"

#include <stdio.h>

NoChainFoundException::NoChainFoundException(){
    msg = "No chain could be found.";
}

void NoChainFoundException::showErrorMessage() {
    fprintf(stderr,"%s", msg.c_str());
}
