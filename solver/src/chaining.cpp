#include "chaining.h"

#include "tmsp.h"
#include "debug.h"
#include "exceptions.h"

map<chainId, chain> chains;
vector< activity* > activities;

void initializeActivities(){
    vector<train*> trains = tmsp->trains;
    FOREACH(trains, it){
        vector<activity*> actVec = (*it)->activities;
        activities.insert(activities.end(), actVec.begin(), actVec.end());
    }
}

vector<activity*> getActivities(){
    return activities;
}

bool compareEST(const activity* a, const activity* b){
    return (a->est < b->est);
}

bool operator<(const chainId& a, const chainId& b){
    return make_pair(a.resource,a.unit) < make_pair(b.resource,b.unit);
}

void initializeChains(){
    for(int r_i=0; r_i<tmsp->n_resources; r_i++){
        for(int u_j=0; u_j<R(r_i)->capacity; u_j++){
            chainId newId = {r_i,u_j};
            chain newChain = {{}};
            chains[newId] = newChain;
        }
    }
}

chainId selectFirstChain(int tr, int act, int res){
    map<chainId, chain>::iterator it;
    for(it=chains.begin();it!=chains.end();it++){
        chainId id = it->first;
        //inefficient; should only iterate over the correct resource
        if(id.resource!=res) continue;
        list<activity*> curChain = it->second.activities;
        if(curChain.size() == 0){
            return id;
        }
        activity* chainEnd = curChain.back();
        if(A(tr,act)->est >= chainEnd->est + chainEnd->duration + chainEnd->flex){
            return id;
        }
    }
    throw NoChainFoundException();
}

chainId selectEarliestChain(int tr, int act, int res){
    chainId* result = 0;
    int curEST = INT_MAX;
    map<chainId, chain>::iterator it;
    for(it=chains.begin();it!=chains.end();it++){
        chainId id = it->first;
        //inefficient; should only iterate over the correct resource
        if(id.resource!=res) continue;
        list<activity*> curChain = it->second.activities;
        if(curChain.size() == 0){
            return id;
        }
        activity* chainEnd = curChain.back();
        int newEST = chainEnd->est + chainEnd->duration + chainEnd->flex;
        if(A(tr,act)->est >= newEST && newEST <= curEST){
            result = &id;
            curEST = newEST;
        }
    }
    if(result==0) throw NoChainFoundException();
    return *result;
}

chainId selectRandomChain(int tr, int act, int res){
    map<chainId, chain>::iterator it;
    vector<chainId> possibleChains;
    for(it=chains.begin();it!=chains.end();it++){
        chainId id = it->first;
        if(id.resource!=res) continue;
        list<activity*> curChain = it->second.activities;
        if(curChain.size() == 0){
            possibleChains.push_back(id);
            continue;
        }
        activity* chainEnd = curChain.back();
        if(A(tr,act)->est >= chainEnd->est + chainEnd->duration + chainEnd->flex){
            possibleChains.push_back(id);
        }
    }
    if(!possibleChains.empty()){
        int random = rand() % possibleChains.size();
        return possibleChains[random];
    }
    throw NoChainFoundException();
}

void pushToBestChains(int tr, int act, int res){
    activity* curAct = A(tr,act);
    int req = Q(tr,act,res);
    if(!req) return;
    chainId randomChain = selectFirstChain(tr,act,res);
    list<activity*> acts = chains[randomChain].activities;
    activity* prevAct = acts.back();
    pushToChain(curAct,&randomChain);
    req--;

    map<chainId, chain>::iterator it;
    for(it=chains.begin();it!=chains.end() && req>0;it++){
        chainId id = it->first;
        if(id.resource!=res) continue;
        list<activity*> curChain = it->second.activities;
        if(curChain.empty()) continue;
        activity* chainEnd = curChain.back();
        if(chainEnd == prevAct){
            pushToChain(curAct, &id);
            req--;
        }
    }

    for(it=chains.begin();it!=chains.end() && req>0;it++){
        chainId id = it->first;
        if(id.resource!=res) continue;
        list<activity*> curChain = it->second.activities;
        if(curChain.empty()){
            pushToChain(curAct, &id);
            req--;
            continue;
        }
        activity* chainEnd = curChain.back();
        if(chainEnd != prevAct && curAct->est >= chainEnd->est + chainEnd->duration + chainEnd->flex){
            pushToChain(curAct, &id);
            req--;
        }
    }

    if(req>0) throw NoChainFoundException();
}

void pushToChain(activity* act, chainId* id){
    list<activity*>* chain = &chains[*id].activities;
    if(!chain->empty()){
        activity* chainEnd = chain->back();
        add_precedence(chainEnd->i, chainEnd->j, act->i, act->j);
        output("PC: %d %d %d %d\n", chainEnd->i, chainEnd->j, act->i, act->j);
    }
    chain->push_back(act);
}

//Creates a frame of the current state of the program to be displayed in the GUI.
//This frame includes EST, LST of every activity/group and each precedence constraint posted after the previous frame.
void add_frame() {
    int i, j, k;
    output("STATE:");
    FOREACH(activities, it){
        activity* act = *it;
        i = act->i, j = act->j;
        activity* a = A(i,j);
        output(" %d %d %d %d", i, a->est, a->lst + a->flex, (len(a->groupchilds)+1));
        output(" %d %d", act->i, act->j);
        if(len(a->groupchilds) >= 1) {
            // <job> <est> <lst> <#acts> [act id]
            for (k = 0; k < len(a->groupchilds); k ++) {
                activity * child = list_get(a->groupchilds, k);
                output(" %d %d", child->i, child->j);
            }
        }
    }
    output(" -1\n");
}

void print_chain(int i, int j) {
    output("CHAIN:");
    chainId id = {i,j};
    list<activity*> chain = chains[id].activities;
    // <resource> <chain> <nrOfActivities>
    output(" %d %d %d",i,j,len(chain));
    FOREACH(chain, it){
        activity* act = *it;
        //<job/train> <activity>
        output(" %d %d",act->i,act->j);
    }
    output(" -1\n");
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

    srand(time(NULL));

    //Clear precedence constraints that were generated by ESTA+
    output("CLEARSOFTPREC\n");

    initializeActivities();
    sort(activities.begin(), activities.end(), compareEST);

    initializeChains();
    add_frame();

    for(int i=0; i<tmsp->n_resources; i++){
        FOREACH(activities, it){
            activity* act = *it;
            /*try{
                pushToBestChains(act->i,act->j,i);
            }
            catch(NoChainFoundException &e){
                e.showErrorMessage();
                return false;
            }*/

            for(int m=0;m<Q(act->i,act->j,i);m++){
                chainId id;
                try{
                    id = selectFirstChain(act->i,act->j,i);
                }
                catch(NoChainFoundException &e){
                    e.showErrorMessage();
                    return false;
                }
                pushToChain(act, &id);
            }
        }
        add_frame();
        FOREACH(chains, it){
            chainId id = it->first;
            if(id.resource==i && len(it->second.activities)>0){
                print_chain(id.resource,id.unit);
            }
        }
    }
    return true;
}
