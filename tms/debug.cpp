#include "alles.h"

#include "debug.h"

void _debug(char* format, ...) {
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

