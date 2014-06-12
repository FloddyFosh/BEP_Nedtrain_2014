#include "frame.h"

Frame::Frame() : isChain(false) {}

QVector<Group *> Frame::getGroups() {
    return groups;
}

QList<QPoint*>* Frame::getSelectedProfile() {
    return 0;
}

QList<QPoint*>* Frame::getUsedProfile() {
    return 0;
}

Chain* Frame::getChain() {
    return 0;
}

void Frame::addGroup(Group * g) {
    groups.append(g);
}
