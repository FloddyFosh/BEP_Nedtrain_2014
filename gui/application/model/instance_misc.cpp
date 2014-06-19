#include "model/instance.h"

#include <climits>
#include <algorithm>

Instance::Instance(QObject *parent) :
    QObject(parent)
{
    fileName = "";
    next_job_index = 0;
    next_resource_index = 0;

    startTime = QTime(0, 0, 0);
    timelineStartTime = QTime(0, 0, 0);
    timeOffset = 0;
    useHoursOnTimeline = false;
    userChanges = false;

    importOffset = 0;
    importing = false;

    totalFlexibility = -1;
}

Instance::~Instance() {
    foreach(Resource *r, resources) delete r;
    foreach(Job *j, jobs) delete j;
    foreach(ResourceDecrease *rd, decreases) delete rd;
    foreach(Group *g, groups) delete g;
    foreach(Precedence *p, precedences) delete p;
    foreach(Precedence *p, softPrecedences) delete p;
}

QString Instance::getFileName() {
    return fileName;
}

void Instance::setFileName(QString name) {
    fileName = name;
}

QString Instance::toString() const {
    QString result;
    foreach(Resource *r, resources)
        result += r->toString() + "\n";

    result += "\n";

    foreach(Job * j, jobs)
        result += j->toString() + "\n\n";

    foreach(Precedence *p, precedences)
        result += p->toString() + "\n";

    foreach(Precedence *p, softPrecedences)
        result += p->toString() + "\n";

    foreach(Resource *r, resources)
    	foreach(ResourceDecrease* d, r->getDecreases())
    		result += d->toString() + "\n";
    
    return result;
}

bool Instance::hasUserChanges() {
    return userChanges;
}

void Instance::setUserChanges(bool changes) {
    if(changes)
        emit isModified(true);
    userChanges = changes;
    emit userChange();
}

int Instance::earliestReleaseDate() const {
    if (!jobs.size()) return 0;
    QList<int> dates;
    foreach(Job * j, jobs) dates.append(j->releaseDate());
    return *min_element(dates.begin(), dates.end());
}

int Instance::latestDueDate() const {
    if (!jobs.size()) return 0;
    QList<int> dates;
    foreach(Job * j, jobs) dates.append(j->dueDate());
    return *max_element(dates.begin(), dates.end());
}

int Instance::duration() const {
    return latestDueDate() - earliestReleaseDate();
}

Frame* Instance::getFrame(int frameNr) {
    return replayFrames[frameNr];
}

int Instance::getMaxFrameNr() {
    return replayFrames.size() - 1;
}

void Instance::setFrames(QVector<Frame *> frames) {
    replayFrames = frames;
}

void Instance::clearFrames() {
    replayFrames.clear();
}

void Instance::setTotalFlex(int flex) {
    prevTotalFlexibility = totalFlexibility;
    totalFlexibility = flex;
}

int Instance::getTotalFlex() {
    return totalFlexibility;
}

int Instance::getPrevFlex() {
    return prevTotalFlexibility;
}

QMap<int, Resource *> Instance::getResources() { return resources; }
QMap<int, Job *> Instance::getJobs() { return jobs; }
QVector<Precedence *> Instance::getHardPrecedences() { return precedences; }
QVector<Precedence *> Instance::getSoftPrecedences() { return softPrecedences; }
QList<Group *> Instance::getGroups() { return groups; }

Resource * Instance::R(int i) { return resources.contains(i) ? resources[i] : 0; }
Job *      Instance::J(int i) { return jobs.contains(i) ? jobs[i] : 0; }
Activity * Instance::A(int i, int j) { return J(i)->getActivities().contains(j) ? J(i)->getActivities()[j] : 0; }
