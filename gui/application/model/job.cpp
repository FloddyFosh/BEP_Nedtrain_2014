#include "model/job.h"
using namespace std;

Job::Job(Instance *instance, int id, QString name, int release_date, int due_date)
    : _id(id), _name(name), _release_date(release_date), _due_date(due_date), _instance(instance)
{
    connect(this, SIGNAL(jobChanged()), instance, SIGNAL(instanceChanged()));
}

Job::Job() : _id(-1), _name(""), _release_date(0), _due_date(0), _instance(0) { }

void Job::addGroup(Group *g) {
    groups.append(g);
    emit groupAdded(g);
}

void Job::removeGroup(Group *g) {
    groups.removeOne(g);
    emit groupRemoved(g);
}

QList<Group *> Job::getGroups() {
    return groups;
}

void Job::clearGroups() {
    groups.clear();
}

void Job::addActivity(Activity *a) {
    activities[a->id()] = a;
    emit activityAdded(a);
}

void Job::removeActivity(Activity *a) {
    activities.remove(a->id());
    emit activityRemoved();
}

bool Job::hasActivities() {
    return activities.size() > 0;
}

QMap<int, Activity*> Job::getActivities() {
    return activities;
}

int Job::findAvailableActivityId() {
    int i = -1;
    while (activities.count(++i) > 0) { }
    return i;
}

int Job::getRequiredAmount(Resource *r) {
    int amount = 0;
    foreach(Activity *a, getActivities().values()) {
        if(a->getRequiredAmount(r) > amount)
            amount = a->getRequiredAmount(r);
    }
    return amount;
}

int Job::id() const {
    return _id;
}

QString Job::name() const {
    return _name;
}

int Job::releaseDate() const {
    return _release_date;
}

int Job::dueDate() const {
    return _due_date;
}

int Job::duration() {
    return dueDate()-releaseDate();
}

Instance *Job::instance() const {
    return _instance;
}

void Job::setName(QString name) {
    if (!name.isEmpty()) {
        _name = name;
        emit jobChanged();
    }
}

void Job::setReleaseDate(int rd) {
    if (rd >= 0) {
        _release_date = rd;
        emit jobChanged();
    }
}

void Job::setDueDate(int dd) {
    if (dd >= 0) {
        _due_date = dd;
        emit jobChanged();
    }
}

QString Job::toString() {
    QString output = QString("J %1 %2 %3 \"%4\"\n").arg(_id).arg(releaseDate()).arg(dueDate()).arg(name());
    foreach(Group * g, groups)
        output += g->toString();
    return output;
}

bool Job::operator==(const Job &other) const {
    return _id == other._id && name() == other.name() && releaseDate() == other.releaseDate() && dueDate() == other.dueDate();
}

int Job::groupCount() { return groups.size(); }

