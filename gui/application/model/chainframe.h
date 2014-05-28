#ifndef CHAINFRAME_H
#define CHAINFRAME_H

#include "frame.h"
#include "activity.h"
#include "instance.h"
#include "chain.h"

#include <QPoint>

class ChainFrame : public Frame {
private:
    Chain* chain;
    QList<QPoint*>* selectedProfile;
    QList<QPoint*>* usedProfile;
public:
    bool isChain;
    ChainFrame(Chain* c, QList<QPoint*>* prev);
    void initialize();

    Chain* getChain();
    QList<QPoint*>* getSelectedProfile();
    QList<QPoint*>* getUsedProfile();

    void setUsedProfile(QList<QPoint *> *p);
    void setSelectedProfile(QList<QPoint*>* p);
    QList<QPoint*>* addProfiles(QList<QPoint*>* pl1, QList<QPoint*>* pl2);
};

#endif // CHAINFRAME_H
