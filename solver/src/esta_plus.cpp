#include "alles.h"

#include "tmsp.h"
#include "stjn.h"
#include "esta_plus.h"
#include "debug.h"
#include "heap.h"

void print_hele_state();

extern int merge_bandwidth;
int leveling_constraints_before_chaining;
vector<map<int, peak2_t *> > peakhash;
Heap *peakqueue;

int get_conflict_type(conflict_t* c) {
    int a_before_b, b_before_a;
    
    a_before_b = BEFORE(c->n1, c->n2);
    b_before_a = BEFORE(c->n2, c->n1);

    if(!a_before_b && !b_before_a)
        return 1;
    if(!a_before_b && b_before_a)
        return 2;
    if(a_before_b && !b_before_a)
        return 3;
    return 4;
}

conflict_t* new_conflict(node_t* act1, node_t* act2) {
    conflict_t* c = new conflict_t;

    c->n1 = act1;
    c->n2 = act2;

    c->type = get_conflict_type(c);

    return c;
}

/* puts all conflicts in the peak into the right conflict set t1 to t4 */
void classify_conflicts(peak2_t* peak, vector<conflict_t *> * t1, vector<conflict_t *> * t2, vector<conflict_t *> * t3, vector<conflict_t *> * t4) {
    int i, j;
    vector<conflict_t *> * list = 0;

    for (i = 0; i < peak->activities.size(); i++) {
        for (j = i + 1; j < peak->activities.size(); j++) {
            node_t *act_i = (node_t *) list_get(peak->activities, i);
            node_t *act_j = (node_t *) list_get(peak->activities, j);

            conflict_t* c = new_conflict(act_i, act_j);

            switch (c->type) {
                case 1: list = t1; break;
                case 2: list = t2; break;
                case 3: list = t3; break;
                case 4: list = t4; break;
            }

            list_append(*list, c);
        }
    }
}


double get_w_res(conflict_t* c, bool div_S) {
    double S = 1;
    if (div_S) {
        if(SLACK(c->n1, c->n2) == 0 && SLACK(c->n2, c->n1) == 0)
            S = 1;
        else
            S = (double) (min(SLACK(c->n1, c->n2), SLACK(c->n2, c->n1))) /
                    (max(SLACK(c->n1, c->n2), SLACK(c->n2, c->n1)));
    }
    double sqrtS = sqrt(S);

    return min(SLACK(c->n1, c->n2) / sqrtS,
                   SLACK(c->n2, c->n1) / sqrtS);
}

int merge_score(node_t *a, node_t *b) {
    int res, score=0;

    for(res = 0; res < tmsp->n_resources; res++) if (R(res))
        score += ((Q(a->i, a->j, res) > 0) == (Q(b->i, b->j, res) > 0));
    return score;
}

conflict_t* select_merge_conflict(peak2_t *peak) {
    int i, j, score=-1;
    node_t *candidate_a, *candidate_b;

    for(i = 0; i < peak->activities.size(); i++) {
        node_t *act_i = peak->activities[i];
        for(j = i + 1; j < peak->activities.size(); j++) {
            node_t *act_j = peak->activities[j];
            
            if(act_i->i != act_j->i) continue;
            
            if (not act_i->canmerge or not act_j->canmerge) continue;

            //if(abs(act_i->est - act_j->est) <= merge_bandwidth) {
            if(abs(SLACK(act_i, act_j) - SLACK(act_j, act_i)) <= merge_bandwidth) {
                int newscore = merge_score(act_i, act_j);
                if(newscore > score) {
                    candidate_a = act_i;
                    candidate_b = act_j;
                    score = newscore;
                }
            }
        }
    }
    if(score > -1)
        return new_conflict(candidate_a, candidate_b);
    else
        return NULL;
}

