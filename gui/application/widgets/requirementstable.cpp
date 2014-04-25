#include "widgets/requirementstable.h"

#include <QLabel>
#include <QHeaderView>

RequirementsTable::RequirementsTable(Activity *a, Instance* instance)
    : QTableWidget(instance->getResources().size(), 2), instance(instance), activity(a), activityTemplate(0)
{
    init();
}

RequirementsTable::RequirementsTable(Instance* instance)
    : QTableWidget(instance->getResources().size(), 2), instance(instance), activity(0), activityTemplate(0)
{
    init();
}

void RequirementsTable::setActivityTemplate(ActivityTemplate* a) {
    activityTemplate = a;
    fillIn();
}

void RequirementsTable::init() {
    editor = 0;
    horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    verticalHeader()->setVisible(false);
    setSelectionMode(QAbstractItemView::NoSelection);

    QStringList labels;
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
    else
        return activityTemplate->getRequiredAmount(r->name());
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
