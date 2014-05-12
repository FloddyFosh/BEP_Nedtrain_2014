#include "chaining.h"
#include "alles.h"

#include "tmsp.h"
#include "stjn.h"
#include "esta_plus.h"
#include "debug.h"
#include "heap.h"

bool compareActivities(const activity* a, const activity* b) {
    return (a->est < b->est);
}

//Resource unit j of activity i with resource k
int selectChain(int i, int j, int k){
    return 1;
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
    vector<activity*> activities;
    for(int i=0;i<tmsp->n_trains; i++){
        activities.insert(activities.end(), T(i)->activities.begin(), T(i)->activities.end());
    }

    sort(activities.begin(), activities.end(), compareActivities);

    for(int i=0;i<activities.size();i++){
        cout << "trein" << list_get(activities,i)->i << " act" << list_get(activities,i)->j << " " << list_get(activities,i)->est << "\n";
    }
	
	//INITIALIZE CHAINS
	//splits resources in resource units


	
    /*for(int i=0; i<tmsp->n_resources; i++){
        for(int j=0; j<R(i)->capacity; j++){
            for(int k=0; k<activities.size(); k++){
                activity* act = activities[k];
                for(int m=0; m<REQ(act->i,act->j,i); m++){
                    vector<activity*> chain = selectChain(i,j,k);
                    activity* chainEnd = list_get(chain, chain.size());
                    chain.push_back(act);
                    add_precedence(chainEnd->i, chainEnd->j, act->i, act->j);
                }
            }
        }
    }*/
	
	
	return 1;
}
