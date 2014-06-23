#ifndef JOB_H
#define JOB_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>

class Instance;
class Activity;
class Group;
class Resource;

/** Data container for integer start-end (time) interval.
  */
struct Interval {
    int start; ///< beginning of interval
    int end; ///< end of interval
    bool overlaps(Interval &other); ///< returns true iff the intervals overlap
    bool touch(Interval &other); ///< returns true iff the intervals touch
};

/** Job definition. A job has a release date and a due date in between
 * which all of its activities must be processed.
 */
class Job : public QObject {
    Q_OBJECT

    int _id; ///< job id (as used when saved to file and sent to solvers)
    QString _name; ///< name of the job
    int _release_date; ///< release date of job. Activities can start no earlier than this time.
    int _due_date; ///< due date of job. Activities must finish no later than this time.
    QList<Group *> groups; ///< group of activities existing in this group.
    Instance *_instance; ///< pointer to the instance this job belongs to

    QMap<int, Activity*> activities;
public:
    /** Constructor used when reading job from file.
      */
    Job(Instance *instance, int id, QString name, int release_date, int due_date);

    /** Constructor used when creating new job from GUI.
      */
    Job();

    /** Used when adding a group from solver.
      */
    void addGroup(Group *);

    /** Used when removing a group from solver.
      */
    void removeGroup(Group *);

    /** @return a list of all groups within this job in the current frame.
      */
    QList<Group *> getGroups();
    
    /**
     * clears al groups.
     */
    void clearGroups();

    /** Used when adding an activity from the GUI. Emits the appropriate signals.
      */
    void addActivity(Activity *);

    /** Used when removing an activity from the GUI. Emits the appropriate signals.
      */
    void removeActivity(Activity *);

    /**
     * Returns all activities belonging to this job.
     * @return Hash map of activities. The keys are the activity IDs, the values the activity pointers.
     */
    QMap<int, Activity*> getActivities();

    /** Returns true iff the job has activities.
      */
    bool hasActivities();

    /** Returns the first unused activity id for this job.
      */
    int findAvailableActivityId();

    /**
     * Returns the amount of capacity of a given resource required by all activities in this job.
     * @param resource resource in question
     * @return required amount
     */
    int getRequiredAmount(Resource *resource);

    int id() const; ///< returns the job id
    QString name() const; ///< returns the job name
    int releaseDate() const; ///< returns the job release date (int), activities can not start before this date/time
    int dueDate() const; ///< returns the job due date (int), all the activities must be completed by this date/time
    int duration(); ///< returns the job duration time (int).
    Instance *instance() const; ///< returns the instance which this job belongs to

    void setName(QString name); ///< name mutator method used from GUI
    void setReleaseDate(int rd); ///< release date mutator method used from GUI
    void setDueDate(int dd); ///< due date mutator method used from GUI
    /**
     * @return a string representation of this job, including all its activities, parseable by the solver.
     */
    QString toString();

    /**
     * Compare jobs by id, name and dates.
     * @return true if the jobs are the same, false otherwise
     */
    bool operator==(const Job &other) const;

    /**
     * @return the number of groups
     */
    int groupCount();
signals:
    void jobChanged(); ///< emitted whenever this job changes its state
    void activityAdded(Activity *); ///< emitted when an activity is added to this job
    void activityRemoved(); ///< emitted when an activity is being removed.
    void groupAdded(Group *); ///< emitted when a group is added.
    void groupRemoved(Group *g); ///< emitted when a group is removed.
};

#endif // JOB_H
