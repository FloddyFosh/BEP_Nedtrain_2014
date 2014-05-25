#ifndef CHAINFRAME_H
#define CHAINFRAME_H

#include "frame.h"
#include "activity.h"
#include "instance.h"

#include <QPoint>

class ChainFrame : public Frame {
private:
    Chain* chain;
    QList<QPoint*> usedProfile;
    QList<QPoint*> selectedChainProfile;
public:
    ChainFrame(Chain* c);

    Chain* getChain();
    QList<QPoint*> getUsedProfile();
    QList<QPoint*> getSelectedChainProfile();

    void setUsedProfile(QList<QPoint*> p);
    //void addToSelectedChainProfile(QPoint* p);
    void display(Instance *inst);
};

#endif // CHAINFRAME_H
