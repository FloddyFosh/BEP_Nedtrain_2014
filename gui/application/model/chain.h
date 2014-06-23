#ifndef CHAIN_H
#define CHAIN_H

#include <QVector>
#include <QObject>

class Activity;

class Chain : public QObject
{
private:
    Q_OBJECT

    int _resourceId, _chainId;
    QVector<Activity*> activities;

public:
    Chain(int resourceId, int chainId, QVector<Activity*> activities, QObject *parent);

    int resourceId();
    int chainId();
    QVector<Activity*>* getActivities();

    void addActivity(Activity* act);
};

#endif // CHAIN_H
