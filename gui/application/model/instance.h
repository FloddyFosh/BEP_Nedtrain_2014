#ifndef INSTANCE_H
#define INSTANCE_H

#include <QString>
#include <QVector>
#include <QSet>
#include <QMap>
#include <QList>
#include <QTime>

class Group;
class ResourceDecrease;
class Instance;
class Resource;
class Job;
class Activity;
class Requirement;
class Frame;

#include "model/resourcedecrease.h"
#include "model/group.h"
#include "model/resource.h"
#include "model/requirement.h"
#include "model/job.h"
#include "model/activity.h"
#include "model/precedence.h"

/** Root instance struct. Holds references to jobs, precedences and resources.
  */
class Instance : public QObject
{
    Q_OBJECT

    QMap<int, Resource *> resources; ///< map containing all resources.
    QMap<int, Job *> jobs; ///< map containing all jobs.
    QVector<Precedence *> precedences; ///< vector containing all precedences (hard).
    QVector<Precedence *> softPrecedences; ///< vector containing all soft precedences, posted by the solver.
    QMap<int, ResourceDecrease *> decreases; ///< Map from integer job id to dummy job object.
    QList<Group *> groups; ///< List of groups present at the current frame.
    QMap<Activity*, Group*> groupMap; ///< Map from activities to the group in which they are contained. Only goes for the current frame!

    unsigned int next_job_index; ///< ID that will be given to the next job added
    unsigned int next_resource_index;///< ID that will be given to the next resource added

    QTime startTime; ///< start time of the schedule, time representation of the job released first
    QTime timelineStartTime; ///< start time of the timeline
    bool useHoursOnTimeline; ///< bool to indicate whether time representation should be used on timeline
    int timeOffset; ///< horizontal offset used to paint jobs, tasks and resource profile

    int importOffset; ///< jobs and activities will be added with times relative to this offset
    bool importing; ///< set this to true to enable importing mode, index collisions will be avoided
    QMap<int, int> importResourceIndices; ///< maps old resource indices to new indices while importing an instance
    QMap<int, int> importJobIndices; ///< maps old job indices to new indices while importing an instance
    
    bool userChanges; ///< set this to true when user makes a change in the instance
    QString fileName; ///< if this instance was loaded from or saved to a file, this contains the filename

    QVector<Frame *> replayFrames; ///< vector containing all frames shown in the gui

    int prevTotalFlexibility; ///< The previous flexibility of the instance, useful for comparing new solutions.
    int totalFlexibility;   ///< The total flexibility of the current instance, calculated by the solver.
    
public:
    /** @return Constructor, initializes vectors. */
    Instance(QObject *parent = 0);

    /**
     * Destructor, deletes all objects created by the instance such as activities, jobs and resources.
     */
    ~Instance();

    /** @return The earliest job release date over all jobs. */
    int earliestReleaseDate() const;

    /** @return The latest job due date over all jobs. */
    int latestDueDate() const;

    /** @return the length of the timespan between the earliest release date and the latest due date. */
    int duration() const;

    /**
     * @return a string representation of this instance, including jobs, activities, precedences, resources and requirements, parseable by the solver.
     */
    QString toString() const;

    /** Dirty little function to force the instance to emit an instanceChanged() signal.
      */
    void emitChanged();

    /**
     * @return whether hours on timeline should be shown.
     */
    bool hoursOnTimeline();
    /**
     * Set whether hours on timeline should be shown.
     */
    void setHoursOnTimeline(bool b);
    /**
     * @return time offset for this instance
     */
    int getTimeOffset();
    /**
     * Emits signals saying this instance has been saved to disk.
     */
    void emitSaved();

    /**
     * Set import flag.
     */
    void setImportFlag(bool b);

    /**
     * Set import offset.
     * @param o integer representing the import offset
     */
    void setImportOffset(int o);

    /**
     * Clears all import flags, must be done before importing.
     */
    void clearImportMaps();

    /** Update start time of this instance.
     * @param t time
     */
    void updateStartTime();

    /** @return start time of this instance.
     */
    QTime getStartTime();

    /** @return start time of the timeline of this instance.
     */
    QTime getTimeLineStartTime();

    /**
     * Gives a specific frame.
     * @param frameNr The number of the requested frame.
     * @return A Frame object.
     */
    Frame* getFrame(int frameNr);

    /**
     * @return the highest frame number or -1 if there is no frame
     */
    int getMaxFrameNr();

    /**
     * Sets the frames for replaying. Replaces the previous frames.
     * @param frames a QVector of Frames
     */
    void setFrames(QVector<Frame *> frames);

    /**
     * Clears the frames for replaying.
     */
    void clearFrames();

    /** Add a resource to the instance, used by the parser.
     * @param i resource index
     * @param capacity capacity (C(i)) of new resource (R(i))
     * @param name name of new resource
     */
    Resource* addResource(unsigned int i, unsigned int capacity, QString name);

