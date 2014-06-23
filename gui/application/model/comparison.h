#ifndef COMPARISON_H_
#define COMPARISON_H_

#include <QObject>

class Activity;
class Instance;

/**
 * This class provides methods to compare two instances, based on the start
 * times of their activities. It is used in the comparison window to determine
 * which activities have to be painted in grayscale.
 */
class Comparison : public QObject {
    Q_OBJECT

    Instance *first, *second;

public:
    /**
     * Creates a comparison between two instances.
     * @param first The first instance.
     * @param second The second instance.
     */
    Comparison(Instance *first, Instance *second);
    Instance* getFirstInstance(); ///< returns the first instance
    Instance* getSecondInstance(); ///< returns the second instance

    /**
     * Checks if an activity is different between the instances with respect to start times.
     * @param a The activity that should be checked.
     * @return true if the activity has differences, false otherwise
     */
    bool hasDifferences(Activity *a);

private:
    /**
     * Checks whether the instances have the same jobs.
     * If that is not the case, then a comparison is not possible.
     * @return true if the jobs are the same, false otherwise
     */
    bool haveSameJobs();

    /**
     * Checks whether the instances have the same activities.
     * If that is not the case, then a comparison is not possible.
     * @return true if the jobs are the same, false otherwise
     */
    bool haveSameActivities();

    /**
     * Checks whether the instances have the same resources.
     * If that is not the case, then a comparison is not possible.
     * @return true if the jobs are the same, false otherwise
     */
    bool haveSameResources();
};


#endif /* COMPARISON_H_ */
