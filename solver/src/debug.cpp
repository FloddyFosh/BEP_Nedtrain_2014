#include "alles.h"
#include "debug.h"

#include <fstream>

void _debug(char* format, ...) {
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

void _output(char* format, ...){
    va_list args;
    va_start(args,format);
    vfprintf(stdout,format,args);
    va_end(args);
}

void printToFile(ofstream file, char* format, ...){
    va_list args;
    va_start(args,format);
    char out[256];
    vsprintf(out, format, args);
    file << out;
    va_end(args);
}

