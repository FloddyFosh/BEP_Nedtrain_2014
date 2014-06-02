#ifndef __TIMING_H
#define __TIMING_H

#define progress(x)	fprintf(stdout, "PROGRESS: %d\n", x)

typedef struct timing_info {
	char* name;
	struct timeval* start;
	double total;
} timing_info;

timing_info* find_timing_info(char*);
timing_info* find_or_create_timing_info(char*);
void timing_start(char*);
void timing_stop(char*);
void timing_print_summary();

#endif

