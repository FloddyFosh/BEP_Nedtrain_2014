#include "model/comparison.h"

#include "model/instance.h"
#include "controller/exceptions.h"

Comparison::Comparison(Instance *first, Instance *second) : first(first), second(second) {
    if(!haveSameJobs())
        throw ComparisonException(tr("The instances do not have the same jobs."));
    if(!haveSameActivities())
        throw ComparisonException(tr("The instances do not have the same activities."));
    if(!haveSameResources())
        throw ComparisonException(tr("The instances do not have the same resources."));
}

Instance* Comparison::getFirstInstance() {
    return first;
}

Instance* Comparison::getSecondInstance() {
    return second;
}

bool Comparison::hasDifferences(Activity *a) {
    int i = a->job()->id();
    int j = a->id();
    return first->A(i,j)->st() != second->A(i,j)->st();
}

bool Comparison::haveSameJobs() {
    if(first->getJobs().count() != second->getJobs().count())
        return false;
    foreach(Job *j1, first->getJobs()) {
        Job *j2 = second->J(j1->id());
        if(!(j2 && *j1 == *j2))
            return false;
    }
    return true;
}

bool Comparison::haveSameActivities() {
    foreach(Job *j, first->getJobs()) {
        if(j->getActivities().count() != second->J(j->id())->getActivities().count())
            return false;
        foreach(Activity *a1, j->getActivities()) {
            Activity *a2 = second->A(j->id(), a1->id());
            if(!(a2 && *a1 == *a2))
                return false;
        }
    }
    return true;
}

bool Comparison::haveSameResources() {
    if(first->getResources().count() != second->getResources().count())
        return false;
    foreach(Resource *r1, first->getResources()) {
        Resource *r2 = second->R(r1->id());
        if(!(r2 && *r1 == *r2))
            return false;
    }
    return true;
}
