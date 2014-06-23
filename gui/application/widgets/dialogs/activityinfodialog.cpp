#include "activityinfodialog.h"
#include "activitydialog.h"
#include "widgets/app_icon.h"

#include <QLabel>
#include <QPushButton>

ActivityInfoDialog::ActivityInfoDialog(Instance *instance, Activity *a, QWidget *parent) :
    QDialog(parent), a(a)
{
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Activity info"));
    setWindowIcon(AppIcon("icon.png"));

    formlayout = new QFormLayout;
    nameLabel = new QLabel(a->name());
    jobLabel = new QLabel(a->job()->name());
    durationLabel = new QLabel(QString::number(a->duration()));
    estLabel = new QLabel(QString::number(a->est()));
    lstLabel = new QLabel(QString::number(a->lst()));
    stLabel = new QLabel(QString::number(a->st()));
    requirementsTable = new RequirementsTable(a, instance);
    editButton = new QPushButton(tr("Edit"),this);
    closeButton = new QPushButton(tr("Close"),this);
    buttonBox = new QDialogButtonBox(Qt::Horizontal);
    boxlayout = new QVBoxLayout;

    createLayout();
    createSignals();
	setLayout(boxlayout);
}

void ActivityInfoDialog::createLayout() {
    //labels
    nameLabel->setTextFormat(Qt::PlainText);
    formlayout->addRow(tr("Name:"), nameLabel);
    jobLabel->setTextFormat(Qt::PlainText);
    formlayout->addRow(tr("Job:"), jobLabel);
    formlayout->addRow(tr("Duration:"), durationLabel);
    formlayout->addRow(tr("Earliest start time:"), estLabel);
    formlayout->addRow(tr("Latest start time:"), lstLabel);

    if(a->estFlex() != -1 && a->lftFlex() != -1) {
        QLabel* eftLabel = new QLabel(QString::number(a->estFlex()));
        formlayout->addRow(tr("Earliest flex start time:"), eftLabel);
        QLabel* lftLabel = new QLabel(QString::number(a->lftFlex()-a->duration()));
        formlayout->addRow(tr("Latest flex start time:"), lftLabel);
    }

    formlayout->addRow(tr("Start time:"), stLabel);

    //table
    requirementsTable->fillIn();

    //buttons
    closeButton->setDefault(true);

    buttonBox->addButton(editButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(closeButton, QDialogButtonBox::ActionRole);

    //top layout
    boxlayout->addLayout(formlayout);
    boxlayout->addWidget(requirementsTable);
    boxlayout->addWidget(buttonBox);
}

void ActivityInfoDialog::createSignals() {
    connect(editButton, SIGNAL(clicked()), this, SLOT(editActivity()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void ActivityInfoDialog::editActivity(){
	accept();
	ActivityDialog dialog(a->job()->instance(), a, this);
	dialog.exec();
}
