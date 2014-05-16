#include <list>

#include "chaining.h"
#include "alles.h"

#include "tmsp.h"
#include "stjn.h"
#include "esta_plus.h"
#include "debug.h"
#include "heap.h"

map< pair<int,int>, list<activity*> > chains;

bool compareEST(const activity* a, const activity* b) {
    return (a->est < b->est);
}

pair<int,int>* selectChain(int tr, int act, int res){
    printf("selectChain voor act(%d,%d) en resource %d\n",tr,act, res);
    map< pair<int,int>, list<activity*> >::iterator it;
    for(it=chains.begin();it!=chains.end();it++){
        pair<int,int> chainId = it->first;
        //inefficient; should only iterate over the correct resource
        if(chainId.first!=res) continue;
        list<activity*> curChain = it->second;
        if(curChain.size() == 0){
            printf("size 0 -> selected chain (%d,%d)\n",chainId.first, chainId.second);
            printf("NEW: est: %d, eft: %d flex: %d\n",A(tr,act)->est, A(tr,act)->est + A(tr,act)->duration,A(tr,act)->flex);
            return &chainId;
        }
        activity* chainEnd = curChain.back();
        printf("chain: %d unit: %d\n",chainId.first,chainId.second);
        printf("HEAD: est: %d, eft: %d flex: %d\n",chainEnd->est,chainEnd->est + chainEnd->duration, chainEnd->flex);
        if(A(tr,act)->est >= chainEnd->est + chainEnd->duration + chainEnd->flex){
            printf("selected chain (%d,%d)\n",chainId.first,chainId.second);
            printf("NEW: est: %d, eft: %d flex: %d\n",A(tr,act)->est, A(tr,act)->est + A(tr,act)->duration,A(tr,act)->flex);
            return &chainId;
        }
    }
    printf("NO CHAIN SELECTED!\n");
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

    sort(activities.begin(), activities.end(), compareEST);

    /*for(int i=0;i<activities.size();i++){
        printf("act(%d,%d) est: %d\n",activities[i]->i,activities[i]->j,activities[i]->est);
    }*/
	
	//INITIALIZE CHAINS
    //splits resources r_i in resource units u_j
    for(int r_i=0; r_i<tmsp->n_resources; r_i++){
        for(int u_j=0; u_j<R(r_i)->capacity; u_j++){
            pair<int,int> newPair(r_i, u_j);
            chains[newPair];
            printf("res %d cap %d\n",r_i,u_j);
        }
    }

    for(int i=0; i<tmsp->n_resources; i++){
        for(int k=0; k<activities.size(); k++){
            activity* act = activities[k];
            if(Q(act->i,act->j,i)>0)
                printf("activity (%d,%d), resource %d, req %d\n",act->i,act->j,i,Q(act->i,act->j,i));
            for(int m=0;m<Q(act->i,act->j,i);m++){
                pair<int,int> chainId = *selectChain(act->i,act->j,i);
                list<activity*>* chain = &chains[chainId];
                if(!chain->empty()){
                    activity* chainEnd = chain->back();
                    add_precedence(chainEnd->i, chainEnd->j, act->i, act->j);
                    fprintf(stderr, "PC: %d %d %d %d\n", chainEnd->i, chainEnd->j, act->i, act->j);
                }
                chain->push_back(act);
                printf("\n");
            }
        }
    }
	return 1;
}
