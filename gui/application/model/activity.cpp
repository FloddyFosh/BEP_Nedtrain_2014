#include "model/instance.h"
#include "controller/exceptions.h"
#include <cmath>
#include <algorithm>
#include <cassert>
using namespace std;

Activity::Activity(Job *job, int id, QString name, int duration)
    : _id(id), _name(name), _duration(duration), _job(job), flex(0)
{
    connect(this, SIGNAL(activityChanged()), job, SIGNAL(jobChanged()));
}

Activity::Activity()
    : _duration(0), _job (0), flex(0)
{
}

void Activity::setJob(Job *j) {
    _job = j;
    _id = j->findAvailableActivityId();
    connect(this, SIGNAL(activityChanged()), j, SIGNAL(jobChanged()));
}

Job* Activity::job() {
    return _job;
}

Group * Activity::group() {
    return _group;
}

void Activity::setGroup(Group * g) {
    _group = g;
}

int Activity::id() const { return _id; }
QString Activity::name() const { return _name; }

int Activity::offsetWithinGroup() const {
    int ofs = 0;
    foreach(Activity * act, _group->getActivities()) {
        if (act == this) break;
        ofs += act->duration();
    }
    return ofs;
}
int Activity::est() const {
    return _group->getEST() + offsetWithinGroup();
}
int Activity::st() const {
    return _group->getST() + offsetWithinGroup();
}
int Activity::lst() const {
    return _group->getLST() + offsetWithinGroup();
}
int Activity::eet() const {
    return est() + duration();
}
int Activity::duration() const { return _duration; }

void Activity::addPrecedence(Precedence *p) {
    if(p->a1() == this)
        outgoingPrecedences.append(p);
    else if(p->a2() == this)
        incomingPrecedences.append(p);
    else
        throw InstanceManipulationException("Invalid adding of precedence to activity");
}

void Activity::addDecreasePrecedence(ResourceDecrease *d) {
	decreasePrecedences.append(d);
}

QList<ResourceDecrease*> Activity::getDecreases(){
	return decreasePrecedences;
}

void Activity::removeDecrease(ResourceDecrease* d){
	decreasePrecedences.removeAll(d);
}

void Activity::clearDecreases(){
	decreasePrecedences.clear();
}

QList<Precedence *> Activity::getPrecedences() {
    QList<Precedence *> precedences;
    foreach(Precedence *p, incomingPrecedences)
        precedences.append(p);
    foreach(Precedence *p, outgoingPrecedences)
        precedences.append(p);
    return precedences;
}

QList<Precedence *> Activity::getIncomingPrecedences() {
    return incomingPrecedences;
}

QList<Precedence *> Activity::getOutgoingPrecedences() {
    return outgoingPrecedences;
}

void Activity::removePrecedence(Precedence *p) {
    incomingPrecedences.removeAll(p);
    outgoingPrecedences.removeAll(p);
}

void Activity::setName(QString name) {
    if (!name.isEmpty()) {
        _name = name;
        emit activityChanged();
    }
}

void Activity::setDuration(int duration) {
    if (duration > 0) {
        _duration = duration;
        emit activityChanged();
    }
}

QMap<int, Requirement*> Activity::getRequirements() {
    return requirements;
}

int Activity::getRequiredAmount(Resource *r) {
    if (requirements.contains(r->id()))
        return requirements[r->id()]->amount();
    else
        return 0;
}

Requirement* Activity::setRequiredAmount(Resource *r, int amount) {
    Requirement *req = 0;
    if(amount > 0) {
        if (!requirements.contains(r->id())) {
            req = new Requirement(this, r, amount);
            requirements[r->id()] = req;
            r->addActivity(req);
        } else {
            req = requirements[r->id()];
            req->setAmount(amount);
        }
    } else {
        if (requirements.contains(r->id()))
            r->removeActivity(requirements[r->id()]);
        requirements.remove(r->id());
    }
    emit activityChanged();
    return req;
}

bool Activity::operator <(const Activity &other) const {
    return est() == other.est() ? eet() < other.eet() : est() < other.est();
}

bool Activity::operator ==(const Activity &other) const {
    return  _id == other._id
            && name() == other.name()
            && duration() == other.duration()
            && sameRequirementsAs(other);
}

bool Activity::sameRequirementsAs(const Activity &other) const {
    if(requirements.count() != other.requirements.count())
        return false;
    QMapIterator<int, Requirement *> it(requirements);
    while(it.hasNext()) {
        it.next();
        Requirement *r1 = it.value();
        Requirement *r2 = other.requirements.value(it.key());
        if(!r2 || r1->resource()->id() != r2->resource()->id() || r1->amount() != r2->amount())
            return false;
    }
    return true;
}

Interval Activity::overlap(Activity *other) {
    Interval i;
    if (est() >= other->eet() || other->est() >= eet()) {
        // no overlap
        i.start = i.end = 0;
    } else {
        i.start = max(est(), other->est());
        i.end = min(eet(), other->eet());
    }
    return i;
}

void Activity::setFlex(int f) {
    if(flex >= 0)
        flex = f;
}

int Activity::getFlex() {
    return flex;
}

QString Activity::toString(int lockST) {
    QString output = QString("A %1 %2 %3 \"%4\" %5").arg(_job->id()).arg(_id).arg(_duration).arg(_name).arg(st());
    if(lockST != -1)
        output += QString(" %1 %2").arg(st()).arg(st());
    output += "\n";
    foreach(Requirement *r, requirements)
        output += r->toString();
    if(flex > 0)
        output += QString("F %1 %2 %3\n").arg(_job->id()).arg(_id).arg(flex);
    return output;
}

void Activity::setChanged() { emit activityChanged(); }
