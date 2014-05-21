#include "model/precedence.h"
#include "model/activity.h"

#include <cassert>

Precedence::Precedence(Activity *a1, Activity *a2) : _a1(a1), _a2(a2), hard(true) {}

Precedence::Precedence(Activity *a1, Activity *a2, bool hard) : _a1(a1), _a2(a2), hard(hard) {}

Activity* Precedence::a1() {
    return _a1;
}

Activity* Precedence::a2() {
    return _a2;
}

bool Precedence::isHard() {
    return hard;
}

int Precedence::getFrameNr() {
    return frameNumber;
}

void Precedence::setFrameNr(int frameNr) {
    frameNumber = frameNr;
}

bool Precedence::isIncoming(Activity *a) {
    return a == _a2;
}

bool Precedence::isOutgoing(Activity *a) {
    return a == _a1;
}

bool Precedence::operator==(const Precedence &other) const {
    return (other._a1 == _a1 && other._a2 == _a2 && other.hard == hard);
}

QString Precedence::toString() {
    QString output;
    if(isHard())
        output = "P %1 %2 %3 %4";
    else
        output = "S %1 %2 %3 %4";
    return output.arg(_a1->job()->id()).arg(_a1->id()).arg(_a2->job()->id()).arg(_a2->id());
}