conflict_t* select_conflict(peak2_t* peak) {
    int i;

    if (peak->activities.size() == 1) {
        // this activity alone requires more than is available in total
        return 0;
    }

    vector<conflict_t *> type1_conflicts,type2_conflicts,type3_conflicts,type4_conflicts;
    classify_conflicts(peak, &type1_conflicts, &type2_conflicts, &type3_conflicts, &type4_conflicts);

    if (type2_conflicts.empty() and type3_conflicts.empty() and type4_conflicts.empty()) {
        // all conflicts are type 1, this peak is unresolvable
        return 0;
    }


    /* select conflict with minimal w_res
     *
     * w_{res}(a1,a2) = min(d(e_a1,s_a2)/S, d(e_a2,s_a1)/S)
     * if all conflicts are type 4, then S = min/max, else S = 1
     */
    conflict_t* minimal_conflict = 0;
    double minimal_w_res = 0;
    bool div_S = (type2_conflicts.empty() and type3_conflicts.empty());

    for (i = 0; i < len(type2_conflicts); i++) {
        conflict_t* c = type2_conflicts[i];
        double w_res = get_w_res(c, div_S);
        if (!minimal_conflict || w_res < minimal_w_res) {
            minimal_conflict = c;
            minimal_w_res = w_res;
        }
    }
    for (i = 0; i < len(type3_conflicts); i++) {
        conflict_t* c = type3_conflicts[i];
        double w_res = get_w_res(c, div_S);
        if (!minimal_conflict || w_res < minimal_w_res) {
            minimal_conflict = c;
            minimal_w_res = w_res;
        }
    }
    if (div_S) {
        /* if there are type 2 or 3 conflicts, then the type 4 conflicts
         * will not have minimal w_res so we need not check them
         */
        for (i = 0; i < len(type4_conflicts); i++) {
            conflict_t* c = (conflict_t*) list_get(type4_conflicts, i);
            double w_res = get_w_res(c, div_S);
            if (!minimal_conflict || w_res < minimal_w_res) {
                minimal_conflict = c;
                minimal_w_res = w_res;
            }
        }
    }

    return minimal_conflict;

}

// Calculate the ordering score. Positive means n --> m is preferred,
// negative means m --> n is preferred.
int precedence_score(conflict_t* c) {
    debug("(%d,%d) --> (%d,%d) slack %d\n", c->n1->i, c->n1->j, c->n2->i, c->n2->j, SLACK(c->n1, c->n2));
    debug("(%d,%d) --> (%d,%d) slack %d\n", c->n2->i, c->n2->j, c->n1->i, c->n1->j, SLACK(c->n2, c->n1));
    return SLACK(c->n1, c->n2) - SLACK(c->n2, c->n1);
}

// lower score = higher priority
int peak_score(peak2_t *peak) {
    return -(peak->capacity - C(peak->resource));
}

struct nodesESTLT { bool operator()(node_t * a, node_t * b) const { return a->est < b->est; } };

void find_peaks(int res) {
    vector<node_t *> queue;
    int i, j;

    peakhash[res].clear();

    for(i = 0; i < len(R(res)->requirements); i++) {
        requirement *req = R(res)->requirements[i];
        queue.push_back(acts[req->i][req->j]);
    }
    sort(queue.begin(), queue.end(), nodesESTLT ());
    int cur = -1;
    node_t *act;
    peak2_t *peak = NULL, *oldpeak = NULL;
    FOREACH(queue, it) {
        node_t * act (*it);
        if(act->est > cur) {
            cur = act->est;
            debug("Creating new peak at t=%d\n", cur);
            oldpeak = peak;
            peak = new peak2_t;
            peak->time = cur;
            peak->capacity = 0;
            peak->resource = res;
            if(oldpeak) {
                heap_add(peakqueue, peak_score(oldpeak), oldpeak);
                for(j = 0; j < oldpeak->activities.size(); j++) {
                    node_t *oldact = oldpeak->activities[j];
                    if(oldact->est + oldact->len > cur) {
                        peak->capacity += Q(oldact->i, oldact->j, res);
                        list_append(peak->activities, oldact);
                        debug("Adding old activity %d,%d to peak, using capacity %d\n", oldact->i, oldact->j, Q(oldact->i, oldact->j, res));
                    }
                }
            }
            peakhash[res][cur] = peak;
        }
        debug("Adding new activity %d,%d to peak, using capacity %d\n", act->i, act->j, Q(act->i, act->j, res));
        peak->capacity += Q(act->i, act->j, res);
        list_append(peak->activities, act);
    }
    if(peak != NULL)
        heap_add(peakqueue, peak_score(peak), peak);
}

