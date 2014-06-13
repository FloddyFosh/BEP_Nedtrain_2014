#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateTimeEdit>

#include "resourcedecreasedialog.h"
#include "widgets/app_icon.h"

ResourceDecreaseDialog::ResourceDecreaseDialog(Instance *i, Resource* res, QWidget *parent) :
        QDialog(parent), resource(res), instance(i) {
    setWindowTitle(tr("Decrease capacity"));
    setWindowIcon(AppIcon("icon.png"));

    capacityEdit = new QSpinBox;
    capacityEdit->setMinimum(1);
    capacityEdit->setMaximum(res->capacity());
    capacityEdit->setValue(1);

    QFormLayout *formlayout = new QFormLayout;

    if(instance->hoursOnTimeline()){
        fromDateTimeEdit = new QDateTimeEdit;
        fromDateTimeEdit->setDisplayFormat("h:mm");

        toDateTimeEdit = new QDateTimeEdit;
        toDateTimeEdit->setDisplayFormat("h:mm");

        fromDayEdit = new QSpinBox;
        fromDayEdit->setMinimum(1);
        fromDayEdit->setMaximum(1 << 30);

        toDayEdit = new QSpinBox;
        toDayEdit->setMinimum(1);
        toDayEdit->setMaximum(1 << 30);

        formlayout->addRow(tr("From - day:"), fromDayEdit);
        formlayout->addRow(tr("From - time:"), fromDateTimeEdit);
        formlayout->addRow(tr("Till - day:"), toDayEdit);
        formlayout->addRow(tr("Till - time:"), toDateTimeEdit);
        formlayout->addRow(tr("Decrease by:"), capacityEdit);
    }
    else{
        fromEdit = new QSpinBox;
        fromEdit->setRange(0, INT_MAX);
        fromEdit->setValue(0);

        tillEdit = new QSpinBox;
        tillEdit->setRange(1, INT_MAX);
        tillEdit->setValue(1);

        formlayout->addRow(tr("From:"), fromEdit);
        formlayout->addRow(tr("Till:"), tillEdit);
        formlayout->addRow(tr("Decrease by:"), capacityEdit);
    }

    QDialogButtonBox *buttonbox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QVBoxLayout *boxlayout = new QVBoxLayout;
    boxlayout->addLayout(formlayout);
    boxlayout->addWidget(buttonbox);

    setLayout(boxlayout);

    connect(buttonbox, SIGNAL(accepted()), this, SLOT(apply()));
    connect(buttonbox, SIGNAL(rejected()), this, SLOT(reject()));

}

void ResourceDecreaseDialog::apply() {
    int from, till;

    if(instance->hoursOnTimeline()){
        QTime release = fromDateTimeEdit->time();
        from = QTime(0,0,0).secsTo(release)/60 + ((fromDayEdit->value()-1)*60*24);

        QTime due = toDateTimeEdit->time();
        till = QTime(0,0,0).secsTo(due)/60 + ((toDayEdit->value()-1)*60*24);
    }
    else{
        from = fromEdit->value();
        till = tillEdit->value();
    }

    int cap = capacityEdit->value();
    instance->addDummyJob(resource, from, till, cap);
    emit decreaseAdded();

    resource->instance()->setUserChanges(true);
    accept();
}
