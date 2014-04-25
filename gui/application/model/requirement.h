#ifndef REQUIREMENT_H_
#define REQUIREMENT_H_

#include <QObject>

class Activity;
class Resource;

/**
 * Resource requirement. An activity may require the use of a constant amount
 * of capacity from a set of resources. This struct records those requirements.
 */
class Requirement : public QObject {
    Q_OBJECT

    Activity *_activity;
    Resource *_resource;
    int _amount;
public:
    /**
     * Constructor.
     * @param a the activity that is in need of some resource capacity
     * @param r the resource that the activity requires
     * @param amount the integer amount of the resource that is needed by the activity
     */
    Requirement(Activity *a, Resource *r, int amount);

    Activity* activity(); ///< returns the activity belonging to this requirement
    Resource* resource(); ///< returns the resource belonging to this requirement
    int amount(); ///< returns the amount belonging to this requirement

    void setAmount(int amount);  ///< sets the amount

    /**
     * Compare by earliest start time of the corresponding activities.
     * @return comparison result
     */
    bool operator <(const Requirement &other) const;

    /**
     * @return a string representation of this requirement, parseable by the solver.
     */
    QString toString();
};

#endif /* REQUIREMENT_H_ */
