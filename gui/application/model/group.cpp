#include "model/group.h"

Group::Group() : est(-1), lst(-1), estFlex(-1), lstFlex(-1), st(-1), job(-1), duration(0), locked (false) { }

Group::Group(int j, int est, int lst, int st) : est(est), lst(lst), estFlex(-1), lstFlex(-1), st(st), job(j), duration(0), locked (false) {}

QList<Activity*> Group::getActivities() {
    return activities;
}

void Group::addActivity(Activity *a) {
    activities.append(a);
    a->setGroup(this);
    duration += a->duration();
}

void Group::removeActivity(Activity *a) {
    activities.removeOne(a);
    duration -= a->duration();
    emit activityRemoved(a);
    foreach(Activity* a, activities) a->setChanged();
}

bool Group::hasActivities() {
    return activities.size()>0;
}

int Group::size() {
    return activities.size();
}

int Group::getESTFlex() {
    return estFlex;
}

int Group::getLSTFlex() {
    return lstFlex;
}

void Group::setESTFlex(int estFlex) {
    this->estFlex = estFlex;
}

void Group::setLSTFlex(int lstFlex) {
    this->lstFlex = lstFlex;
}

void Group::setEST(int s) {
    est = s;
}

int Group::getEST() {
    return est;
}

void Group::setST(int s) {
	if(s<0)
		return;

    st = s;
    foreach(Activity* a, activities) a->setChanged();
}

int Group::getST() {
    return st;
}

void Group::setLST(int lst) {
    Group::lst = lst;
}

int Group::getLST() {
    return lst;
}

int Group::getEnd() {
    return est+duration;
}

int Group::getJob() {
    return job;
}

int Group::getDuration() {
    return duration;
}

void Group::setDuration(int duration) {
    Group::lst += Group::duration - duration;
    Group::duration = duration;
}

void Group::moveLeft(Activity *ma) {
    for (int i = 1; i < activities.size(); i++) {
        if (activities[i] == ma) {
            swap(activities[i - 1], activities[i]);
            break;
        }
    }
    foreach(Activity* a, activities) a->setChanged();
}

void Group::moveRight(Activity *ma) {
    for (int i = 0; i + 1 < activities.size(); i++) {
        if (activities[i] == ma) {
            swap(activities[i], activities[i + 1]);
            break;
        }
    }
    foreach(Activity* a, activities) a->setChanged();
}

QString Group::toString() {
    QString output = "";

    Activity *representative = activities[0];
    int jobId = representative->job()->id();
    int actId = representative->id();
    
    if (locked) {
        output += activities[0]->toString(st);
    }
    else {
        output += activities[0]->toString(-1);
    }
    for(int k=1; k<activities.size(); k++) {
        output += activities[k]->toString(-1);
    }
    for(int k=1; k<activities.size(); k++) {
        output += QString("G %1 %2 %3 %4\n").arg(jobId).arg(actId).arg(jobId).arg(activities[k]->id());
    }

    return output;
}

void Group::setLocked(bool locked) {
    Group::locked = locked;
}

bool Group::isLocked() {
    return locked;
}