    /** Add a resource to the instance, used by the ui.
     * @param capacity capacity (C(i)) of new resource (R(i))
     * @param name name of new resource
     */
    void addResource(unsigned int capacity, QString name);

    /** Remove a resource from instance.
     * @param i resource index
     */
    void removeResource(unsigned int i);

    /** Check if instance contains resource with specified name and return it.
     * @param name name of the resource
     */
    Resource* containsResource(QString name);

    /** Add a job to the instance, used by the ui.
     * @param release_date release date of new job (T(i))
     * @param due_date due date of new job
     * @param name name of new job
     * @return the added job as an object
     */
    Job* addJob(unsigned int release_date, unsigned int due_date, QString name);

    /** Add a job to the instance, used by the parser.
     * @param i job index
     * @param release_date release date of new job (T(i))
     * @param due_date due date of new job
     * @param name name of new job
     * @return the added job as an object
     */
    Job* addJob(unsigned int i, unsigned int release_date, unsigned int due_date, QString name);

    /** Add a dummy job to the instance, used by the ui. A dummy job is used to create a resource decrease
	* @param release_date release date of new dummy job (T(i))
	* @param due_date due date of new dummy job
	* @param name name of new dummy job
	*/
    ResourceDecrease* addDummyJob(Resource*, unsigned int release_date, unsigned int due_date, int cap);

   /** Add a dummy job to the instance, used by the parser.
	* @param i job index
	* @param release_date release date of new job (T(i))
	* @param due_date due date of new job
	* @param name name of new job
	*/
    void addDummyJob(unsigned int i, unsigned int release_date, unsigned int due_date);

    /**
     * Merge two groups
     * @param i1 job of representative of group 1
     * @param j1 id of representative of group 1
     * @param i2 job of representative of group 2
     * @param j2 id of representative of group 2
     */
    void mergeGroup(unsigned int i1, unsigned int j1, unsigned int i2, unsigned int j2);

    /**
     * Checks if the new job ID is valid or if it conflicts with an existing job.
     * Also increments the id for a next job.
     * @param i The id for a new job.
     * @return A fixed job id, in case of importing.
     */
    int checkNewJobId(unsigned int i);

    /** Remove a job from instance
     * @param j job
     */
    void removeJob(Job *);

    /** Add an activity to the instance, used by the parser.
     * @param i job index
     * @param j activity index
     * @param est earliest start time in working solution, specify -1 if no est set, in this case
                  the release date of the corresponding job will be used
     * @param duration duration of new activity (A(i,j))
     * @param name name of new activity
     */
    void addActivity(unsigned int i, unsigned int j, int est, int lst, unsigned int duration, QString name, int st);
    void addActivity(Activity * a, int est, int lst, int st, bool locked);

    /** Add a requirement to the instance, used by the parser.
     * @param i job index
     * @param j activity index
     * @param k resource index
     * @param q amount of resource R(k) required by activity A(i,j)
     */
    void addRequirement(unsigned int i, unsigned int j, unsigned int k, unsigned int q);

    /**
     * Add a precedence constraint between a resource decrease and an activity
     * @param i1 id of the resource decrease
     * @param i2 job index
     * @param j2 activity index
     */
    void addDecreasePrecedence(unsigned int i1, unsigned int i2, unsigned int j2);

    /**
     * Remove a precedence constraint between a resource decrease and an activity
     * @param dec the resource decrease to be removed
     */
    void removeDecreasePrecedence(ResourceDecrease *dec);

    /** If precedence a1 < a2 exists, show it on frameNumber
     * @param a1 activity 1 object
     * @param a2 activity 2 object
     * @param frameNumber ID of a frame
     */
    void addPrecedenceFrame(Activity* a1, Activity* a2, int frameNumber);

    /** Add a precedence constraint to the instance, used by the parser.
     * @param i1 job 1 index
     * @param j1 activity 1 index
     * @param i2 job 2 index
     * @param j2 activity 2 index
     * @param hard true if this constraint is a <i>hard</i> constraint, and false if it is a <i>soft</i> constraint.
     * @param frameNummer frame number to which this constraint belongs (applies only to soft constraints).
     */
    void addPrecedence(unsigned int i1, unsigned int j1, unsigned int i2, unsigned int j2, bool hard, int frameNummer = 0);
    /** Add a precedence constraint to the instance.
     * @param a1 preceeding activity
     * @param a2 preceeded activity
     * @param hard true if this constraint is a <i>hard</i> constraint, and false if it is a <i>soft</i> constraint.
     * @param frameNumber frame number to which this constraint belongs (applies only to soft constraints).
     */
    void addPrecedence(Activity * a1, Activity * a2, bool hard, int frameNumber = 0);

    /** Merges groups where the specified activities belong to
     *  @param g vector containing two activities in job-task representation
     */
    void createGroup(Group * g);
    
    /**
     * Sets flexibility of an activity.
     * @param i job index
     * @param j activity index
     * @param flex added flexibility
     */
    void setFlex(unsigned int i, unsigned int j, unsigned int flex);

