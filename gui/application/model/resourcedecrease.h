#ifndef RESOURCEDECREASE_H_
#define RESOURCEDECREASE_H_

#include <QString>
#include <QObject>

class Resource;

/**
 * A ResourceDecrease is a temporary decrease of the capacity of a resource, on a
 * specific interval. The capacity of the resource is decreased by the amount of
 * the associated ResourceDecrease.
 */
class ResourceDecrease : public QObject{
	Q_OBJECT

    Resource* resource;
    int start, end, amount, id;
public:
    /**
     * Constructor for a resource decrease object. The corresponding Resource cannot be set via the constructor because of the format in which instances are saved.
     * @param start starting time
     * @param end ending time
     * @param amount decrease amount
     * @param id id
     */
	ResourceDecrease(int start, int end, int amount, int id, QObject *parent = 0);
	/**
	 * @return the starting time of this resource decrease
	 */
	int getFrom();
	/**
	 * @return the ending time of this resource decrease
	 */
	int getTill();
	/**
	 * @return the decrease amount
	 */
	int getAmount();
	/**
	 * @return the job ID for this resource decrease object.
	 */
	int getId();
	/**
	 * @return the ID of the corresponding Resource.
	 */
	int getResourceId();
	/**
	 * sets the decrease amount to the given value.
	 */
	void setAmount(int);
	/**
	 * sets the corresponding Resource to the given Resource.
	 * this cannot be done via the constructor because of the format in which instances are saved.
	 */
	void setResource(Resource * r);
    /**
     * @return a string representation of this resource decrease, parseable by the solver.
     */
	QString toString();
};

#endif /* RESOURCEDECREASE_H_ */
