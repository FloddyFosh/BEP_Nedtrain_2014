#ifndef CHAIN_H
#define CHAIN_H

#include <QVector>
#include <cassert>

#include "model/activity.h"

class Chain : public QObject
{
private:
    Q_OBJECT

    int _resourceId, _chainId;
    QVector<Activity*> activities;

public:
    Chain(int resourceId, int chainId, QVector<Activity*> activities, QObject *parent);
    //Chain(int resourceId, int chainid, QObject* parent);
    //~Chain();

    int resourceId();
    int chainId();
    QVector<Activity*>* getActivities();

    void addActivity(Activity* act);
};

#endif // CHAIN_H
