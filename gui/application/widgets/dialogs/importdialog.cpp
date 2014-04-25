#include "importdialog.h"
#include "controller/instancecontroller.h"
#include "controller/exceptions.h"
#include "data/instance_reader.h"

#include <QDialogButtonBox>
#include <QMessageBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QApplication>

ImportDialog::ImportDialog(MainWindow *parent, InstanceController *c) :
    FormDialog(parent), mw(parent), controller(c)
{
    setWindowTitle(tr("Import instance"));

    fileEdit = new QLineEdit;
    browseButton = new QPushButton(tr("&Browse..."));

    if(controller->getInstance()->hoursOnTimeline()) {
        timeEditHour = new QDateTimeEdit;
        timeEditHour->setDisplayFormat("h:mm");

        timeEdit = new QSpinBox;
        timeEdit->setMinimum(0);
        timeEdit->setMaximum(1 << 30);
        timeEdit->setValue(1);

        addFormField(tr("Import day:"), timeEdit);
        addFormField(tr("Import time:"), timeEditHour);
    } else {
        timeEdit = new QSpinBox;
        timeEdit->setMinimum(0);
        timeEdit->setMaximum(1 << 30);
        timeEdit->setValue(0);

        addFormField(tr("Import at time:"), timeEdit);
    }

    QHBoxLayout *binaryLayout = new QHBoxLayout;
    binaryLayout->addWidget(fileEdit);
    binaryLayout->addWidget(browseButton);
    addFormField(tr("File:"), binaryLayout);

    setUpLayout();
    connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
}

void ImportDialog::browse() {
    QString binary = QFileDialog::getOpenFileName(this, tr("Select instance file"), controller->getParent()->getWorkingDirectory());
    if (!binary.isEmpty()) {
        fileEdit->setText(binary);
        controller->getParent()->setWorkingDirectory(QFileInfo (binary).path());
    }
}

void ImportDialog::apply() {
    if(fileEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("Task scheduler"), tr("No instance selected."));
    }
    else {
        try{
            int time = 0;

            if(controller->getInstance()->hoursOnTimeline()) {
                QTime t = timeEditHour->time();
                time = QTime(0,0,0).secsTo(t)/60 + ((timeEdit->value()-1)*60*24);
            } else {
                time = timeEdit->value();
            }

            QApplication::setOverrideCursor(Qt::WaitCursor);
            controller->getInstance()->clearImportMaps();
            controller->getInstance()->setImportFlag(true);
            controller->getInstance()->setImportOffset(time);
            bool success = InstanceReader::import(controller->getInstance(), fileEdit->text());
            controller->getInstanceWidget()->updateZoom();
            controller->getInstance()->setImportFlag(false);
            QApplication::restoreOverrideCursor();

            if(!success) {
                QMessageBox::warning(this, tr("Task scheduler"), tr("Instance import failed."));
            }
        }
        catch(NoInstanceException const& e) {
            QMessageBox::warning(this, tr("Task scheduler"), tr("Cannot import the instance because there is no instance currently loaded."));
        }
    }
    accept();
}
