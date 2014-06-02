#include "alles.h"

#include "timing.h"
#include "debug.h"

vector<timing_info *> timings;
struct timeval* tv_end = 0;

timing_info* new_timing_info(char* name) {
	timing_info* ti = new timing_info;
	ti->name = strdup(name);
	ti->start = new timeval;
	ti->total = 0;
	return ti;
}

void timing_init() {
	if (!tv_end) {
		tv_end = new timeval;
	}
}


timing_info* find_timing_info(char* name) {
	int i;
	for (i = 0; i < len(timings); i++) {
		timing_info* ti = (timing_info*) list_get(timings, i);
		if (!strcmp(name, ti->name)) {
			return ti;
		}
	}
	return 0;
}

timing_info* find_or_create_timing_info(char* name) {
	timing_info* ti = find_timing_info(name);
	if (!ti) {
		ti = new_timing_info(name);
		list_append(timings, ti);
	}
	return ti;
}

void timing_start(char* name) {
	timing_init();
	timing_info* ti = find_or_create_timing_info(name);
	gettimeofday(ti->start, NULL);
}

void timing_stop(char* name) {
	double seconds = 0;
	gettimeofday(tv_end, NULL);
	timing_info* ti = find_timing_info(name);

	int diff_sec = tv_end->tv_sec - ti->start->tv_sec;
	int diff_usec = tv_end->tv_usec - ti->start->tv_usec;

	seconds += diff_sec;
	seconds += diff_usec / (double) 1000000;
	
	ti->total = ti->total + seconds;
}

void timing_print_summary() {
	int i;
	for (i = 0; i < len(timings); i++) {
		timing_info* ti = ((timing_info*)list_get(timings, i));
		debug("%s: %lf\n", ti->name, ti->total);
	}
}

