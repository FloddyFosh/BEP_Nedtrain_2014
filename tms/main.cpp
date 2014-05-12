#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#include "token.h"
#include "grammar.tab.hpp"

#include "tmsp.h"
#include "stjn.h"
#include "esta_plus.h"
//#include "chaining.h"
#include "timing.h"
#include "debug.h"

int merge_bandwidth = -1;

extern void print_hele_state();
extern void handle_neg_cyc();

int solve() {
	debug("Constructing STJN.\n");
	timing_start("stjn");
	int stjn_consistent = stjn_construct();
	progress(10);
	timing_stop("stjn");
	if (!stjn_consistent) {
		debug("Problem inconsistent.\n");
		handle_neg_cyc();
		progress(100);
		return 0;
	}

	debug("Running ESTA+ algorithm.\n");
	timing_start("esta+");
	if (esta_plus()) {
		progress(75);
		timing_stop("esta+");
		/*if (is_est_valid()) {
			debug("Found valid schedule.\n");
			debug("Running CHAINING algorithm.\n");
			timing_start("chaining");
			if (!chaining_algorithm()) {
				timing_stop("chaining");
				debug("Chaining failed!\n");
				progress(100);
				return 0;
			} else {
				timing_stop("chaining");
				debug("Chaining succeeded.\n");
				print_est_schedule();
				fflush(stdout);
				progress(100);
				return 1;
			}
		} else {
			debug("ESTA succeeded but cannot find valid schedule!\n");
			progress(100);
			return 0;
		}*/
                print_est_schedule();
                fflush(stdout);
                progress(100);
                return 1;
	} else {
		timing_stop("esta+");
		printf("Could not find valid schedule.\n");
		progress(100);
		return 0;
	}

}

/*
// calculate throughput as sum of EET of entire train
int throughput() {
	int i,j,tp = 0;

	for (i = 0; i < tmsp->n_trains; i++) if (T(i)) {
		if (N(i) > 0) {
			int latest_end_time = 0;
			int earliest_start_time = inf;
            for (j = 0; j < N(i); j++) if (A(i,j)) {
				if (EST(i,j) < earliest_start_time) {
					earliest_start_time = EST(i,j);
				}
				if (EET(i,j) > latest_end_time) {
					latest_end_time = EET(i,j);
				}
			}
			tp += latest_end_time - earliest_start_time;
		}
	}
	return tp;
}

double robustness() {
	return 0.0;
}

void print_one_line_summary() {
	fprintf(stderr, "%lf ", find_timing_info("parsing")->total);
	fprintf(stderr, "%lf ", find_or_create_timing_info("esta+")->total);
	fprintf(stderr, "%lf ", find_or_create_timing_info("chaining")->total);
	fprintf(stderr, "%d ", leveling_constraints_before_chaining);
	fprintf(stderr, "%d ", leveling_constraints_after_chaining);
	fprintf(stderr, "%lf ", robustness());
	fprintf(stderr, "%d\n", throughput());
}*/

int add_mutexes = 0;
int main(int argc, char *argv[]) {
    int c;
	
	timing_start("total");

    while((c = getopt(argc, argv, "m:x")) != -1) {
        switch(c){
            case 'm':
                merge_bandwidth = atoi(optarg);
                break;
            case 'x':
                add_mutexes = 1;
                break;
            case '?':
                if(optopt == 'm')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if(isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                return 1;
        }
    }

	debug("Parsing.\n");

	// parser
	timing_start("parsing");
	yyin = stdin;
	if (yyparse() != 0) {
		fprintf(stderr, "Parsing failed. Aborting!\n");
	}
	timing_stop("parsing");

	// only one activity per train at any time
    if(add_mutexes)
	    add_train_mutexes();

	// print error summary
	if (error_counter > 0) {
		fprintf(stderr, "\n%d error%s\n\n", error_counter, error_counter == 1 ? "" : "s");
		return -1;
	}
	
	int solved = solve();

	timing_stop("total");

	fprintf(stderr, "Instance %ssolved.\n", (solved ? "" : "not "));
	//print_one_line_summary();

	timing_print_summary();
	/*fprintf(stderr, "leveling constraints before chaining: %d\n", leveling_constraints_before_chaining);
	fprintf(stderr, "leveling constraints after chaining: %d\n", leveling_constraints_after_chaining);
	fprintf(stderr, "robustness: %lf\n", robustness());
	fprintf(stderr, "throughput: %d\n", throughput());*/

	return !solved;
}


