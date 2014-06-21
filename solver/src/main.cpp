#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#include "token.h"
#include "grammar.tab.hpp"

#include "timing.h"
#include "output.h"
#include "solve.h"
#include "tmsp.h"

int main(int argc, char *argv[]) {
    int c;
    int merge_bandwidth = -1;
    int add_mutexes = 0;

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
        const char* filepath = "../../instances/small.instance";
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

    int solved = solve(merge_bandwidth, add_mutexes);

    timing_stop("Total");

    cdebug("\nInstance %ssolved.\n", (solved ? "" : "not "));
    output("Instance %ssolved.\n", (solved ? "" : "not "));

    timing_print_summary();

    return !solved;
}
