#include "activitytemplate.h"

ActivityTemplate::ActivityTemplate(QString n, int d) : id(-1), name(n), duration(d){}

ActivityTemplate::ActivityTemplate(int i, QString n, int d) : id(i), name(n), duration(d){}

int ActivityTemplate::getDuration() const {
    return duration;
}

QString ActivityTemplate::getName() const {
    return name;
}

int ActivityTemplate::getID() const {
    return id;
}

void ActivityTemplate::setID(int i) {
    id = i;
}

void ActivityTemplate::setName(QString n) {
    name = n;
}

void ActivityTemplate::setDuration(int d) {
    duration = d;
}

void ActivityTemplate::addRequirement(QString n, int d) {
    requirements.insert(n, d);
}

void ActivityTemplate::setRequirements(QMap<QString, int> req) {
    requirements.clear();
    QMapIterator<QString, int> i(req);
    while (i.hasNext()) {
        i.next();
        if(i.value() == 0) throw 0;
        requirements.insert(i.key(), i.value());
    }
}

QMap<QString, int> ActivityTemplate::getRequirements() const {
    return requirements;
}

void ActivityTemplate::removeRequirement(QString n) {
    requirements.remove(n);
}

int ActivityTemplate::getRequiredAmount(QString name) {
    QMap<QString, int>::const_iterator it = requirements.find(name);
    if (it != requirements.end())
        return it.value();
    else
        return 0;
}

bool ActivityTemplate::isCompatibleWith(Instance *instance) {
    bool compatible = true;
    QMapIterator<QString, int> q(requirements);
    while (q.hasNext() && compatible) {
        q.next();
        compatible = instance->containsResource(q.key());
    }
    return compatible;
}

QString ActivityTemplate::getResourceNames() {
    QMapIterator<QString, int> it(this->getRequirements());
    QString resourceNames = "";
    while (it.hasNext()) {
        it.next();
        resourceNames += it.key();
        if(it.hasNext())
            resourceNames += ", ";
    }
    return resourceNames;
}
