#ifndef JOBINFODIALOG_H
#define JOBINFODIALOG_H

#include <QDialog>
#include <QLabel>
#include <QFormLayout>
#include <QDialogButtonBox>

#include "model/instance.h"

class Job;
class RequirementsTable;

/**
 * Dialog for showing information about a job
 */
class JobInfoDialog : public QDialog
{
    Q_OBJECT

    QFormLayout *formlayout;
    QLabel* nameLabel;
    QLabel* releaseDayLabel;
    QLabel* releaseDateTimeLabel;
    QLabel* dueDayLabel;
    QLabel* dueDateTimeLabel;
    QLabel* releaseDateLabel;
    QLabel* dueDateLabel;
    QLabel* actAmountLabel;
    QLabel* totalFlexLabel;
    QLabel* minFlexLabel;
    RequirementsTable *requirementsTable;
    QPushButton* editButton;
    QPushButton* closeButton;
    QDialogButtonBox* buttonBox;
    QVBoxLayout *boxlayout;

    Job *j;

    void createLayout(Instance *instance);
    void createSignals();

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
