#ifndef ACTIVITYTEMPLATE_H_
#define ACTIVITYTEMPLATE_H_

#include <QObject>
#include <QString>
#include <QMap>

class Instance;

/**
 * An activity template is a predefined activity with associated duration and
 * set of resource requirements.
 */
class ActivityTemplate : public QObject {
    Q_OBJECT

    int id, duration; ///< id of the activity template
    QString name; ///< name of the activity
    QMap<QString, int> requirements; ///< set of resources and demand required by activity.

public:
    /**
     * Constructor to make an activity template that already exists in db.
     * @param i id
     * @param n name
     * @param d duration
     */
    ActivityTemplate(int i, QString n, int d);

    /**
     * Constructor to make an activity template that does not exist in db.
     * @param n name
     * @param d duration
     */
    ActivityTemplate(QString n, int d);

    /**
     * Get id of activity template.
     */
    int getID() const;

    /**
     * Get name of activity template.
     */
    QString getName() const;

    /**
     * Get duration of activity template.
     */
    int getDuration() const;

    /**
     * Set id of activity template.
     * @param i id
     */
    void setID(int i);

    /**
     * Set name of activity template.
     * @param n name
     */
    void setName(QString n);

    /**
     * Set duration of activity template.
     * @param d duration
     */
    void setDuration(int d);

    /**
     * Add requirement to the template.
     * @param n name
     * @param d duration
     */
    void addRequirement(QString n, int d);

    /**
     * Add resource requirements to the template.
     * @param req resource requirements
     */
    void setRequirements(QMap<QString, int> req);

    /**
     * Get resource requirements.
     */
    QMap<QString, int> getRequirements() const;

    /**
     * Find a requirement for a resource and return the requirement amount.
     * @param name THe name of a resource.
     * @return The required amount, if it exists, or 0.
     */
    int getRequiredAmount(QString name);

    /**
     * Remove requirement with specified name.
     * @param n name
     */
    void removeRequirement(QString n);

    /**
     * Checks if this template is compatible with
     * resources of an instance.
     * @param instance An instance that is to be checked
     */
    bool isCompatibleWith(Instance *);

    /**
     * Returns formatted string with names of all resources
     */
    QString getResourceNames();
};

#endif
