#include "chain.h"

Chain::Chain(int resourceId, int chainId, QVector<Activity*> acts) :
    _resourceId(resourceId), _chainId(chainId), activities(acts) {}

int Chain::resourceId() {
    return _resourceId;
}

int Chain::chainId() {
    return _chainId;
}

QVector<Activity *> Chain::getActivities() {
    return activities;
}

void Chain::addActivity(Activity* act) {
    if(!activities.empty()){
        Activity* last = activities.last();
        assert(last->est() + last->duration() + last->getFlex() <= act->est());
    }
    activities.append(act);
}
