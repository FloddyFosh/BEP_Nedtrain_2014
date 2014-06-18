#include "frame.h"

Frame::Frame() : isChain(false) {}

QVector<Group *> Frame::getGroups() {
    return groups;
}

QVector<int> Frame::getAffectedResIds() {
    return affectedResIds;
}

QVector<int> Frame::getAffectedJobIds() {
    return affectedJobIds;
}

void Frame::addGroup(Group * g) {
    groups.append(g);
}

void Frame::addAffResourceId(int resId) {
    if(!affectedResIds.contains(resId))
        affectedResIds.append(resId);
}

void Frame::addAffJobId(int jobId) {
    if(!affectedJobIds.contains(jobId))
        affectedJobIds.append(jobId);
}

void Frame::clearAffected() {
    affectedJobIds.clear();
    affectedResIds.clear();
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