// delete updated activity n from peaks in [old_est, est) on resource res
// returns the last peak before the new est
peak2_t *remove_act_from_peaks(node_t *n, int res) {
    peak2_t *peak, *lastpeak=NULL;
    int i, j;

    debug("Removing (%d,%d) from peaks in [%d, %d)\n", n->i, n->j, n->old_est, (n->old_est+n->len < n->est? n->old_est+n->len : n->est));
    for(i = n->old_est; i < n->est && i < n->old_est+n->len; i++) {
        map<int, peak2_t *>::iterator peakit = peakhash[res].find(i);
        if(peakit == peakhash[res].end() && i == n->old_est) {
            // this peak should always exist, so we complain and quit
            debug("ERROR: no peak found for (%d,%d) at original starting time!\n", n->i, n->j);
            exit(1);
        } else if(peakit == peakhash[res].end()) {
            continue;
        }
        peak = (*peakit).second;
        peak->capacity -= Q(n->i, n->j, res);
        j = find(peak->activities.begin(), peak->activities.end(), n) - peak->activities.begin();
        if(j == peak->activities.size()) {
            debug("ERROR: current activity not found in this peak!\n");
            debug("Activity (%d,%d) at 0x%x est %d requires %d.\n", n->i, n->j, n, n->est, Q(n->i, n->j, peak->resource));
            if(len(n->group) > 1) {
                debug("Group of %d activities\n", len(n->group));
            }
            debug("Peak at t=%d on resource %d, capacity %d.\n", peak->time, peak->resource, peak->capacity);
            for(i = 0; i < peak->activities.size(); i++) {
                node_t *act = peak->activities[i];
                debug("activity %d: (%d,%d) at 0x%x est %d requires %d\n", i, act->i, act->j, act, act->est, Q(act->i, act->j, peak->resource));
            }
            exit(1);
        }
        peak->activities.erase(peak->activities.begin() + j);
        lastpeak = peak; // note, there's always at least one peak being processed in this loop, so lastpeak is never NULL
    }

    if(i < n->est) {
        // in this case there's a gap between the old end and the new start,
        // so lastpeak might not contain the right peak
        for(i = n->est; i >= n->old_est+n->len; i--) {
            map<int, peak2_t *>::iterator peakit = peakhash[res].find(i);
            if(peakit != peakhash[res].end()) {
                lastpeak = (*peakit).second;
                break;
            }
        }
    }

    return lastpeak;
}

// move updated activity n to the peak at its est
// create a new peak if necessary, using data from lastpeak
void move_act_to_peak(node_t *n, int res, peak2_t *lastpeak) {
    int i;
    peak2_t *peak;

    map<int, peak2_t *>::iterator peakit = peakhash[res].find(n->est);
    if(peakit == peakhash[res].end()) {
        // create a new peak at n->est
        debug("Creating new peak at %d with capacity %d\n", n->est, Q(n->i, n->j, res));
        peak = new peak2_t;
        peak->time = n->est;
        peak->capacity = Q(n->i, n->j, res);
        peak->resource = res;
        peak->activities.push_back(n);

        // copy active activities from lastpeak
        // lastpeak should be the peak preceding this new peak
        for(i = 0; i < lastpeak->activities.size(); i++) {
            node_t *act = lastpeak->activities[i];
            // Note: if old_est is set we need to use it. Otherwise, act will
            // be moved to some peaks prematurely
            if((act->old_est >= 0 ? act->old_est : act->est) + act->len > peak->time) {
                peak->capacity += Q(act->i, act->j, res);
                peak->activities.push_back(act);
            }
        }

        // Add the new peak to the hash table, and to the processing queue
        // At this moment the peak is still resource-feasible, but more acts
        // might be added at a later time
        peakhash[res][n->est] = peak;
        heap_add(peakqueue, peak_score(peak), peak);
    } else {
        peak = (*peakit).second;
        // In the case of an existing peak, n is already present if the old
        // end time is equal or larger than the new start time
        if(n->old_est + n->len <= n->est) {
            peak->activities.push_back(n);
            peak->capacity += Q(n->i, n->j, res);
            heap_add(peakqueue, peak_score(peak), peak);
        }
    }
}