    /**
     * @return whether the instance was changed by the user since the last time solving
     */
    bool hasUserChanges();
    /**
     * set whether user changes have occured to the instance since the last time solving
     */
    void setUserChanges(bool);

    /**
     * @return the group where the given activity belongs to, or NULL if it is not grouped (with respect to the current frame).
     */
    Group* activityGroup(Activity *a);
    
    /** Changes partition in groups to the given one.
     */
    void setGroupPartition(QVector<Group *>);

    /** Adds job mutexes, called after parsing complete. We assume that only one
     * activity can be performed per job at any time. This is enforced by adding
     * a capacity-1 resource for every job and every activity is then forced to
     * require exclusive access to this resource. */
    void addJobMutexes();

    /**
     * removes the given precedence from this instance
     * @param p precedence
     */
    void removePrecedence(Precedence * p);

    /**
     * removes all precedences associated with the given activity
     * @param a the activity
     */
    void removePrecedences(Activity *a);

    /**
     * removes the precedence \f$a_1 \prec a_2\f$ from this instance
     * @param a1 preceeding activity
     * @param a2 preceeded activity
     */
    void removePrecedence(Activity *a1, Activity *a2);
    /**
     * removes the given activity \f$a_i\f$and all its requirements from this instance.
     * Precedence constraints are updated as follows: for each pair of an incoming constraint and an outgoing constraint with respect to \f$a_i\f$, add the corresponding transitive constraint to this instance. Then remove all constraints adjacent to \f$a_i\f$.
     * @param a activity
     */
    void removeActivity(Activity * a);
    
    /**
     * @return the precedence object representing \f$a_1 \prec a_2\f$ if it exists, or NULL otherwise.
     * @param a1 preceeding activity
     * @param a2 preceeded activity
     */
    Precedence* precedenceExists(Activity * a1, Activity * a2);
    /**
     * Searches for an <i>implied</i> precedence exists between two activities. An <i>implied</i> precedence from \f$a_1\f$ to \f$a_2\f$ is a chain of precedences starting at \f$a_1\f$ and ending at \f$a_2\f$.
     * @return whether an <i>implied</i> precedence \f$a_1 \prec \ldots \prec a_2\f$ exists.
     * @param a1 preceeding (source) activity
     * @param a2 preceeded (destination) activity
     */
    bool impliedPrecedenceExists(Activity * src, Activity * dst);

    /**
     * Remove all soft precedences that were added by the solver.
     */
    void clearSoftPrecedences();
    /**
     * @return whether the solver added any soft precedences.
     */
    bool hasSoftPrecedences();

    /**
     * Map from integer resource id to resource object.
     * The integer IDs are used during loading and saving.
     */
    QMap<int, Resource *> getResources();

    /**
     * Map from integer job id to job object.
     * The integer IDs are used during loading and saving.
     */
    QMap<int, Job *> getJobs();

    /**
     * Returns all hard precedence constraints.
     * @return vector of hard precedence constraints.
     */
    QVector<Precedence *> getHardPrecedences();

    /**
     * Returns all soft precedence constraints.
     * @return vector of soft precedence constraints.
     */
    QVector<Precedence *> getSoftPrecedences();

    /**
     * Returns all groups.
     * @return vector of groups.
     */
    QList<Group *> getGroups();
    
    /**
     * @return The complete path to the file which contains this instance.
     */
    QString getFileName();

    /**
     * Sets the filename for this instance, if it has been saved or loaded.
     * @param name The complete path to the file which contains this instance.
     */
    void setFileName(QString name);

    /**
     * Sets the total flexibility of this instance.
     * @param flex The total flexibility.
     */
    void setTotalFlex(int flex);

    /**
     * Gets the total flexibility of this instance.
     * @return integer representing the total flexibility.
     */
    int getTotalFlex();

    /**
     * Gets the previous flexibility of this instance.
     * @return integer representing the previous flexibility.
     */
    int getPrevFlex();

/* convenience functions */

    /** Get resource with id i.
     * @param i resource id
     */
    Resource * R(int i);

    /** Get job with id i.
     * @param i job id
     */
    Job *J(int i);

    /** Shorthand for pointer to \f$a_{ij}\f$.
     * @param i job id
     * @param j activity id
     */
    Activity *A(int i, int j);

private:
    /**
     * Remove a group from the instance.
     * @param g group to remove
     */
    void removeGroup(Group *g);
    /** Removes an activity from the group it belongs to.
      * @param a activity to remove from group
      */
    void removeActivityFromGroup(Activity * a);
signals:
    void fileNameChanged(const QString &); ///< emitted when loaded or when saved under different name
    void isModified(bool); ///< can be used in gui to show * in window title or not
    void instanceChanged(); ///< emitted when a UI update is needed

    void jobAdded(Job *); ///< emitted when a job is added
    void resourceAdded(Resource *); ///< emitted when a resource is added
    void userChange(); ///< emitted when the instance was changed by the user
};

#endif // INSTANCE_H
