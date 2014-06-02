#ifndef __DEBUG_H
#define __DEBUG_H

#define LINE "-----------------------------------------------------------------------------------------------------\n"
void _debug(char* format, ...);
void _output(char* format, ...);
void printToFile(std::ofstream fstream, char* format, ...);

#ifdef DEBUG
#define debug(format, ...) _debug("[" __FILE__ ":%s] " format, __func__, ##__VA_ARGS__)
#define cdebug(format, ...) _debug(format, ##__VA_ARGS__)
#define output(format, ...) _output(format, ##__VA_ARGS__)
#else
#define debug(format, ...)
#define cdebug(format, ...)
#define output(format, ...)
#endif

#endif
