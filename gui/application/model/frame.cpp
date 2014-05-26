#include "frame.h"
#include "instance.h"

Frame::Frame() : isChain(false) {}//: usedProfile(0), selectedProfile(0) {}

QVector<Group *> Frame::getGroups() {
    return groups;
}

/*QList<QPoint*>* Frame::getUsedProfile() {
    return usedProfile;
}

QList<QPoint*>* Frame::getSelectedProfile() {
    return selectedProfile;
}*/

void Frame::addGroup(Group * g) {
    groups.append(g);
}

void Frame::display(Instance* inst){
    inst->setGroupPartition(groups);

}
