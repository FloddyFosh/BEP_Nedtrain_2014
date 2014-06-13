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
#include "output.h"
#include "flexibility.h"
#include "chaining.h"

int merge_bandwidth = -1;

extern void print_hele_state();
extern void handle_neg_cyc();

int solve() {
    cdebug("\nConstructing STJN.\n");
    timing_start("STJN");
    int stjn_consistent = stjn_construct();
    solverProgress(10);
    timing_stop("STJN");
    if (!stjn_consistent) {
        cdebug("Problem inconsistent. Aborting!\n");
        handle_neg_cyc();
        solverProgress(100);
        return 0;
    }
    cdebug("Constructing STJN Done.\n");

    cdebug("\nRunning ESTA+ algorithm.\n");
    timing_start("ESTA+");
    if (esta_plus()) {
        solverProgress(50);
        timing_stop("ESTA+");
    } else {
        timing_stop("ESTA+");
        cdebug("Could not find valid schedule. Aborting!\n");
        solverProgress(100);
        return 0;
    }
    cdebug("ESTA+ algorithm Done.\n");

    cdebug("\nRunning Chaining algorithm.\n");
    timing_start("Chaining");
    if(chaining()){
        solverProgress(75);
        timing_stop("Chaining");
    } else {
        timing_stop("Chaining");
        cdebug("Could not find valid schedule. Aborting!\n");
        solverProgress(100);
        return 0;
    }
    cdebug("Chaining algorithm Done.\n");

    cdebug("\nConstructing flexibility intervals using Linear Programming solver.\n");
    timing_start("LP");
    if(flexibility()){
        solverProgress(90);
        timing_stop("LP");
    } else {
        timing_stop("LP");
        cdebug("Could not find valid schedule. Aborting!\n");
        solverProgress(100);
        return 0;
    }
    cdebug("Constructing flexibility intervals Done.\n");

    solverProgress(100);
    return 1;
}

int add_mutexes = 0;
int main(int argc, char *argv[]) {
    int c;

    timing_start("Total");

    while((c = getopt(argc, argv, "m:x")) != -1) {
        switch(c){
            case 'm':
                merge_bandwidth = atoi(optarg);
                break;
            case 'x':
                add_mutexes = 1;
                break;
            case '?':
                if(optopt == 'm') {
                    cdebug("Option -%c requires an argument.\n", optopt);
                }
                else if(isprint(optopt)) {
                    cdebug("Unknown option `-%c'.\n", optopt);
                }
                else {
                    cdebug("Unknown option character `\\x%x'.\n", optopt);
                }
                return 1;
            default:
                return 1;
        }
    }

    cdebug("Parsing Instance.\n");

    // parser
    timing_start("Parsing");
    int readFromFile = 0;
    if(readFromFile){
    // open a file handle to a particular file:
        const char* filepath = "../../instances/demo.instance";
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
        cdebug("Parsing failed. Aborting!\n");
    }
    timing_stop("Parsing");

    // only one activity per train at any time
    if(add_mutexes)
        add_train_mutexes();

    // print error summary
    if (error_counter > 0) {
        cdebug("\n%d error%s\n\n", error_counter, error_counter == 1 ? "" : "s");
        return -1;
    }

    cdebug("Parsing Done.\n");

    int solved = solve();

    timing_stop("Total");

    cdebug("\nInstance %ssolved.\n", (solved ? "" : "not "));
    output("Instance %ssolved.\n", (solved ? "" : "not "));

    timing_print_summary();

    return !solved;
}
