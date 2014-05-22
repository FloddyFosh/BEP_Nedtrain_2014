#ifndef CHAIN_H
#define CHAIN_H

#include <QVector>
#include <cassert>

#include "model/activity.h"

class Chain
{
private:
    int _resourceId, _chainId;
    QVector<Activity*> activities;

public:
    Chain(int resourceId, int id, QVector<Activity*> activities);

    int resourceId();
    int chainId();
    QVector<Activity*> getActivities();

    void addActivity(Activity* act);
};

#endif // CHAIN_H
