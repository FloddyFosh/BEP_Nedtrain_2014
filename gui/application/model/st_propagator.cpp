#include <set>
#include <algorithm>

#include "st_propagator.h"
#include "controller/instancecontroller.h"

ST_Propagator::ST_Propagator(InstanceController * controller, Group * group) : controller (controller), group (group)
{
    Instance * instance = controller->getInstance();
    foreach(Job * j, instance->getJobs()) {
        foreach(Group * g, j->getGroups()) {
            allGroups.push_back(g);
            savedST[g] = g->getST();
        }
    }
    vector<Precedence *> precedences;
    foreach(Precedence * p, instance->getHardPrecedences()) {
        precedences.push_back(p);
    }
    foreach(Precedence * p, instance->getSoftPrecedences()) {
        precedences.push_back(p);
    }
    for (vector<Precedence *>::iterator it = precedences.begin(); it != precedences.end(); ++it) {
        Precedence * p = *it;
        Group * preceding = instance->activityGroup(p->a1()), * preceded = instance->activityGroup(p->a2());
         adj[preceding].push_back(preceded );
        radj[preceded ].push_back(preceding);
    }
}

bool ST_Propagator::propagate(int fakeDuration, bool left) {
    se.clear();
    se.insert(qq_t (st[group], group));
    while (!se.empty()) {
        Group * x = (left ? *se.begin() : *(--se.end())).second;
        se.erase(left ? se.begin() : (--se.end()));

        if (not (x->getEST() <= st[x] and st[x] <= x->getLST())) return false;
        vector<Group *> & otherGroups(left ? adj[x] : radj[x]);

        foreach(Group *y, otherGroups)
            if(!relax(x, y, fakeDuration, left)) return false;
    }
    return true;
}

bool ST_Propagator::relax(Group *x, Group *y, int fakeDuration, bool left) {
    int st_y_2 = left ? st[x] + (x == group ? fakeDuration : x->getDuration()) : st[x] - (y == group ? fakeDuration : y->getDuration());
    if((left && st[y] < st_y_2) || (!left && st[y] > st_y_2)) {
        if (y->isLocked()) return false;
        se.erase (qq_t (st[y], y));
        st[y] = st_y_2;
        se.insert(qq_t (st[y], y));
    }
    return true;
}

void ST_Propagator::moveIfPossible(int dt) {
    if (group->isLocked()) return;
    st = savedST;
    st[group] += dt;
    // to the right
    if (propagate(group->getDuration(), true) and propagate(group->getDuration(), false)) {
        applyChanges();
    }
}

void ST_Propagator::changeDurationIfPossible(int newDuration) {
    if (group->isLocked()) return;
    int lct (group->getLST() + group->getDuration());
    if (group->getST() + newDuration > lct) return;
    st = savedST;
    // to the right
    if (propagate(newDuration, true) and propagate(newDuration, false)) {
        applyChanges(newDuration);
    }
}

void ST_Propagator::applyChanges(int newDuration) {
    controller->disconnectActivitiesFromResourceWidgets();
    
    foreach(Group *g, allGroups) {
        if (st[g] != g->getST()) g->setST(st[g]);
    }
    
    if (newDuration != -1) {
        group->getActivities()[0]->setDuration(newDuration);
        group->setDuration(newDuration);
    }
    
    controller->reconnectActivitiesToResourceWidgets();
    controller->repaintResourceWidgets();
}
