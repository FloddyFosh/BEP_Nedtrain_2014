#ifndef ACTIVITYINFODIALOG_H
#define ACTIVITYINFODIALOG_H

#include <QDialog>

#include "model/instance.h"

class Activity;

/**
 * Dialog for showing information about an activity
 */
class ActivityInfoDialog : public QDialog
{
    Q_OBJECT

    Activity *a;

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
