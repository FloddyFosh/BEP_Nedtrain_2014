#ifndef FLEXIBILITY_H
#define FLEXIBILITY_H

#include <QSpinBox>

class InstanceController;
#include "model/activity.h"
#include "widgets/dialogs/formdialog.h"

class FlexibilityDialog : public FormDialog
{
    Q_OBJECT

    InstanceController *controller;
    Activity *activity;
    QSpinBox *flexibility;

    void createLayout();

public:
    /** Constructor.
      * @param activity The activity whose flexibility is edited.
      * @param parent parent widget
      */
    explicit FlexibilityDialog(InstanceController *controller, Activity *activity, QWidget *parent = 0);

private slots:
    /**
     * Applies the flexibility to the activity.
     */
    void apply();
};

#endif // FLEXIBILITY_H
