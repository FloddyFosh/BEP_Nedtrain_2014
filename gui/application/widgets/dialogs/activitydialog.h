#ifndef ACTIVITYDIALOG_H
#define ACTIVITYDIALOG_H

#include <QComboBox>

#include "widgets/requirementstable.h"
#include "widgets/dialogs/formdialog.h"

class Activity;

/**
 * Dialog for creating or editing activities. If the activity object that is passed
 * in the constructor has no job pointer set, then it is considered a new activity.
 * In this case the job selection dropdown is shown and in the apply() slot, the job
 * link is set. All processing is done in the apply() slot and outside of this dialog
 * the developer does not have to worry about updating the activity. The UI should be
 * updated based on the signals emitted by the activity, job and instance objects.
 */
class ActivityDialog : public FormDialog
{
    Q_OBJECT

    Activity *a;
    Instance *instance;

    QComboBox *jobEdit;
    QLineEdit *nameEdit;
    QSpinBox *durationEdit;
    RequirementsTable *requirementsTable;
    QComboBox *templateList;

    bool usingTemplate;

public:
    /**
     * Constructor. Pass an instance object and an activity that belongs to a job of this instance
     * or an instance object and a new activity object. The activity object will be linked to a job
     * in the apply() method.
     */
    explicit ActivityDialog(Instance *, Activity *, QWidget *parent = 0);

private slots:
    void apply(); ///< Applies the changes to the activity object. If the activity object is new, it is added to the job object selected in the job dropdown.
    void templateChanged(int); ///< fills in the form fields when a template is chosen
    void setModified(); ///< clears the template field if modifications are done
};

#endif // ACTIVITYDIALOG_H
