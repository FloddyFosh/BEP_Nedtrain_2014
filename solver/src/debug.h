#ifndef __DEBUG_H
#define __DEBUG_H

#define LINE "-----------------------------------------------------------------------------------------------------\n"
void cdebug(string format, ...);
void debug(string format, ...);
void output(string format, ...);
void printToFile(std::ofstream fstream, string format, ...);

#endif
