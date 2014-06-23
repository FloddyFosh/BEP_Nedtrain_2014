#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDateTimeEdit>
#include <QMessageBox>

#include "resourcedecreasedialog.h"
#include "widgets/app_icon.h"

ResourceDecreaseDialog::ResourceDecreaseDialog(Instance *i, Resource* res, QWidget *parent) :
        QDialog(parent), resource(res), instance(i) {
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Decrease capacity"));
    setWindowIcon(AppIcon("icon.png"));

    capacityEdit = new QSpinBox;
    formlayout = new QFormLayout;
    fromDateTimeEdit = new QDateTimeEdit;
    toDateTimeEdit = new QDateTimeEdit;
    fromDayEdit = new QSpinBox;
    toDayEdit = new QSpinBox;
    fromEdit = new QSpinBox;
    tillEdit = new QSpinBox;
    buttonbox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    boxlayout = new QVBoxLayout;

    createLayout(instance, res);
    createSignals();
}

void ResourceDecreaseDialog::createLayout(Instance *instance, Resource *res) {
    capacityEdit->setMinimum(1);
    capacityEdit->setMaximum(res->capacity());
    capacityEdit->setValue(1);

    if(instance->hoursOnTimeline()){
        fromDateTimeEdit->setDisplayFormat("h:mm");
        toDateTimeEdit->setDisplayFormat("h:mm");

        fromDayEdit->setMinimum(1);
        fromDayEdit->setMaximum(INT_MAX);

        toDayEdit->setMinimum(1);
        toDayEdit->setMaximum(INT_MAX);

        formlayout->addRow(tr("From - day:"), fromDayEdit);
        formlayout->addRow(tr("From - time:"), fromDateTimeEdit);
        formlayout->addRow(tr("Till - day:"), toDayEdit);
        formlayout->addRow(tr("Till - time:"), toDateTimeEdit);
        formlayout->addRow(tr("Decrease by:"), capacityEdit);
    }
    else{
        fromEdit->setRange(0, INT_MAX);
        fromEdit->setValue(0);

        tillEdit->setRange(1, INT_MAX);
        tillEdit->setValue(1);

        formlayout->addRow(tr("From:"), fromEdit);
        formlayout->addRow(tr("Till:"), tillEdit);
        formlayout->addRow(tr("Decrease by:"), capacityEdit);
    }

    boxlayout->addLayout(formlayout);
    boxlayout->addWidget(buttonbox);

    setLayout(boxlayout);
}

void ResourceDecreaseDialog::createSignals() {
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

    if(till < from) {
        QMessageBox::information(this, tr("Invalid input"), tr("The till date should be after the from date.\nPlease correct your input."));
    }
    else {
        int cap = capacityEdit->value();
        instance->addDummyJob(resource, from, till, cap);
        emit decreaseAdded();

        resource->instance()->setUserChanges(true);
        accept();
    }
}
