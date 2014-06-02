#include "solvingprogressdialog.h"
#include "controller/instancecontroller.h"

#include <QBoxLayout>
#include <QLayout>
#include <QApplication>

SolvingProgressDialog::SolvingProgressDialog(Solver *solver, InstanceController *controller, QWidget *parent) :
    QDialog(parent), solver (solver), controller (controller), instance (controller->getInstance())
{
    // create ui
    setWindowTitle(tr("%1 Solver").arg(solver->getName()));

    numSoftPrecedences = 0;
    foreach(Precedence* p, instance->getSoftPrecedences()){
        if(!p->isDisabled()) numSoftPrecedences++;
    }

    statusLabel = new QLabel;

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel(tr("Status:")));
    hbox->addWidget(statusLabel, 1);

    progressBar = new QProgressBar;
    progressBar->setRange(0, 0);
    progressBar->setFixedWidth(500);

    outcomeLabel = new QLabel;

    // more or less button
    moreButton = new QPushButton(tr("More..."));
    moreButton->setCheckable(true);
    moreButton->setDefault(true);

    // buttons layout
    buttonbox = new QDialogButtonBox;
    cancelButton = buttonbox->addButton(QDialogButtonBox::Cancel);
    buttonbox->addButton(moreButton, QDialogButtonBox::ActionRole);

    log = new QTextEdit;
    log->setVisible(false);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setSizeConstraint(QLayout::SetFixedSize);
    vbox->addLayout(hbox);
    vbox->addWidget(progressBar);
    vbox->addWidget(outcomeLabel);
    vbox->addWidget(log);
    vbox->addWidget(buttonbox);

    setLayout(vbox);

    // connect signals
    connect(buttonbox, SIGNAL(rejected()), solver, SLOT(cancel()));
    connect(buttonbox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(moreButton, SIGNAL(toggled(bool)), this, SLOT(toggleMoreOrLess(bool)));

    connect(solver, SIGNAL(messageReceived(QString)), log, SLOT(append(QString)));
    connect(solver, SIGNAL(statusReceived(QString)), statusLabel, SLOT(setText(QString)));
    connect(solver, SIGNAL(progressMade(int)), this, SLOT(setProgress(int)));
    connect(solver, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(solverFinished()));
    connect(solver, SIGNAL(peak(int,int,int)), controller, SLOT(peak(int,int,int)));

    // start solving
    QApplication::setOverrideCursor(Qt::BusyCursor);
    controller->disconnectActivitiesFromResourceWidgets();
    solver->start(instance);
}

void SolvingProgressDialog::toggleMoreOrLess(bool more) {
    QPushButton *moreButton = qobject_cast<QPushButton *>(sender());
    if (moreButton) {
        moreButton->setText(more ? tr("Less...") : tr("More..."));
        log->setVisible(more);
    }
}

void SolvingProgressDialog::solverFinished() {
    int newConstraints = 0;
    foreach(Precedence* p, instance->getSoftPrecedences()){
        if(!p->isDisabled()) newConstraints++;
    }
    newConstraints -= numSoftPrecedences;

    instance->setUserChanges(false);
    controller->reconnectActivitiesToResourceWidgets();
    controller->repaintResourceWidgets();
    controller->setupSlider(true);
    controller->toLastFrame();
    
    log->append("\nFinished.");
    if(solver->isSolved()) {
        outcomeLabel->setStyleSheet("QLabel { color : #008800; }");
        outcomeLabel->setText(tr("The instance has been solved successfully.\nThe solver posted %1 precedence constraints.").arg(newConstraints));
    } else if(!solver->isSolved()) {
        QString msg = "";

        if(solver->getPeakResource() != -1) {
            msg = tr("The instance could not be solved, due to a resource peak on\nresource '%1'.").arg(instance->R(solver->getPeakResource())->name());
        }
        else if(solver->getMutexJob() != -1) {
            msg = tr("The tasks in project '%1' could not be scheduled mutually exclusive.").arg(instance->J(solver->getMutexJob())->name());
        }
        else {
            msg = tr("The instance could not be solved.");
        }

        outcomeLabel->setStyleSheet("QLabel { color : #ff0000; }");
        outcomeLabel->setText(msg);
    } else {
        outcomeLabel->setText(tr("See 'More...' for more information."));
    }
    buttonbox->removeButton(cancelButton);
    buttonbox->addButton(QDialogButtonBox::Ok);
    setProgress(100);
    QApplication::restoreOverrideCursor();
}

void SolvingProgressDialog::setProgress(int value) {
    progressBar->setRange(0,100);
    progressBar->setValue(value);
}
