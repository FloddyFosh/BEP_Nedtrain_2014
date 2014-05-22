#ifndef __DEBUG_H
#define __DEBUG_H

void _debug(char* format, ...);

#ifdef DEBUG
#define debug(format, ...) _debug(__FILE__ ":%s: " format, __func__, ##__VA_ARGS__)
#else
#define debug(format, ...)
#endif

#endif
