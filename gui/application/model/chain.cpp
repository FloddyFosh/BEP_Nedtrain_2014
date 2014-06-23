#include "chain.h"

Chain::Chain(int resourceId, int chainId, QVector<Activity*> acts, QObject* parent) :
    QObject(parent), _resourceId(resourceId), _chainId(chainId), activities(acts) {}

int Chain::resourceId() {
    return _resourceId;
}

int Chain::chainId() {
    return _chainId;
}

QVector<Activity*>* Chain::getActivities() {
    return &activities;
}

void Chain::addActivity(Activity* act) {
    activities.append(act);
}
