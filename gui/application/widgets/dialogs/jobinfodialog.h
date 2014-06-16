#ifndef JOBINFODIALOG_H
#define JOBINFODIALOG_H

#include <QDialog>
#include "model/instance.h"

class Job;

/**
 * Dialog for showing information about a job
 */
class JobInfoDialog : public QDialog
{
    Q_OBJECT

    Job *j;

public:
    /**
     * Constructs dialog with job information.
     * @param i instance
     * @param j job
     * @param parent parent widget
     */
    explicit JobInfoDialog(Instance *i, Job *j, QWidget *parent = 0);

private slots:
    void editJob(); ///< shows the edit job dialog
};

#endif // JOBINFODIALOG_H
