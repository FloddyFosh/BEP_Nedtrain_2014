#include "model/instance.h"
#include "model/chain.h"
#include "resource.h"

Resource::Resource(Instance *instance, int id, QString name, int capacity, QObject *parent) :
    QObject(parent), _instance(instance), _id(id), _name(name), _capacity(capacity)
{ }

Resource::Resource(QString n) : _instance(0), _id(-1), _name(n), _capacity(0)
{}

int Resource::id() { return _id; }
Instance* Resource::instance() { return _instance; }
QString Resource::name() { return _name; }
int Resource::capacity() { return _capacity; }

void Resource::setName(QString name) {
    if (_name != name && name != "") {
        _name = name;
        emit resourceChanged();
    }
}

void Resource::setCapacity(int capacity) {
    if(_capacity != capacity && capacity >= 0) {
        _capacity = capacity;
        emit resourceChanged();
    }
}

QVector<Requirement*> Resource::getRequirements() {
    return requirements;
}

QMap<int, Chain*>* Resource::getChains() {
    return &chains;
}

void Resource::addActToChain(int jobId, int actId, int chainId) {
    Activity* act = _instance->getJobs()[jobId]->getActivities().value(actId);
    QMap<int,Chain*>* chainsPointer = getChains();
    if(!chainsPointer->contains(chainId)){
        QVector<Activity*> actVec;
        Chain newChain(_id,chainId,&actVec);
        chainsPointer->insert(chainId, &newChain);
    }

    /*QVector<Activity*> actVec;
    Chain newChain(_id,chainId,actVec);
    chains[chainId] = &newChain;*/
    Chain* chain = chainsPointer->value(chainId);
    //chain->getActivities().append(act);
    chain->addActivity(act);
    Chain* chain2 = chain;
}

void Resource::addActivity(Requirement *req) {
    requirements.append(req);
    emit activityAdded(req->activity());
    emit resourceChanged();
}

bool Resource::hasActivities() {
    return requirements.size() > 0;
}

void Resource::removeActivity(Requirement *req) {
	int index = requirements.indexOf(req);

	if(index>=0)
		requirements.remove(index);

	emit resourceChanged();
}

QMap<int, ResourceDecrease*> Resource::getDecreases() {
	return decreases;
}

ResourceDecrease *Resource::getDecrease(int id) {
	return decreases.contains(id) ? decreases[id] : 0;
}

void Resource::addDecrease(ResourceDecrease* d) {
	decreases[d->getId()] = d;
	emit resourceChanged();
}

void Resource::removeDecrease(ResourceDecrease* d) {
	decreases.remove(d->getId());
	_instance->removeDecreasePrecedence(d);
	delete d;
	emit resourceChanged();
}

QString Resource::toString() {
    return QString("R %1 %2 \"%3\"").arg(_id).arg(_capacity).arg(_name);
}

bool Resource::operator ==(const Resource &other) const {
    return _id == other._id && _name == other._name && _capacity == other._capacity;
}
