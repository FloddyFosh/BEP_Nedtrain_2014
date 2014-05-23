#include <list>

#include "chaining.h"
#include "alles.h"

#include "tmsp.h"
#include "stjn.h"
#include "esta_plus.h"
#include "debug.h"
#include "heap.h"
#include "exceptions.h"

map< pair<int,int>, list<activity*> > chains;
vector< activity* > activities;

void initializeActivities(){
    FOREACH(tmsp->trains, it){
        vector<activity*> actVec = (*it)->activities;
        activities.insert(activities.end(), actVec.begin(), actVec.end());
    }
}

bool compareEST(const activity* a, const activity* b) {
    return (a->est < b->est);
}

void initializeChains(){
    for(int r_i=0; r_i<tmsp->n_resources; r_i++){
        for(int u_j=0; u_j<R(r_i)->capacity; u_j++){
            pair<int,int> newPair(r_i, u_j);
            chains[newPair];
        }
    }
}

pair<int,int> selectChain(int tr, int act, int res){
    map< pair<int,int>, list<activity*> >::iterator it;
    for(it=chains.begin();it!=chains.end();it++){
        pair<int,int> chainId = it->first;
        //inefficient; should only iterate over the correct resource
        if(chainId.first!=res) continue;
        list<activity*> curChain = it->second;
        if(curChain.size() == 0){
            return chainId;
        }
        activity* chainEnd = curChain.back();
        if(A(tr,act)->est >= chainEnd->est + chainEnd->duration + chainEnd->flex){
            return chainId;
        }
    }
    throw NoChainFoundException();
}

void pushToChain(activity* act, pair<int,int>* chainId){
    list<activity*>* chain = &chains[*chainId];
    if(!chain->empty()){
        activity* chainEnd = chain->back();
        add_precedence(chainEnd->i, chainEnd->j, act->i, act->j);
        fprintf(stderr, "PC: %d %d %d %d\n", chainEnd->i, chainEnd->j, act->i, act->j);
    }
    chain->push_back(act);
}

//Creates a frame of the current state of the program to be displayed in the GUI.
//This frame includes EST, LST of every activity/group and each precedence constraint posted after the previous frame.
void add_frame() {
    int i, j, k;
    fprintf(stderr, "STATE:");
    FOREACH(activities, it){
        activity* act = *it;
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

void print_chains() {
    fprintf(stderr, "CHAINS:");
    map< pair<int,int>, list<activity*> >::iterator it;
    for(it=chains.begin();it!=chains.end();it++){
        pair<int,int> chainId = it->first;
        // <resource> <chain> <nrOfActivities>
        fprintf(stderr," %d %d %d",chainId.first,chainId.second,len(it->second));
        FOREACH(it->second, actIt){
            activity* act = *actIt;
            //<job/train> <activity>
            fprintf(stderr," %d %d",act->i,act->j);
        }
    }
    fprintf(stderr," -1\n");
}

void print_chain(int i, int j) {
    fprintf(stderr, "CHAIN:");
    pair<int,int> newPair(i,j);
    list<activity*> chain = chains[newPair];
    // <resource> <chain> <nrOfActivities>
    fprintf(stderr," %d %d %d",i,j,len(chain));
    FOREACH(chain, it){
        activity* act = *it;
        //<job/train> <activity>
        fprintf(stderr," %d %d",act->i,act->j);
    }
    fprintf(stderr," -1\n");
}

bool chaining() {
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
    //fprintf(stderr,"CLEARSOFTPREC\n");

    initializeActivities();
    sort(activities.begin(), activities.end(), compareEST);

    add_frame();

    initializeChains();

    for(int i=0; i<tmsp->n_resources; i++){
        FOREACH(activities, it){
            activity* act = *it;
            for(int m=0;m<Q(act->i,act->j,i);m++){
                pair<int,int> chainId;
                try{
                    chainId = selectChain(act->i,act->j,i);
                }
                catch(NoChainFoundException &e){
                    e.showErrorMessage();
                    return false;
                }
                pushToChain(act, &chainId);
            }
        }
        FOREACH(chains, it){
            pair<int,int> chainId = it->first;
            if(chainId.first==i){
                print_chain(chainId.first,chainId.second);
                add_frame();
            }
        }
    }
    //print_chains();
    return true;
}
