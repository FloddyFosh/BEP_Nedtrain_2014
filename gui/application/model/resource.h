#ifndef RESOURCE_H
#define RESOURCE_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>

class Activity;
class Chain;
class Instance;
class Requirement;
class ResourceDecrease;

/** Resource definition. A resource has a constant capacity, a name and a list
 * of requirements. The requirements supply a list of activities that need to
 * use this resource. Instead of keeping a list of activities, we keep a list
 * of requirements. By keeping a list of requirements, we know not only what
 * activities use the resource, but also how much of the capacity they require.
 */
class Resource : public QObject
{
    Q_OBJECT

    Instance *_instance;
    int _id;
    QString _name;
    int _capacity;
    QVector<Requirement*> requirements;
    QMap<int, ResourceDecrease*> decreases;
    QMap<int,Chain*> chains;
public:
    /** Constructor to construct a complete resource object.
      * @param instance pointer to the instance object this resource belongs to
      * @param id integer id of this resource
      * @param name name of the resource
      * @param capacity integer capacity of the resource, constant through time, no support for variable resource capacity yet
      * @param parent unused
      */
    Resource(Instance *instance, int id, QString name, int capacity, QObject *parent = 0);

    /** Constructor to construct dummy resource object for testing.
     * @param name name of the resource
     */
    Resource(QString name);

    int id(); ///< returns the id of this resource
    Instance* instance(); ///< returns a pointer to the instance belonging to this resource
    QString name(); ///< returns the name of this resource
    int capacity(); ///< returns the capacity of this resource

    void setName(QString name); ///< name mutator method used from GUI
    void setCapacity(int capacity); ///< capacity mutator method used from GUI

    /**
     * Returns all requirements (activities that require this resource).
     * @return a QVector of Requirement objects
     */
    QVector<Requirement*> getRequirements();

    /**
     * @return The chain object associated with chainId
     */
    Chain* getChain(int chainId);

    /**
     * @return The map that contains all chain objects
     */
    QMap<int,Chain*> getChains();

    /**
     * Clears the saved chains of this resource.
     */
    void clearChains();

    /**
     * @param act Activity that will be added to following chain.
     * @param chain Chain that was selected by chaining algorithm.
     */
    void addActToChain(Activity *act, int chainId);

    /**
     * @param chainId id of the resource unit of which the chain will be initialized
     */
    void initializeChain(int chainId);

    /** Add an activity that requires this resource. The parameter is a Requirement pointer
      * that has an activity pointer and an integer amount. This method is used when an
      * activity is created through the GUI.
      *
      * @param req pointer to the requirement object that is to be added
      */
    void addActivity(Requirement *req);
    /**
     * Remove a requirement of this resource. The parameter is a Requirement pointer
     * that has an activity pointer and an integer amount. 
     * 
     * @param req pointer to the requirement object that is to be added
     */
    void removeActivity(Requirement *req);
    /**
     * @return whether any requirement of an activity for this resource exists.
     */
    bool hasActivities();
    /**
     * @return a list of all ResourceDecrease objects belonging to this resource.
     */
    QMap<int, ResourceDecrease*> getDecreases();

    /**
     * Returns the resource decrease with specified id.
     * @param id the id
     * @return resourcedecrease with id given
     */
    ResourceDecrease *getDecrease(int id);

    /**
     * Adds a ResourceDecrease object to this resource.
     */
    void addDecrease(ResourceDecrease*);

    /**
     * Removes a ResourceDecrease object from this resource.
     */
    void removeDecrease(ResourceDecrease*);

    /**
     * @return a string representation of this resource, parseable by the solver.
     */
    QString toString();

    /**
     * Compare resources by id, name and capacity.
     * @return true if the resources are the same, false otherwise
     */
    bool operator ==(const Resource &other) const;

signals:
    void activityAdded(Activity *a); ///< emitted when a requirement is added @param a the activity in the requirement
    void resourceChanged(); ///< emitted when this resource changes state in any way
};


#endif // RESOURCE_H
