#include "requirement.h"

#include "resource.h"
#include "activity.h"
#include "job.h"

Requirement::Requirement(Activity *a, Resource *r, int amount)
    : _activity(a), _resource(r), _amount(amount) { }

Activity* Requirement::activity() {
    return _activity;
}

Resource* Requirement::resource() {
    return _resource;
}

int Requirement::amount() {
    return _amount;
}

void Requirement::setAmount(int amount) {
    _amount = amount;
}

bool Requirement::operator <(const Requirement &other) const {
    return _activity < other._activity;
}

QString Requirement::toString() {
    return QString("Q %1 %2 %3 %4\n").arg(_activity->job()->id()).arg(_activity->id()).arg(_resource->id()).arg(_amount);
}
