#include "chaining.h"

#include "tmsp.h"
#include "output.h"
#include "exceptions.h"
#include "stjn.h"

#define useHeuristic true
#define useRandom true

map<chainId, chain> chains;
vector< activity* > activities;

void initializeActivities(){
    activities.clear();
    vector<train*> trains = tmsp->trains;
    FOREACH(trains, trit){
        if(*trit) {
            vector<activity*> actVec = (*trit)->activities;
            FOREACH(actVec, actit){
                if(*actit) activities.push_back(*actit);
            }
        }
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
    chains.clear();
    for(int r_i=0; r_i<tmsp->n_resources; r_i++){
        if(!R(r_i)) continue;
        for(int u_j=0; u_j<R(r_i)->capacity; u_j++){
            chainId newId = {r_i,u_j};
            chain newChain = {};
            chains[newId] = newChain;
        }
    }
}

bool isFeasibleConstraint(activity* act1, activity* act2){
    int curEst = act1->est == -1 ? Tr(act1->i)->release_date : act1->est;
    int prevLft = act2->est + act2->duration + act2->flex;
    if(act2->est == -1) prevLft = Tr(act2->i)->due_date;
    return curEst >= prevLft;
}

chainId selectFirstChain(int tr, int act, int res){
    FOREACH(chains,it){
        chainId id = it->first;
        if(id.resource!=res) continue;
        list<activity*> curChain = it->second.activities;
        if(curChain.empty()){
            return id;
        }
        activity* chainEnd = curChain.back();
        if(isFeasibleConstraint(A(tr,act),chainEnd)){
            return id;
        }
    }
    throw NoChainFoundException();
}

chainId selectRandomChain(int tr, int act, int res){
    map<chainId, chain>::iterator it;
    vector<chainId> possibleChains;
    for(it = chains.begin(); it != chains.end(); ++it){
        chainId id = it->first;
        if(id.resource != res) continue;
        list<activity*> curChain = it->second.activities;
        if(curChain.empty()){
            possibleChains.push_back(id);
            continue;
        }
        activity* chainEnd = curChain.back();
        if(isFeasibleConstraint(A(tr,act),chainEnd)){
            possibleChains.push_back(id);
        }
    }
    if(!possibleChains.empty()){
        int random = rand() % possibleChains.size();
        return possibleChains[random];
    }
    throw NoChainFoundException();
}

chainId selectBestChain(int tr, int act, int res){
    vector<chainId> possibleChains;
    FOREACH(chains, chit){
        if(chit->first.resource != res) continue;
        list<activity*> actVec = chit->second.activities;
        if(actVec.empty()) continue;
        activity* lastAct = actVec.back();
        FOREACH(tmsp->precedences, prit){
            precedence* prec = *prit;
            if(prec->i1 == lastAct->i && prec->j1 == lastAct->j
                    && prec->i2 == tr && prec->j2 == act) {
                possibleChains.push_back(chit->first);
                break;
            }
        }
    }
    if(!possibleChains.empty()){
        int random = rand() % possibleChains.size();
        return possibleChains[random];
    }
    return selectRandomChain(tr,act,res);
}

void pushToBestChains(int tr, int act, int res){
    activity* curAct = A(tr,act);
    int req = Q(tr,act,res);
    if(!req) return;
    chainId selectedChain;
    selectedChain = selectBestChain(tr,act,res);
    list<activity*> acts = chains[selectedChain].activities;
    activity* prevAct = acts.back();
    pushToChain(curAct,&selectedChain);
    req--;

    //Finds chains k where last(k) == prevAct,
    //because posting to such a chain will not create a new prec constraint
    map<chainId, chain>::iterator it;
    for(it = chains.begin(); it != chains.end() && req>0; ++it){
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

    //Otherwise, post to other feasible chains
    for(it = chains.begin(); it != chains.end() && req>0; ++it){
        chainId id = it->first;
        if(id.resource!=res) continue;
        list<activity*> curChain = it->second.activities;
        if(curChain.empty()){
            pushToChain(curAct, &id);
            req--;
            continue;
        }
        activity* chainEnd = curChain.back();
        if(chainEnd != prevAct && isFeasibleConstraint(curAct,chainEnd)){
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
        add_precedence(chainEnd->i, chainEnd->j, act->i, act->j, false);
        stjn_add_precedence(activity_to_node[chainEnd],activity_to_node[act]);
    }
    chain->push_back(act);
}

void add_frame() {
    int k;
    output("STATE:");
    FOREACH(activities, it){
        activity* act = *it;
        int i = act->i, j = act->j;
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
    vector<precedence*> newPrecedences;
    FOREACH(tmsp->precedences, it){
        if((*it)->isHard) newPrecedences.push_back(*it);
    }
    tmsp->precedences = newPrecedences;

    initializeActivities();
    sort(activities.begin(), activities.end(), compareEST);

    initializeChains();
    add_frame();

    for(int i=0; i<tmsp->n_resources; i++){
        if(!R(i)) continue;
        FOREACH(activities, it){
            activity* act = *it;
            if(useHeuristic){
                try{
                    pushToBestChains(act->i,act->j,i);
                }
                catch(NoChainFoundException &e){
                    e.showErrorMessage();
                    return false;
                }
            }
            else{
                for(int m=0;m<Q(act->i,act->j,i);m++){
                    chainId id;
                    try{
                        if(useRandom) id = selectRandomChain(act->i,act->j,i);
                        else id = selectFirstChain(act->i,act->j,i);
                    }
                    catch(NoChainFoundException &e){
                        e.showErrorMessage();
                        return false;
                    }
                    pushToChain(act, &id);
                }
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
