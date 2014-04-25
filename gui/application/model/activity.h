#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "model/instance.h"
#include "model/precedence.h"

using namespace std;

class Job;
class Requirement;
class Interval;
class Group;

/**
 * An activity is a unit of work that must be completed on a specific job.
 * The most important property of an activity object is its earliest start
 * time (est). The visualiser presents an activity at this point in time.
 * Solvers should provide updated EST values to the visualiser.
 */
class Activity : public QObject {
    Q_OBJECT

    int _id;
    QString _name;
    int _duration;
    Job *_job;
    int flex;
    QList<Precedence *> incomingPrecedences;
    QList<Precedence *> outgoingPrecedences;
    QList<ResourceDecrease *> decreasePrecedences;

    QMap<int, Requirement*> requirements;
    
    Group * _group;
public:

    /**
     * Constructor used when loading from file.
     * @param j parent job
     * @param id integer id from inputfile
     * @param name descriptive name of this activity
     * @param duration integer duration of the activity
     */
    Activity(Job *j, int id, QString name, int duration);

    /**
     * Constructor used when adding new activity in UI.
     */
    Activity();

    /**
     * Set the group where the activity belongs to.
     * @param g the group
     */
    void setGroup(Group *g);

    /**
     * @return the group where the activity belongs to.
     */
    Group * group();
    
    /**
     * Initialize activity that was manually added through UI.
     */
    void setJob(Job *);

    /**
     * @return a QMap with requirements
     */
    QMap<int, Requirement*> getRequirements();

    /**
     * Returns the amount of capacity of a given resource required by this activity.
     * @param resource resource in question
     * @return required amount
     */
    int getRequiredAmount(Resource *resource);

    /**
     * Update the amount required by this activity of a given resource. The requirement
     * is also synced with the Resource, which also keeps track of the requirements.
     * @param resource the resource in question
     * @param amount integer amount required
     * @returns a pointer to the generated Requirement object, or 0 if amount was 0
     */
    Requirement* setRequiredAmount(Resource *, int amount);

    /**
     * The possible overlap interval is found (based on est and lst from this and the other activity) and returned.
     * @param other possibly overlapping activity
     * @return overlap interval
     */
    Interval overlap(Activity *other);

    /**
     * @return the id of this activity.
     */
    int id() const;

    /**
     * @return a descriptive name of this activity.
     */
    QString name() const;

private:
    /**
     * @return sum of durations of activities in this group at positions before this activity.
     */
    int offsetWithinGroup() const;
public:
    /**
     * @return the earliest starting time of this activity.
	 */
    int est() const;
    
    /**
     * @return the earliest ending time of this activity.
	 */
    int eet() const;
    
    /**
     * @return the latest starting time of this activity.
	 */
    int lst() const;
    
    /**
     * @return the currently set starting time of this activity.
	 */
    int  st() const;

    /**
     * @return the duration of this activity.
     */
    int duration() const;

    /**
     * Update the name of this activity.
     * @param name the new name
     */
    void setName(QString name);

	/**
	 * Update added flexibility of the activity.
	 * @param f added flexibilit
	 */
	void setFlex(int f);

	/**
	 * @return added flexibility
	 */
	int getFlex();

    /**
     * @return the job which this activity belongs to
     */
    Job* job();

    /**
     * Update the duration of this activity.
     * @param duration new integer duration
     */
    void setDuration(int duration);

    /**
     * Compare activities (by earliest starting time, then by duration).
     * @return comparison result
     */
    bool operator <(const Activity &other) const;

    /**
     * Compare activities by id, name and duration.
     * Also checks if the requirements match.
     * @return true if the activities are the same, false otherwise
     */
    bool operator ==(const Activity &other) const;

    /**
     * Checks if this activity has the same requirements as another one.
     * @param other The other activity.
     * @return true if both activities contain the same requirements, false otherwise.
     */
    bool sameRequirementsAs(const Activity &other) const;

    /**
     * Adds a precedence to the activity.
     * @param p precedence
     */
    void addPrecedence(Precedence *p);

    /**
     * @return a list containing all incoming or outgoing precedences with respect to this activity.
     */
    QList<Precedence *> getPrecedences();

    /**
     * Adds a resource decrease which has a precedence constraint to this activity
     * @param d The decrease which has a precedence relation to this activity
     */
    void addDecreasePrecedence(ResourceDecrease *d);

    /**
     * @return A list of all the resource decreases which have a precedence relation to this activity
     */
    QList<ResourceDecrease*> getDecreases();

    /**
     * Removes a decrease from the internal list of resource decreases which have a precedence relation to this activity
     */
    void removeDecrease(ResourceDecrease* d);

    /**
     * Clears the internal list of resource decreases which have a precedence relation to this activity
     */
    void clearDecreases();

    /**
     * @return a list containing all incoming precedences with respect to this activity.
     */
    QList<Precedence *> getIncomingPrecedences();

    /**
     * @return a list containing all outgoing precedences with respect to this activity.
     */
    QList<Precedence *> getOutgoingPrecedences();

    /**
     * Removes a precedence from this activity.
     * @param precedence precedence to be removed
     */
    void removePrecedence(Precedence * precedence);

    /**
     * Clears the maximum allowed interval for the activity's est and lct.
     */
    void clearInterval();

    /**
     * @return a string representation of this activity, parseable by the solver.
     */
    QString toString(int lockST = -1);
    
    /**
     * forces the activityChanged signal to be emitted.
     */
    void setChanged();
signals:
    void activityChanged(); ///< signal to signify that a UI update may be needed
    void precedenceAdded(Precedence *); ///< emitted when a precedence is added
};

#endif // ACTIVITY_H
