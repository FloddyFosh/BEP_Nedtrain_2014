#ifndef JOBDIALOG_H
#define JOBDIALOG_H

#include <QLineEdit>
#include <QSpinBox>

#include "model/instance.h"
#include "widgets/dialogs/formdialog.h"

class Job;

/**
 * Dialog for creating or editing jobs. Pass a NULL pointer for the job object
 * to add a new job. All processing is done in the apply() slot and outside of this dialog
 * the developer does not have to worry about updating the job. The UI should be
 * updated based on the signals emitted by the instance and job object.
 */
class JobDialog : public FormDialog
{
    Q_OBJECT

    Instance *i;
    Job *j;

    QLineEdit *nameEdit;
    QSpinBox *releaseDateEdit;
    QSpinBox *dueDateEdit;

    QDateTimeEdit *releaseDateTimeEdit;
    QDateTimeEdit *dueDateTimeEdit;
    QSpinBox *releaseDayEdit;
    QSpinBox *dueDayEdit;

    int maxActVal;

public:
    /** Constructor. Pass an instance object and a job that belongs to this instance
      * or an instance object and a NULL pointer.
      * @param instance instance
      * @param job job
      * @param parent parent widget
      */
    explicit JobDialog(Instance *instance, Job *job, QWidget *parent = 0);

private slots:
    void apply(); ///< Applies the changes to the job. If a NULL pointer was passed, a new job is added to the instance. Otherwise, the changes are applied to the passed job object.
};

#endif // JOBDIALOG_H
