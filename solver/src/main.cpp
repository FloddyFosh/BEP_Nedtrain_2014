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
#include "timing.h"
#include "debug.h"
#include "flexibility.h"
#include "chaining.h"

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
        progress(50);
        timing_stop("esta+");
    } else {
        timing_stop("esta+");
        debug("Could not find valid schedule.\n");
        progress(100);
        return 0;
    }

    //print_est_schedule();

    debug("Running chaining algorithm.\n");
    timing_start("chaining");
    if(chaining()){
        progress(75);
        timing_stop("chaining");
    } else {
        timing_stop("chaining");
        debug("Could not find valid schedule.\n");
        progress(100);
        return 0;
    }

    debug("Constructing flexibility intervals using Linear Programming solver.\n");
    timing_start("LP");
    if(flexibility()){
        progress(90);
        timing_stop("LP");
    } else {
        timing_stop("LP");
        debug("Could not find valid schedule.\n");
        progress(100);
        return 0;
    }

    progress(100);
    return 1;
}

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
                    debug("Option -%c requires an argument.\n", optopt);
                else if(isprint(optopt))
                    debug("Unknown option `-%c'.\n", optopt);
                else
                    debug("Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                return 1;
        }
    }

    debug("Parsing.\n");

    // parser
    timing_start("parsing");
    int readFromFile = 0;
    if(readFromFile==1){
    // open a file handle to a particular file:
        char* filepath = "../../instances/demo.instance";
        FILE *myfile = fopen(filepath, "r");
    	// make sure it's valid:
    	if (!myfile) {
            cout << "Could not find " << filepath << "." << endl;
    		return -1;
    	}
    	// set lex to read from it instead of defaulting to STDIN:
        yyin = myfile;
    }
    else{
        yyin = stdin;
    }
    if (yyparse() != 0) { // yyparse doet iets met bison grammar
        debug("Parsing failed. Aborting!\n");
    }
    timing_stop("parsing");

    // only one activity per train at any time
    if(add_mutexes)
        add_train_mutexes();

    // print error summary
    if (error_counter > 0) {
        debug("\n%d error%s\n\n", error_counter, error_counter == 1 ? "" : "s");
        return -1;
    }

    int solved = solve();

    timing_stop("total");

    fprintf(stdout, "Instance %ssolved.\n", (solved ? "" : "not "));

    timing_print_summary();

    return !solved;
}
