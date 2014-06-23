#include "requirementstable.h"

#include "model/instance.h"
#include "model/activitytemplate.h"

#include <QLabel>
#include <QHeaderView>

RequirementsTable::RequirementsTable(Activity *a, Instance* instance)
    : QTableWidget(instance->getResources().size(), 2), instance(instance), activity(a), activityTemplate(0), job(0)
{
    init();
}

RequirementsTable::RequirementsTable(Job *j, Instance* instance)
    : QTableWidget(instance->getResources().size(), 2), instance(instance), activity(0), activityTemplate(0), job(j)
{
    init();
}

RequirementsTable::RequirementsTable(Instance* instance)
    : QTableWidget(instance->getResources().size(), 2), instance(instance), activity(0), activityTemplate(0), job(0)
{
    init();
}

void RequirementsTable::setActivityTemplate(ActivityTemplate* a) {
    activityTemplate = a;
    fillIn();
}

void RequirementsTable::setJob(Job *j) {
    job = j;
    fillIn();
}

void RequirementsTable::init() {
    editor = 0;
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    verticalHeader()->setVisible(false);
    setSelectionMode(QAbstractItemView::NoSelection);

    QStringList labels;
    if(job)
        labels << tr("Resource") << tr("Minimal needed");
    else
        labels << tr("Resource") << tr("Amount");
    setHorizontalHeaderLabels(labels);
}

void RequirementsTable::fillIn() {
    int row = 0;
    foreach(Resource *r, instance->getResources()) {
        QTableWidgetItem *name = new QTableWidgetItem(r->name());
        name->setFlags(Qt::ItemIsEnabled);
        setItem(row, 0, name);
        if(editor) {
            spinners[r] = new QSpinBox;
            spinners[r]->setValue(getRequiredAmount(r));
            setCellWidget(row, 1, spinners[r]);
            connect(spinners[r], SIGNAL(valueChanged(int)), editor, SLOT(setModified()), Qt::UniqueConnection);
        } else {
            QLabel* resourceLabel = new QLabel(QString::number(getRequiredAmount(r)));
            setCellWidget(row, 1, resourceLabel);
        }
        row++;
    }
}

void RequirementsTable::setEditableBy(QWidget* widget) {
    editor = widget;
}

void RequirementsTable::importTemplate(ActivityTemplate *import) {
    QMapIterator<Resource *, QSpinBox *> it(spinners);
    while (it.hasNext()) {
        it.next();
        Resource *r = it.key();
        QSpinBox *spinner = it.value();
        int duration = 0;
        if(import->getRequirements().contains(r->name()))
            duration = import->getRequirements().value(r->name());
        spinner->setValue(duration);
    }
}

int RequirementsTable::getRequiredAmount(Resource *r) {
    if(activity)
        return activity->getRequiredAmount(r);
    else if(activityTemplate)
        return activityTemplate->getRequiredAmount(r->name());
    else if(job) {
        return job->getRequiredAmount(r);
    }
    else return 0;
}

void RequirementsTable::clearSpinners() {
    foreach(QSpinBox *spinner, spinners)
        spinner->setValue(0);
}

void RequirementsTable::applyToActivity() {
    QMap<QString, int> requirements;
    QMapIterator<Resource *, QSpinBox *> it(spinners);
    while (it.hasNext()) {
        it.next();
        Resource *r = it.key();
        int duration = it.value()->value();
        if(activity)
            activity->setRequiredAmount(r, it.value()->value());
        if(duration > 0)
            requirements.insert(r->name(), duration);
    }
    if(activityTemplate)
        activityTemplate->setRequirements(requirements);
}
