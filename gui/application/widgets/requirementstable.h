#ifndef REQUIREMENTSTABLE_H_
#define REQUIREMENTSTABLE_H_

#include <QTableWidget>
#include <QSpinBox>

#include "model/activitytemplate.h"

/**
 * A RequirementsTable is used for display or editing
 * the requirements of an activity or activity template.
 */
class RequirementsTable : public QTableWidget {
    Q_OBJECT

    Instance *instance;
    Activity *activity;
    ActivityTemplate *activityTemplate;

    QMap<Resource *, QSpinBox *> spinners;
    QWidget *editor;

public:

    /**
     * Creates a table.
     * @param a The activity that is edited.
     * @param instance The instance that contains the resources.
     */
    RequirementsTable(Activity *a, Instance *instance);

    /**
     * Creates a table.
     * @param instance The instance that contains the resources.
     */
    RequirementsTable(Instance *instance);
    void setActivityTemplate(ActivityTemplate *a);

    /**
     * Fills in the table with the requirements
     * from an activity or an activity template.
     */
    void fillIn();

    /**
     * Sets the table to be editable.
     * @param widget The parent widget, which the signals valueChanged() will be slotted to.
     */
    void setEditableBy(QWidget* widget);

    /**
     * Import a template for editing an activity.
     * This can only be called after the requirements table
     * has been setup for editing an activity.
     *
     * @param import The activity template.
     */
    void importTemplate(ActivityTemplate *import);

    /**
     * Clears all spinners.
     */
    void clearSpinners();

    /**
     * Apply the requirements from the table to the activity or template.
     */
    void applyToActivity();

private:
    void init();
    int getRequiredAmount(Resource *r);
};


#endif /* REQUIREMENTSTABLE_H_ */
