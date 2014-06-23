#include "jobinfodialog.h"
#include "jobdialog.h"
#include "widgets/app_icon.h"
#include "widgets/requirementstable.h"

#include <QLabel>
#include <QDateTimeEdit>
#include <QPushButton>

JobInfoDialog::JobInfoDialog(Instance *instance, Job *j, QWidget *parent) :
    QDialog(parent), j(j)
{
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Job info"));
    setWindowIcon(AppIcon("icon.png"));

    formlayout = new QFormLayout;
    nameLabel = new QLabel(j->name());
    releaseDayLabel = new QLabel(QString::number(j->releaseDate()/(60*24)+1));
    releaseDateTimeLabel = new QLabel(QTime(0,0,0).addSecs(j->releaseDate()*60).toString("h:mm"));
    dueDayLabel = new QLabel(QString::number(j->dueDate()/(60*24)+1));
    dueDateTimeLabel = new QLabel(QTime(0,0,0).addSecs(j->dueDate()*60).toString("h:mm"));
    releaseDateLabel = new QLabel(QString::number(j->releaseDate()));
    dueDateLabel = new QLabel(QString::number(j->dueDate()));
    actAmountLabel = new QLabel(QString::number(j->getActivities().size()));
    requirementsTable = new RequirementsTable(j, instance);
    editButton = new QPushButton(tr("Edit"),this);
    closeButton = new QPushButton(tr("Close"),this);
    buttonBox = new QDialogButtonBox(Qt::Horizontal);
    boxlayout = new QVBoxLayout;

    createLayout(instance);
    createSignals();
    setLayout(boxlayout);
}

void JobInfoDialog::createLayout(Instance *instance) {
    //labels
    nameLabel->setTextFormat(Qt::PlainText);
    formlayout->addRow(tr("Name:"), nameLabel);

    if(instance->hoursOnTimeline()) {
        formlayout->addRow(tr("Release Day:"), releaseDayLabel);
        formlayout->addRow(tr("Release Time:"), releaseDateTimeLabel);
        formlayout->addRow(tr("Due Day:"), dueDayLabel);
        formlayout->addRow(tr("Due Time:"), dueDateTimeLabel);
    }
    else {
        formlayout->addRow(tr("Release Date:"), releaseDateLabel);
        formlayout->addRow(tr("Due Date:"), dueDateLabel);
    }

    formlayout->addRow(tr("Activities:"), actAmountLabel);

    int totalflex = 0, minflex = INT_MAX;
    foreach(Activity *a, j->getActivities().values()) {
        if(a->estFlex() == -1 || a->lftFlex() == -1) {
            totalflex = -1;
            minflex = -1;
            break;
        }
        else {
            int flex = (a->lftFlex() - a->duration()) - a->estFlex();
            totalflex += flex;
            if(flex < minflex) minflex = flex;
        }
    }

    if(totalflex >= 0 && minflex >= 0) {
        totalFlexLabel = new QLabel(QString::number(totalflex));
        minFlexLabel = new QLabel(QString::number(minflex));
        formlayout->addRow(tr("Total Job Flexibility:"), totalFlexLabel);
        formlayout->addRow(tr("Minimal Activity Flexibility:"), minFlexLabel);
    }

    //table
    requirementsTable->fillIn();

    //buttons
    closeButton->setDefault(true);

    buttonBox->addButton(editButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(closeButton, QDialogButtonBox::ActionRole);

    // top layout
    boxlayout->addLayout(formlayout);
    boxlayout->addWidget(requirementsTable);
    boxlayout->addWidget(buttonBox);
}

void JobInfoDialog::createSignals() {
    connect(editButton, SIGNAL(clicked()), this, SLOT(editJob()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void JobInfoDialog::editJob(){
    accept();
    JobDialog dialog(j->instance(), j, this);
    dialog.exec();
}

