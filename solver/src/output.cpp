#include "alles.h"
#include "output.h"

#include <fstream>

void cdebug(string format, ...) {
    va_list args;
    va_start(args, format);
    char err[8096];
    vsprintf(err, format.c_str(), args);
    cerr << err;
    va_end(args);
}

void _debug(string format, ...) {
    va_list args;

    va_start(args, format);
    vfprintf(stderr, format.c_str(), args);
    va_end(args);
}

void output(string format, ...){
    va_list args;
    va_start(args,format);
    char out[8096];
    vsprintf(out,format.c_str(),args);
    cout << out;
    va_end(args);
}