// add activity n to existing peaks in [(old_)est+len, est+len)
void add_act_to_peaks(node_t *n, int res) {
    int i;
    peak2_t *peak;

    if(n->old_est + n->len <= n->est) i = n->est + 1; // new start of n is at or after the old end
    else i = n->old_est + n->len; // new start of n is before the old end
    debug("Adding (%d,%d) to peaks in [%d, %d)\n", n->i, n->j, i, n->est+n->len);

    for(; i < n->est + n->len; i++) {
        map<int, peak2_t *>::iterator peakit = peakhash[res].find(i);
        if(peakit == peakhash[res].end()) {
            continue; // we only add to existing peaks
        }
        peak = (*peakit).second;
        peak->capacity += Q(n->i, n->j, res);
        heap_add(peakqueue, peak_score(peak), peak);
        peak->activities.push_back(n);
    }
}

void update_peakheap(set<node_t *, orderLT> & h) {
    peak2_t *lastpeak;
    int i, res;

    while (!h.empty()) {
        node_t * n = *h.begin(); h.erase(h.begin());
        if(n->old_est < 0) continue; // due to reconvergence, there might be duplicates
        debug("Updating peaks for activity (%d,%d) (old EST %d, new EST %d, len %d)\n", n->i, n->j, n->old_est, n->est, n->len);
        for(res = 0; res < tmsp->n_resources; res++) if (R(res)) {
            if(Q(n->i, n->j, res) == 0) continue; // act doesn't use this resource, continue

            lastpeak = remove_act_from_peaks(n, res);
            move_act_to_peak(n, res, lastpeak);
            add_act_to_peaks(n, res);

        }
        n->old_est = -1;
        for(i = 0; i < len(n->next); i++) {
            node_t * m = n->next[i];
            if(m->old_est < 0) continue;
            h.insert(m);
        }
    }
}

void update_peaks(node_t *n) {
    set<node_t *, orderLT> h;
    h.insert(n);
    update_peakheap(h);
}

void update_group_peak(node_t *g, node_t *a, node_t *b) {
    int res, i, j, seen_us;
    peak2_t *peak = 0;
    node_t *m;

    for(res = 0; res < tmsp->n_resources; res++) if (R(res)) {
        if(Q(g->i, g->j, res) == 0) continue;
        debug("Updating peaks on resource %d for grouped activity (%d,%d)\n", res, g->i, g->j);
        peak2_t * lastpeak = NULL;
        if(g->old_est >= 0) {
            debug("New EST, remove in [%d,%d).\n", g->old_est, g->est);

            for(i = g->old_est; i < g->est; i++) {
                // Fix peak in [old_est, est)
                map<int, peak2_t *>::iterator peakit = peakhash[res].find(i);
                if(peakit == peakhash[res].end())
                    continue;
                peak = (*peakit).second;

                // Re-compute capacity for this peak & remove a and b from list
                peak->capacity = 0;
                vector<node_t *> l;
                for(j = 0; j < peak->activities.size(); j++) {
                    m = peak->activities[j];
                    if(m == a || m == b) continue;
                    l.push_back(m);
                    peak->capacity += Q(m->i, m->j, res);
                }
                peak->activities = l;
                heap_add(peakqueue, peak_score(peak), peak);
                lastpeak = peak;
            }
        }
        debug("Recomputing peaks in [%d, %d)\n", g->est, g->est+g->len);

        for(i = g->est; i < g->est+g->len; i++) {
            map<int, peak2_t *>::iterator peakit (peakhash[res].find(i));
            int newpeak = 0;
            if(peakit == peakhash[res].end()) {
                if(i == g->est) {
                    // If there's no peak at the start of g, this resource was not used by
                    // the activity originally starting at this time, and only by the one
                    // starting at old_est. Use lastpeak (which cannot be NULL at this point)
                    // to create a new peak.
                    debug("Creating new peak at %d on resource %d.\n", i, res);
                    newpeak = 1;
                    peak = new peak2_t;
                    peak->time = i;
                    peak->resource = res;

                    if(!lastpeak) {
                        debug("ERROR: assumption failure: lastpeak is not set.\n");
                        exit(1);
                    }
                    // Copy active activities from lastpeak
                    for(j = 0; j < lastpeak->activities.size(); j++) {
                        node_t *act = lastpeak->activities[j];
                        if(act->old_est >= 0) {
                            debug("ERROR: assumption failure: why is act->old_est set??\n");
                            exit(1);
                        }
                        if(act->est + act->len > peak->time)
                                            printf("ADD: i=%d, j=%d\n", act->i, act->j),peak->activities.push_back(act);
                    }
                } else
                    continue;
            }
            else peak = (*peakit).second;
            debug("Found peak at t=%d, recomputing.\n", i);
            peak->capacity = 0;
            seen_us = 0;
            vector<node_t *> l;
            for(j = 0; j < peak->activities.size(); j++) {
                m = peak->activities[j];
                // Remove a and b, but only if they're not equal to g
                if((a != g && m == a) || (b != g && m == b)) {
                    continue;
                }
                l.push_back(m);
                peak->capacity += Q(m->i, m->j, res);
                if(m == g) seen_us = 1;
            }
            peak->activities = l;
            // If g is not equal to a or b it is newly created, so it must be added
            if(!seen_us) {
                debug("Adding new group to this peak.\n");
                peak->activities.push_back(g);
                peak->capacity += Q(g->i, g->j, res);
            }
            if(newpeak) {
                peakhash[res][peak->time] = peak;
                heap_add(peakqueue, peak_score(peak), peak);
            }
        }
    }
    g->old_est = -1;

    // Update peaks for moved successors
    set<node_t *, orderLT> h;
    for(i = 0; i < len(g->next); i++) {
        m = g->next[i];
        if(m->old_est < 0) continue;
        h.insert(m);
    }
    update_peakheap(h);
}

