#include "activityinfodialog.h"
#include "activitydialog.h"

#include <QLabel>
#include <QPushButton>

ActivityInfoDialog::ActivityInfoDialog(Instance *instance, Activity *a, QWidget *parent) :
    QDialog(parent), a(a)
{
    setWindowTitle(tr("Activity info"));
    QFormLayout *formlayout = new QFormLayout;

    //labels
    QLabel* nameLabel = new QLabel(a->name());
    formlayout->addRow(tr("Name:"), nameLabel);
	QLabel* jobLabel = new QLabel(a->job()->name());
    formlayout->addRow(tr("Job:"), jobLabel);
    QLabel* durationLabel = new QLabel(QString::number(a->duration()));
    formlayout->addRow(tr("Duration:"), durationLabel);
    QLabel* estLabel = new QLabel(QString::number(a->est()));
    formlayout->addRow(tr("Earliest start time:"), estLabel);
    QLabel* lstLabel = new QLabel(QString::number(a->lst()));
    formlayout->addRow(tr("Latest start time:"), lstLabel);
    if(a->estFlex() != -1 && a->lftFlex() != -1) {
        QLabel* eftLabel = new QLabel(QString::number(a->estFlex()));
        formlayout->addRow(tr("Earliest flex start time:"), eftLabel);
        QLabel* lftLabel = new QLabel(QString::number(a->lftFlex()-a->duration()));
        formlayout->addRow(tr("Latest flex start time:"), lftLabel);
    }
    QLabel* stLabel = new QLabel(QString::number(a->st()));
    formlayout->addRow(tr("Start time:"), stLabel);

    //table
    RequirementsTable *requirementsTable = new RequirementsTable(a, instance);
    requirementsTable->fillIn();

	//buttons
	QPushButton* editButton = new QPushButton(tr("Edit"),this);
	QPushButton* closeButton = new QPushButton(tr("Close"),this);
	closeButton->setDefault(true);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
	buttonBox->addButton(editButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(closeButton, QDialogButtonBox::ActionRole);

	// top layout
	QVBoxLayout *boxlayout = new QVBoxLayout;
	boxlayout->addLayout(formlayout);
	boxlayout->addWidget(requirementsTable);
	boxlayout->addWidget(buttonBox);

	// signals
	connect(editButton, SIGNAL(clicked()), this, SLOT(editActivity()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));

	setLayout(boxlayout);
}

void ActivityInfoDialog::editActivity(){
	accept();
	ActivityDialog dialog(a->job()->instance(), a, this);
	dialog.exec();
}
