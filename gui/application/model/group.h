#ifndef GROUP_H_
#define GROUP_H_

#include "model/activity.h"

/**
 * Group class. A group represents an collection of Activity objects from the same Job, with a common EST and LST.
 * From the solver perspective, there exists no order between activities in the same group.
 * However, from the GUI perspective, an order exists.
 */
class Group : public QObject {
    Q_OBJECT

    QList<Activity*> activities;
    int est;
    int lst;
    int estFlex;
    int lstFlex;
    int st;
    int job;
    int duration;
    bool locked;
public:
    /**
     * Constructs an empty group object with EST = LST = -1.
     */
    Group();

    /**
     * Constructs an empty group object from (job ID, EST, LST).
     * @param j job ID
     * @param est group EST
     * @param lst group LST
     */
    Group(int j, int est, int lst, int st);

    /**
     * @return list of activities contained in this group.
     */
    QList<Activity*> getActivities();

    /**
     * adds the given activity to this group (places it at the end of the activity list).
     * @param a the activity
     */
    void addActivity(Activity *a);

    /**
     * removes the given activity to this group.
     * @param the activity to be removed
     */
    void removeActivity(Activity *a);

    /**
     * @return whether this group contains a positive amount of activities.
     */
    bool hasActivities();

    /**
     * @return number of activities in this group.
     */
    int size();

    /**
     * @return the group flexibility est.
     */
    int getESTFlex();

    /**
     * @return the group flexibility lst
     */
    int getLSTFlex();

    /**
     * @param estflex
     */
    void setESTFlex(int estFlex);

    /**
     * @param lstflex
     */
    void setLSTFlex(int lstFlex);

    /**
     * set the group EST to the given value.
     * @param est the est
     */
    void setEST(int est);

    /**
     * @return group EST.
     */
    int getEST();

    /**
	 * set the group ST to the given value.
	 * @param st the start time
	 */
	void setST(int st);

	/**
	 * @return group ST.
	 */
	int getST();

    /**
     * set the group LST to the given value.
     * @param lst the lst
     */
    void setLST(int lst);

    /**
     * @return group LST.
     */
    int getLST();

    /**
     * @return group EET.
     */
    int getEnd();

    /**
     * @return job ID of job where this group belongs to.
     */
    int getJob();

    /**
     * @return group duration
     */
    int getDuration();

    /**
     * set the group duration to the given value.
     * @param duration the duration
     */
    void setDuration(int duration);

    /**
     * moves the given activity one place to the left in the activity list.
     */
    void moveLeft(Activity *a);

    /**
     * moves the given activity one place to the right in the activity list.
     */
    void moveRight(Activity *a);

    /**
     * @return string with merge commands of this group.
     */
    QString toString();

    /**
     * set the locked variable of the group.
     * @param locked
     */
    void setLocked(bool locked);

    /**
     * @return true iff the group is locked.
     */
    bool isLocked();
signals:
    void activityRemoved(Activity *a); ///< emitted when an activity is removed from the group.
};

#endif /* GROUP_H_ */
