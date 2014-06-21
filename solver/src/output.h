#ifndef __DEBUG_H
#define __DEBUG_H

#include "alles.h"

#define LINE "------------------------------------------------\n"
void cdebug(string format, ...);
void _debug(string format, ...);
void output(string format, ...);

#ifdef DEBUG
#define debug(format, ...) _debug("[" __FILE__ ":%s] " format, __func__, ##__VA_ARGS__)
#else
#define debug(format, ...)
#endif

#endif
