#ifndef MODEL_STATE_H
#define MODEL_STATE_H

using namespace std;

#include "activity.h"
#include "instance.h"

/**
 * The Frame class stores all groups and all (est, lst) for each group and for each non-grouped activity.
 * After each precedence posted by the solver, and each grouping operation by the solver, a new Frame is remembered.
 */
class Frame {
private:
    QVector<Group *> groups;
    //QList<QPoint*>* usedProfile;
    QList<QPoint*>* selectedProfile;
public:
    Frame();
    bool isChain;

    /**
     * Return groups belonging to this frame.
     * @return A vector of groups.
     */
    QVector<Group *> getGroups();

    virtual QList<QPoint*>* getUsedProfile();
    virtual QList<QPoint*>* getSelectedProfile();
    virtual Chain* getChain();

    /**
     * add a group to this frame.
     * @param g group to be added
     */
    void addGroup(Group * g);
};

#endif // MODEL_STATE_H