void handle_neg_cyc() {
    print_hele_state(); // inclusief laatste merge of geposte constraint.
    deque<node_t *> pad;
    for (node_t * t (cycle_node); t; t = t->node_pred) {
        pad.push_front(t);
    }
    pad.pop_back(); // cycle node zelf geen 2x
    for (node_t * t (cycle_node); t; t = t->node_succ) {
        pad.push_back(t);
    }
    printf("CHAIN: ");
    FOREACH(pad, it) printf("%d %d ", (*it)->i, (*it)->j);
    node_t * vorige;
    bool wrong (false);
    FOREACH(pad, it) {
        node_t * t (*it);
        if (len(t->group) == 0) wrong = 1; // groep was al weg.
        if (it != pad.begin()) {
            wrong |= find(t->prev.begin(), t->prev.end(), vorige) == t->prev.end();
            wrong |= find(vorige->next.begin(), vorige->next.end(), t) == vorige->next.end();
        }
        vorige = t;
    }
    fprintf(stderr, "-1");
    if (wrong) { fprintf(stderr, "O.O"); while (1); }
}

int esta_plus() {
    /* Earliest Start Time Algorithm:
     *
     * 1. create STN
     * 2. loop
     * 3.   refresh temporal information in stn
     * 4.   conflictSet = computeConflicts
     * 5.   if no conflictSet { return EST schedule }
     * 6.   if unsolvable conflictSet { return null }
     * 7.   conflict = selectConflict
     * 8.   pc = selectPrecedenceConstraint(conflict)
     * 9.   postConstraint(pc)
     * 10.end loop
     */
    int i, j, k, ret=1, order;
    node_t *from, *to;
    peak2_t *peak;
    conflict_t *conflict;

    print_est_schedule();
    print_hele_state();//jan

    peakhash = vector<map<int, peak2_t *> > (tmsp->n_resources);
    peakqueue = new_heap();

    for(i = 0; i < tmsp->n_resources; i++) if (R(i)) {
        debug("Finding resource peaks for resource %d\n", i);
        find_peaks(i);
    }
    vector<pair<node_t *, node_t *> > posted;
    while((peak = (peak2_t *) heap_remove(peakqueue)) != NULL) {
        debug("Processing peak: t=%d res=%d cap=%d n_act=%d\n", peak->time, peak->resource, peak->capacity, peak->activities.size());
        if(peak->capacity <= C(peak->resource)) {
            debug("Peak is lower than resource capacity, continuing!\n");
            continue;
        }
        debug("Capacity of peak exceeds resource, flattening.\n");

        if(merge_bandwidth >= 0 && (conflict = select_merge_conflict(peak)) != NULL) {
            debug("Conflict (%d,%d) <--> (%d,%d) selected for merging.\n", conflict->n1->i, conflict->n1->j, conflict->n2->i, conflict->n2->j);
            if((to = stjn_merge(conflict->n1, conflict->n2)) == NULL) {
                debug("STN inconsistent!\n");
                ret = 0;
                goto cleanup;
            }
            if(to->i == conflict->n1->i && to->j == conflict->n1->j)
                fprintf(stderr, "MRG: %d %d %d %d\n", to->i, to->j, conflict->n2->i, conflict->n2->j);
            else
                fprintf(stderr, "MRG: %d %d %d %d\n", to->i, to->j, conflict->n1->i, conflict->n1->j);
            update_group_peak(to, conflict->n1, conflict->n2);
            
            print_hele_state();
        } else if((conflict = select_conflict(peak)) != NULL) {
            debug("Conflict (%d,%d) <--> (%d,%d) selected for leveling.\n", conflict->n1->i, conflict->n1->j, conflict->n2->i, conflict->n2->j);
            order = precedence_score(conflict);
            if(order > 0) {
                from = conflict->n1;
                to = conflict->n2;
            } else {
                from = conflict->n2;
                to = conflict->n1;
            }
            debug("Posting constraint (%d,%d) --> (%d,%d) (score %d).\n", from->i, from->j, to->i, to->j, order);
            fprintf(stderr, "PC: %d %d %d %d\n", from->i, from->j, to->i, to->j);
            posted.push_back(make_pair(from, to));
            
            if(!stjn_add_precedence(from, to)) {
                while (1); // mag nooit gebeuren
                
                debug("STN inconsistent!\n");
                ret = 0;
                goto cleanup;
            }
            update_peaks(to);
            
            print_hele_state();
        } else {
            debug("Unresolvable peak!\n");
            ret = 0;
            goto cleanup;
        }

        if(peak->capacity > C(peak->resource)) {
            debug("Peak capacity still exceeds resource capacity, re-inserting peak into queue.\n");
            heap_add(peakqueue, peak_score(peak), peak);
        }
    }

cleanup:
    peakhash.clear();
    delete_heap(peakqueue);
    if (ret)
    {
        bool wrong (false);
        // verify the EST schedule.
        for (int i = 0; i < tmsp->n_trains; i ++) if (T(i)) {
            for (int j = 0; j < N(i); j ++) if (A(i, j)) {
                node_t * act (acts[i][j]);
                A(i,j)->est = acts[i][j]->est;
                A(i,j)->lst = acts[i][j]->lst;
                if (!act->group.empty()) {
                    if (act->est < RD(i)) wrong = 1;
                    if (act->est + act->len > DD(i)) wrong = 1;
                    
                    FOREACH(act->prev, it) {
                        node_t * actPred (*it);
                        if (actPred->group.empty()) wrong = 1;
                        if (actPred->est + actPred->len > act->est) wrong = 1;
                    }
                    FOREACH(act->next, it) {
                        node_t * actSucc (*it);
                        if (actSucc->group.empty()) wrong = 1;
                        if (actSucc->est < act->est + act->len) wrong = 1;
                    }
                }
            }
        }
        if (wrong) fprintf(stderr, "SOLUTION WRONG!"), throw 0;
    }
    else
    {
        extern int add_mutexes;
        // print PEAK
        if (add_mutexes and peak->resource >= tmsp->n_resources - tmsp->n_trains)
        {
            fprintf(stderr, "MUTEX: %d %d %d",
                peak->time,
                peak->resource - (tmsp->n_resources - tmsp->n_trains),
                peak->capacity);
        }
        else
        {
            fprintf(stderr, "PEAK: %d %d %d",
                peak->time,
                peak->resource,
                peak->capacity);
        }
        
        FOREACH(peak->activities, it) fprintf(stderr, " %d %d", (*it)->i, (*it)->j);
        fprintf(stderr, " -1\n");
    }
    fprintf(stderr, "DONE!\n");
    return ret;
}

void print_hele_state() {
    int i, j, k;
    fprintf(stderr, "STATE:");
    extern vector<activity *> nodesInVolgordeVanInput;
    // Print merged tasks
    FOREACH(nodesInVolgordeVanInput, it) {
        activity * act = *it;
        int i = act->i, j = act->j;
        node_t * a = acts[i][j];
        if(len(a->group) >= 1) {
            // <job> <est> <lst> <#acts> [act id]
            fprintf(stderr, " %d %d %d %d", i, a->est, a->lst + a->flex, len(a->group));
            for (k = 0; k < len(a->group); k ++) {
                activity * child = list_get(a->group, k);
                fprintf(stderr, " %d %d", child->i, child->j);
            }
        }
    }
    fprintf(stderr, " -1\n");
}
