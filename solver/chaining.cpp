#include <list>

#include "chaining.h"
#include "alles.h"

#include "tmsp.h"
#include "stjn.h"
#include "esta_plus.h"
#include "debug.h"
#include "heap.h"

map< pair<int,int>, list<activity*> > chains;

bool compareActivities(const activity* a, const activity* b) {
    return (a->est < b->est);
}

list<activity*>* selectChain(int tr, int act, int res){
    printf("selectChain voor act(%d,%d) en resource %d\n",tr,act, res);
    FOREACH(chains,elem){
        //pair<int,int> newPair(tr,act);
        list<activity*> curChain = *elem;
        activity* chainEnd = curChain.back();
        if(A(tr,act)->est >= chainEnd->est + chainEnd->duration){
            printf("selected chain\n");
            return &curChain;
        }
    }
    return 0;
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
    //splits resources r_i in resource units u_j
    int c = 0;
    for(int r_i=0; r_i<tmsp->n_resources; r_i++){
        for(int u_j=0; u_j<R(r_i)->capacity; u_j++){
            pair<int,int> newPair(r_i, u_j);
            chains[newPair];
            c++;
        }
    }
    //cout << "chains.size(): " << chains.size() << endl;
    //cout << "c: " << c << endl;
	
    for(int i=0; i<tmsp->n_resources; i++){
        for(int j=0; j<R(i)->capacity; j++){
            for(int k=0; k<activities.size(); k++){
                activity* act = activities[k];
                for(int m=0; m<REQ(act->i,act->j,i)->amount; m++){
                    list<activity*>* chain = selectChain(i,j,k);
                    activity* chainEnd = chain->back();
                    chain->push_back(act);
                    add_precedence(chainEnd->i, chainEnd->j, act->i, act->j);
                    printf("new constraint: (%d,%d) < (%d,%d)\n",chainEnd->i,chainEnd->j,act->i,act->j);
                }
            }
        }
    }
	return 1;
}
