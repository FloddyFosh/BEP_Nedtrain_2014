#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>

#include "mainwindow.h"
#include "model/instance.h"
#include "instancemdisubwindow.h"
#include "data/instance_reader.h"

#include "controller/instancecontroller.h"

InstanceMdiSubWindow::InstanceMdiSubWindow(InstanceController *controller, QWidget *parent)
: QMdiSubWindow (parent), controller (controller) { }

bool InstanceMdiSubWindow::save() {
    Instance *instance = controller->getInstance();
    if (instance->getFileName().isEmpty())
        return saveAs();
    else {
        return saveFile(instance->getFileName());
    }
}

bool InstanceMdiSubWindow::saveAs() {
    QString fileName = QFileDialog::getSaveFileName(this, QString (), controller->getParent()->getWorkingDirectory());
    if (!fileName.isEmpty()) {
        controller->getParent()->setWorkingDirectory(QFileInfo (fileName).path());
        return saveFile(fileName);
    }
    else return false;
}


void InstanceMdiSubWindow::closeEvent(QCloseEvent * closeEvent) {
    if (isWindowModified()) {
    	if(Controller::develop){
    		QMdiSubWindow::closeEvent(closeEvent);
    		controller->getParent()->getMainWindow()->setButtonState();
    		return;
    	}

        int ans = QMessageBox::question(this, tr("Instance not saved"), tr("Do you want to save the instance before closing?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

        if (ans == QMessageBox::Save) {
            if (!save()) {
                closeEvent->ignore(); return;
            }
        }
        else if (ans == QMessageBox::Cancel) {
            closeEvent->ignore(); return;
        }
    }
    QMdiSubWindow::closeEvent(closeEvent);

    controller->getParent()->getMainWindow()->setButtonState();
}

void InstanceMdiSubWindow::updateFileName(const QString &fileName) {
    setWindowFilePath(fileName);
    setWindowTitle(QFileInfo(fileName).fileName()+"[*]");
}

bool InstanceMdiSubWindow::saveFile(const QString &fileName) {
    Instance *instance = controller->getInstance();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool success = InstanceReader::save(instance, fileName);
    QApplication::restoreOverrideCursor();
    if (success) {
        controller->setStatusMessage(tr("File saved"), 2000);
        controller->getParent()->getMainWindow()->rememberRecentFile(fileName);
        updateFileName(fileName);
        setWindowModified(false);
    } else {
        QMessageBox::warning(this, tr("Task scheduler"),
                             tr("Cannot save file %1.")
                             .arg(fileName));
        controller->setStatusMessage(tr("Cannot save file %1.").arg(fileName), 2000);
    }
    return success;
}

