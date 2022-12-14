#ifndef ACTIVITYINFODIALOG_H
#define ACTIVITYINFODIALOG_H

#include <QDialog>
#include <QLabel>
#include <QFormLayout>
#include <QDialogButtonBox>

#include "model/instance.h"

class Activity;
class RequirementsTable;

/**
 * Dialog for showing information about an activity
 */
class ActivityInfoDialog : public QDialog
{
    Q_OBJECT

    Activity *a;

    QFormLayout *formlayout;
    QLabel* nameLabel;
    QLabel* jobLabel;
    QLabel* durationLabel;
    QLabel* estLabel;
    QLabel* lstLabel;
    QLabel* stLabel;
    RequirementsTable *requirementsTable;
    QPushButton* editButton;
    QPushButton* closeButton;
    QDialogButtonBox* buttonBox;
    QVBoxLayout *boxlayout;

    void createLayout();
    void createSignals();

public:
    /**
     * Constructs dialog with activity information.
     * @param i instance
     * @param a activity
     * @param parent parent widget
     */
    explicit ActivityInfoDialog(Instance *i, Activity *a, QWidget *parent = 0);

private slots:
    void editActivity(); ///< shows the edit activity dialog
};

#endif // ACTIVITYINFODIALOG_H
