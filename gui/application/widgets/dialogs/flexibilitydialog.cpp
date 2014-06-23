#include "flexibilitydialog.h"

#include "controller/instancecontroller.h"
#include "model/instance.h"

FlexibilityDialog::FlexibilityDialog(InstanceController *controller, Activity *activity, QWidget *parent) :
    FormDialog(parent), controller(controller), activity(activity)
{
    setWindowTitle(tr("Set flexibility"));

    flexibility = new QSpinBox;

    createLayout();
    setUpLayout();
}

void FlexibilityDialog::createLayout() {
    flexibility->setSuffix(tr(" time units"));
    flexibility->setMaximum(activity->job()->dueDate() - activity->job()->releaseDate() - activity->duration());
    flexibility->setValue(activity->getFlex());

    addFormField(new QLabel(tr("You can add flexibility to this activity by specifying how many additional"
            "                   \ntime units are reserved, in case an activity overruns its time.")));
    addFormField(tr("Flexibility:"), flexibility);
}

void FlexibilityDialog::apply() {
    activity->setFlex(flexibility->value());
    controller->getInstance()->setUserChanges(true);
    accept();
}
