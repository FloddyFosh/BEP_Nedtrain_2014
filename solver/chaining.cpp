#include "tms/tmsp.h"
#include "tms/alles.h"

bool() sortActivities(activity& a, activity& b) {
	return (a->est < b->est);
}

int chaining() {
	/* PSEUDOCODE: [Generating Robust Partial Order Schedules, Policella 2004]
	Input: A problem P and one of its fixed-times schedules S
	Output: A partial order solution POS
	1. POS <- P
	2. Sort all the activities according to their start times in S
	3. Initialize all chains empty
	4. for each resource r_j
	5.		for each activity a_i
	6.				for 1 to req_i,j
	7.						k <- SelectChain(a_i,r_j)
	8.						a_k <- last(k)
	9.						AddConstraint(POS, a_k < a_i)
	10.						last(k) <- a_i
	11. return POS	
	*/
	
	//SORT
	for(int i=0; i<tmsp->trains.length; i++) {
		sort(T(i)->activities.begin(), T(i)->activities.end(), sortActivities());
	}
	
	//INITIALIZE CHAINS
	//splits resources in resource units
	
	//for(int i=0; i<tmsp->
	
	
	return 1;
}