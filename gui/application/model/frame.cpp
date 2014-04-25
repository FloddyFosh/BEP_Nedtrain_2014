#include "frame.h"

QVector<Group *> Frame::getGroups() {
    return groups;
}

void Frame::addGroup(Group * g) {
    groups.append(g);
}
