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
    setWindowTitle(tr("Job info"));
    setWindowIcon(AppIcon("icon.png"));
    QFormLayout *formlayout = new QFormLayout;

    //labels
    QLabel* nameLabel = new QLabel(j->name());
    nameLabel->setTextFormat(Qt::PlainText);
    formlayout->addRow(tr("Name:"), nameLabel);

    if(instance->hoursOnTimeline()) {
        QLabel* releaseDayLabel = new QLabel(QString::number(j->releaseDate()/(60*24)+1));
        formlayout->addRow(tr("Release Day:"), releaseDayLabel);

        QLabel* releaseDateTimeLabel = new QLabel(QTime(0,0,0).addSecs(j->releaseDate()*60).toString("h:mm"));
        formlayout->addRow(tr("Release Time:"), releaseDateTimeLabel);

        QLabel* dueDayLabel = new QLabel(QString::number(j->dueDate()/(60*24)+1));
        formlayout->addRow(tr("Due Day:"), dueDayLabel);

        QLabel* dueDateTimeLabel = new QLabel(QTime(0,0,0).addSecs(j->dueDate()*60).toString("h:mm"));
        formlayout->addRow(tr("Due Time:"), dueDateTimeLabel);
    }
    else {
        QLabel* releaseDateLabel = new QLabel(QString::number(j->releaseDate()));
        formlayout->addRow(tr("Release Date:"), releaseDateLabel);

        QLabel* dueDateLabel = new QLabel(QString::number(j->dueDate()));
        formlayout->addRow(tr("Due Date:"), dueDateLabel);
    }

    QLabel* actAmountLabel = new QLabel(QString::number(j->getActivities().size()));
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
        QLabel* totalFlexLabel = new QLabel(QString::number(totalflex));
        formlayout->addRow(tr("Total Job Flexibility:"), totalFlexLabel);

        QLabel* minFlexLabel = new QLabel(QString::number(minflex));
        formlayout->addRow(tr("Minimal Activity Flexibility:"), minFlexLabel);
    }

    //table
    RequirementsTable *requirementsTable = new RequirementsTable(j, instance);
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
    connect(editButton, SIGNAL(clicked()), this, SLOT(editJob()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));

    setLayout(boxlayout);
}

void JobInfoDialog::editJob(){
    accept();
    JobDialog dialog(j->instance(), j, this);
    dialog.exec();
}

