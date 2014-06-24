#include "solve.h"
#include "tmsp.h"
#include "stjn.h"
#include "esta_plus.h"
#include "timing.h"
#include "output.h"
#include "flexibility.h"
#include "chaining.h"

extern void handle_neg_cyc();
int merge_bandwidth;

int solve(int merge_bw, int add_mutexes) {
    merge_bandwidth = merge_bw;

    if(!solveSTJN()) return 0;
    if(!solveESTA(add_mutexes)) return 0;
    if(!solveChaining()) return 0;
    if(!solveLP()) return 0;

    return 1;
}

int solveSTJN() {
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
    return 1;
}

int solveESTA(int add_mutexes) {
    cdebug("\nRunning ESTA+ algorithm.\n");
    timing_start("ESTA+");
    if (esta_plus(merge_bandwidth, add_mutexes)) {
        solverProgress(50);
        timing_stop("ESTA+");
    } else {
        timing_stop("ESTA+");
        cdebug("Could not find valid schedule. Aborting!\n");
        solverProgress(100);
        return 0;
    }
    cdebug("ESTA+ algorithm Done.\n");
    return 1;
}

int solveChaining() {
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
    return 1;
}

int solveLP() {
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
