#include "frame.h"
#include "instance.h"

QVector<Group *> Frame::getGroups() {
    return groups;
}

void Frame::addGroup(Group * g) {
    groups.append(g);
}

void Frame::display(Instance* inst){
    inst->setGroupPartition(groups);
}
