#include "alles.h"

#include "debug.h"

void debug(string format, ...) {
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format.c_str(), args);
	va_end(args);
}

