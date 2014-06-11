#include "activitydialog.h"
#include "data/template_gateway.h"

#include <QMessageBox>
#include <QLineEdit>

ActivityDialog::ActivityDialog(Instance *instance, Activity *a, QWidget *parent) :
    FormDialog(parent), a(a), instance(instance)
{
    if (!a->job()) {
        setWindowTitle(tr("New Activity"));
        jobEdit = new QComboBox;

        foreach(Job *j, instance->getJobs()) {
            jobEdit->addItem(j->name(), qVariantFromValue((void *) j));
        }
    } else {
        setWindowTitle(tr("Edit Activity"));
    }
    nameEdit = new QLineEdit(a->job() ? a->name() : QString());
    durationEdit = new QSpinBox;
    durationEdit->setMinimum(1);
    if (!a->job()) {
        durationEdit->setMaximum(((Job *) jobEdit->itemData(jobEdit->currentIndex()).value<void *>())->duration());
    }
    else if(a->lftFlex() > 0) {
        durationEdit->setMaximum(a->lftFlex()-a->st());
    }
    else {
        durationEdit->setMaximum(qMin(a->job()->dueDate() - a->st(), (a->lst()+a->duration()) - a->st() ));
    }
    durationEdit->setValue(a->job() ? a->duration() : 1);

    usingTemplate = false;
    templateList = new QComboBox;
    templateList->addItem(tr("(none)"), QVariant());
    foreach (ActivityTemplate *a, TemplateGateway::getAll())
        templateList->addItem(a->getName(), qVariantFromValue((void *) a));

    // form fields
    if (!a->job()) addFormField(tr("&Job:"), jobEdit);
    addFormField(tr("&Template:"), templateList);
    addFormField(tr("&Name:"), nameEdit);

    if(instance->hoursOnTimeline())
        addFormField(tr("&Duration in minutes:"), durationEdit);
    else
        addFormField(tr("&Duration:"), durationEdit);

    // requirements table
    requirementsTable = new RequirementsTable(a, instance);
    requirementsTable->setEditableBy(this);
    requirementsTable->fillIn();
    addWidget(requirementsTable);

    // signals
    if(!a->job()) connect(jobEdit, SIGNAL(currentIndexChanged(int)), this, SLOT(jobChanged(int)));
    connect(templateList, SIGNAL(currentIndexChanged(int)), this, SLOT(templateChanged(int)));
    connect(nameEdit, SIGNAL(textChanged(QString)), this, SLOT(setModified()));
    connect(durationEdit, SIGNAL(valueChanged(QString)), this, SLOT(setModified()));

    setUpLayout();
}

void ActivityDialog::apply() {
    QString name = nameEdit->text().trimmed();
    if(name.isEmpty()) {
        QMessageBox::information(this, tr("Invalid input"), tr("Please fill in a name."));
    } else {
        a->setName(name);
        a->setDuration(durationEdit->value());

        if (!a->job()) {
            Job *j = (Job *) jobEdit->itemData(jobEdit->currentIndex()).value<void *>();
            a->setJob(j);
            instance->addActivity(a, j->releaseDate(), j->dueDate() - a->duration(), j->releaseDate(), false);
        }
        requirementsTable->applyToActivity();
        instance->setUserChanges(true);

        accept();
    }
}

void ActivityDialog::jobChanged(int index) {
    durationEdit->setMaximum(((Job *) jobEdit->itemData(index).value<void *>())->duration());
}

void ActivityDialog::templateChanged(int index) {
    usingTemplate = false;
    if(index > 0) {
        ActivityTemplate *activity = (ActivityTemplate *) templateList->itemData(index).value<void *>();

        if(activity->isCompatibleWith(instance)) {
            nameEdit->setText(activity->getName());
            durationEdit->setValue(activity->getDuration());
            requirementsTable->importTemplate(activity);
            usingTemplate = true;
        } else {
            QString message = tr("Not all resources from this template are available in the instance. Please add the following resources to this instance:\n")
                                +"\n"+activity->getResourceNames();
            QMessageBox::warning(this, tr("Incomplete resources"), message);
            templateList->setCurrentIndex(0);
        }
    } else {
        requirementsTable->clearSpinners();
    }
}

void ActivityDialog::setModified() {
    if(usingTemplate) {
        disconnect(templateList, SIGNAL(currentIndexChanged(int)), this, SLOT(templateChanged(int)));
        templateList->setCurrentIndex(0);
        connect(templateList, SIGNAL(currentIndexChanged(int)), this, SLOT(templateChanged(int)));
    }
}
