#include <list>

#include "chaining.h"
#include "alles.h"

#include "tmsp.h"
#include "stjn.h"
#include "esta_plus.h"
#include "debug.h"
#include "heap.h"

map< pair<int,int>, list<activity*> > chains;
vector< activity* > activities;

bool compareEST(const activity* a, const activity* b) {
    return (a->est < b->est);
}

pair<int,int>* selectChain(int tr, int act, int res){
    //printf("selectChain voor act(%d,%d) en resource %d\n",tr,act, res);
    map< pair<int,int>, list<activity*> >::iterator it;
    for(it=chains.begin();it!=chains.end();it++){
        pair<int,int> chainId = it->first;
        //inefficient; should only iterate over the correct resource
        if(chainId.first!=res) continue;
        list<activity*> curChain = it->second;
        if(curChain.size() == 0){
            //printf("size 0 -> selected chain (%d,%d)\n",chainId.first, chainId.second);
            //printf("NEW: est: %d, eft: %d flex: %d\n",A(tr,act)->est, A(tr,act)->est + A(tr,act)->duration,A(tr,act)->flex);
            return &chainId;
        }
        activity* chainEnd = curChain.back();
        //printf("chain: %d unit: %d\n",chainId.first,chainId.second);
        //printf("HEAD: est: %d, eft: %d flex: %d\n",chainEnd->est,chainEnd->est + chainEnd->duration, chainEnd->flex);
        if(A(tr,act)->est >= chainEnd->est + chainEnd->duration + chainEnd->flex){
            //printf("selected chain (%d,%d)\n",chainId.first,chainId.second);
            //printf("NEW: est: %d, eft: %d flex: %d\n",A(tr,act)->est, A(tr,act)->est + A(tr,act)->duration,A(tr,act)->flex);
            return &chainId;
        }
    }
    printf("NO CHAIN SELECTED!\n");
    return 0;
}

void print_state() {
    int i, j, k;
    fprintf(stderr, "STATE:");
    FOREACH(activities, it) {
        activity * act = *it;
        i = act->i, j = act->j;
        activity* a = A(i,j);
        fprintf(stderr, " %d %d %d %d", i, a->est, a->lst + a->flex, (len(a->groupchilds)+1));
        fprintf(stderr, " %d %d", act->i, act->j);
        if(len(a->groupchilds) >= 1) {
            // <job> <est> <lst> <#acts> [act id]            
            for (k = 0; k < len(a->groupchilds); k ++) {
                activity * child = list_get(a->groupchilds, k);
                fprintf(stderr, " %d %d", child->i, child->j);
            }
        }
    }
    fprintf(stderr, " -1\n");
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

    //Clear precedence constraints that were generated by ESTA+
    fprintf(stderr,"CLEARSOFTPREC\n");
    print_state();

    //Sort
    FOREACH(tmsp->trains, it){
        vector<activity*> actVec = (*it)->activities;
        activities.insert(activities.end(), actVec.begin(), actVec.end());
    }
    sort(activities.begin(), activities.end(), compareEST);
	
    //Initialize chains
    for(int r_i=0; r_i<tmsp->n_resources; r_i++){
        for(int u_j=0; u_j<R(r_i)->capacity; u_j++){
            pair<int,int> newPair(r_i, u_j);
            chains[newPair];
        }
    }

    for(int i=0; i<tmsp->n_resources; i++){
        FOREACH(activities, it){
            activity* act = *it;
            for(int m=0;m<Q(act->i,act->j,i);m++){
                pair<int,int> chainId = *selectChain(act->i,act->j,i);
                list<activity*>* chain = &chains[chainId];
                if(!chain->empty()){
                    activity* chainEnd = chain->back();
                    add_precedence(chainEnd->i, chainEnd->j, act->i, act->j);
                    fprintf(stderr, "PC: %d %d %d %d\n", chainEnd->i, chainEnd->j, act->i, act->j);
                }
                chain->push_back(act);
                //printf("\n");
            }
        }
        print_state();
    }
	return 1;
}
