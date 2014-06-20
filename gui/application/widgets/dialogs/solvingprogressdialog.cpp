#include "solvingprogressdialog.h"
#include "controller/instancecontroller.h"
#include "widgets/app_icon.h"

#include <QBoxLayout>
#include <QLayout>
#include <QApplication>

SolvingProgressDialog::SolvingProgressDialog(Solver *solver, InstanceController *controller, QWidget *parent) :
    QDialog(parent), solver (solver), controller (controller), instance (controller->getInstance())
{
    // create ui
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("%1 Solver").arg(solver->getName()));
    setWindowIcon(AppIcon("icon.png"));

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
    infoLabel = new QLabel;

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
    log->setReadOnly(true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setSizeConstraint(QLayout::SetFixedSize);
    vbox->addLayout(hbox);
    vbox->addWidget(progressBar);
    vbox->addWidget(outcomeLabel);
    vbox->addWidget(infoLabel);
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
    connect(solver, SIGNAL(finished(QProcess::ExitStatus)), this, SLOT(solverFinished(QProcess::ExitStatus)));
    connect(solver, SIGNAL(peak(int,int)), controller, SLOT(peak(int,int)));

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

void SolvingProgressDialog::solverFinished(QProcess::ExitStatus status) {
    if(status == QProcess::NormalExit) {
        int newConstraints = 0;
        foreach(Precedence* p, instance->getSoftPrecedences()){
            if(!p->isDisabled()) newConstraints++;
        }
        newConstraints -= numSoftPrecedences;

        instance->setUserChanges(false);
        controller->reconnectActivitiesToResourceWidgets();
        controller->repaintJobWidgets();
        controller->repaintResourceWidgets();
        controller->setupSlider(true);
        controller->toLastFrame();
        controller->getParent()->setFlexTimes();

        log->append("\nFinished.\n");
        if(solver->isSolved()) {
            infoLabel->setTextFormat(Qt::RichText);
            outcomeLabel->setStyleSheet("QLabel { color : #008800; }");
            outcomeLabel->setText(tr("The instance has been solved successfully.\n"));

            QString temp1 = tr("<font color=#008800>The solver was able to get rid of %1 precedence constraints.</font>");
            QString temp2 = tr("<font color=%1>The solver posted %2 precedence constraints.</font>");
            QString temp3 = tr("<br><font color=%1>The total flexibility of this solution is %2.");
            QString temp4 = tr("<font color=%1> (Previously: %2)</font>");

            QString resInfo;

            if(newConstraints < 0)
                resInfo.append(temp1.arg(QString::number(-newConstraints)));
            else {
                if (newConstraints == 0)
                    resInfo.append(temp2.arg("#008800", "0"));
                else if(numSoftPrecedences == 0)
                    resInfo.append(temp2.arg("#008800", QString::number(newConstraints)));
                else
                    resInfo.append(temp2.arg("#ff0000", QString::number(newConstraints)));
            }

            if(instance->getTotalFlex() > -1) {
                if(instance->getPrevFlex() > -1) {
                    if(instance->getTotalFlex() < instance->getPrevFlex()) {
                        resInfo.append(temp3.arg("#ff0000",QString::number(instance->getTotalFlex())));
                        resInfo.append(temp4.arg("#ff0000",QString::number(instance->getPrevFlex())));
                    }
                    else {
                        resInfo.append(temp3.arg("#008800",QString::number(instance->getTotalFlex())));
                        resInfo.append(temp4.arg("#008800",QString::number(instance->getPrevFlex())));
                    }
                }
                else {
                    resInfo.append(temp3.arg("#008800", QString::number(instance->getTotalFlex())));
                }
            }
            infoLabel->setText(resInfo);
        } else if(!solver->isSolved()) {
            QString msg = "";

            if(solver->getPeakResource() != -1) {
                msg = tr("The instance could not be solved, due to a resource peak on\nresource '%1'.").arg(instance->R(solver->getPeakResource())->name());
            }
            else if(solver->getMutexJob() != -1) {
                msg = tr("The activities in job '%1' could not be scheduled mutually exclusive.").arg(instance->J(solver->getMutexJob())->name());
            }
            else {
                msg = tr("The instance could not be solved.");
            }

            outcomeLabel->setStyleSheet("QLabel { color : #ff0000; }");
            outcomeLabel->setText(msg);
        } else {
            outcomeLabel->setText(tr("See 'More...' for more information."));
        }
    } else {
        instance->setUserChanges(true);
        controller->reconnectActivitiesToResourceWidgets();

        log->append("\nTerminated.\n");

        outcomeLabel->setStyleSheet("QLabel { color : #ff0000; }");
        outcomeLabel->setText(tr("The solver has been terminated. Solution so far has been applied."));
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

void SolvingProgressDialog::closeEvent(QCloseEvent *ev){
    solver->cancel();
    solver->getProcess()->waitForFinished();
    ev->accept();
}
