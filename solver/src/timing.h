#ifndef __TIMING_H
#define __TIMING_H

#define solverProgress(x)	output("PROGRESS: %d\n", x)

typedef struct timing_info {
    string name;
	struct timeval* start;
	double total;
} timing_info;

timing_info* find_timing_info(string);
timing_info* find_or_create_timing_info(string);
void timing_start(string);
void timing_stop(string);
void timing_print_summary();

#endif

