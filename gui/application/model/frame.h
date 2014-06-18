#ifndef MODEL_STATE_H
#define MODEL_STATE_H

#include <QVector>

#include "activity.h"

using namespace std;

/**
 * The Frame class stores all groups and all (est, lst) for each group and for each non-grouped activity.
 * After each precedence posted by the solver, and each grouping operation by the solver, a new Frame is remembered.
 */
class Frame {
private:
    QVector<Group *> groups;
    QVector<int> affectedResIds;
    QVector<int> affectedJobIds;
public:
    /**
     * Constructs an empty Frame.
     */
    Frame();

    /**
     * Return groups belonging to this frame.
     * @return A vector of groups.
     */
    QVector<Group *> getGroups();

    /**
     * Return Resource Ids that were affected in this frame.
     * @return A vector of Resource Ids.
     */
    QVector<int> getAffectedResIds();

    /**
     * Return Job Ids that were affected in this frame.
     * @return A vector of Job Ids.
     */
    QVector<int> getAffectedJobIds();

    /**
     * Add a group to this frame.
     * @param g group to be added
     */
    void addGroup(Group * g);

    /**
     * Add an affected resource.
     * @param resId Id of the resource
     */
    void addAffResourceId(int resId);

    /**
     * Add an affected job.
     * @param jobId Id of the job
     */
    void addAffJobId(int jobId);

    /**
     * Clears the affected vectors
     */
    void clearAffected();

    /**
     * Get the sub-profile which is selected in this frame for visualization.
     * @return A list of Qpoints representing the profile
     */
    virtual QList<QPoint*>* getSelectedProfile();

    /**
     * Get the sub-profile which has been used so far for visualization.
     * @return A list of Qpoints representing the profile
     */
    virtual QList<QPoint*>* getUsedProfile();

    /**
     * Get the chain which this frame is representing.
     * @return the chain representing this frame
     */
    virtual Chain* getChain();

    /**
     * Returns if this frame is a ChainFrame.
     * @return boolean
     */
    bool isChain;
};

#endif // MODEL_STATE_H
