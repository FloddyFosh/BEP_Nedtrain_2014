#include "widgets/dialogs/solvewithoptionsdialog.h"
#include "controller/controller.h"

SolveWithOptionsDialog::SolveWithOptionsDialog(Solver *solver, Controller *controller, QWidget *parent) :
    FormDialog(parent), solver(solver), controller(controller)
{
    setWindowTitle(tr("Set options and solve with ") + solver->getName());
    originalArgs = solver->getArguments();

    QLabel *solverName = new QLabel(solver->getName());
    solverName->setTextFormat(Qt::PlainText);
    newTab = new QCheckBox(tr("Solve to new &tab"), this);
    mutex = new QCheckBox(tr("&Mutual exclusion"), this);
    grouping = new QCheckBox(tr("&Grouping"), this);
    threshold = new QSpinBox(this);
    addFormField(tr("Chosen solver:"), solverName);
    addFormField(mutex);
    addFormField(grouping);
    addFormField(tr("&Threshold:"), threshold);
    addFormField(newTab);

    setUpLayout();
    connect(grouping, SIGNAL(stateChanged(int)), this, SLOT(updateGrouping()));
    loadArguments();
}

void SolveWithOptionsDialog::apply() {
    solver->setArguments(createArguments());
    accept();
    if(newTab->isChecked())
        controller->duplicateInstance();
    controller->solve(solver);
    solver->setArguments(originalArgs);
}

void SolveWithOptionsDialog::loadArguments() {
    if(originalArgs.contains("-x"))
        mutex->setChecked(originalArgs.contains("-x"));
    QRegExp pattern("-m ?(\\d+)");
    int pos = pattern.indexIn(originalArgs);
    if(pos > -1) {
        grouping->setChecked(true);
        threshold->setValue(pattern.cap(1).toInt());
    }
    updateGrouping();
}

QString SolveWithOptionsDialog::createArguments() {
    QString arguments = "";
    if(mutex->isChecked())
        arguments += "-x ";
    if(grouping->isChecked())
        arguments += QString("-m %1").arg(threshold->value());
    return arguments.trimmed();
}

void SolveWithOptionsDialog::updateGrouping() {
    threshold->setEnabled(grouping->isChecked());
}
