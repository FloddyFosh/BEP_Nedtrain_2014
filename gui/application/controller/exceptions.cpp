#include "controller/exceptions.h"

#include <QMessageBox>
#include <QStatusBar>
#include <QDebug>

NoInstanceException::NoInstanceException() {
}

void NoInstanceException::showErrorMessage(const QString &message, QMainWindow *window) const {
    QMessageBox::warning(window, QObject::tr("No instance"), message, QMessageBox::Ok);
    window->statusBar()->showMessage(message, 3000);
}

InstanceManipulationException::InstanceManipulationException(const QString &message) : message(message) {}

void InstanceManipulationException::printWarning() const {
    qDebug() << qPrintable(message);
}

void InstanceManipulationException::showErrorMessage(QMainWindow *window) const {
    QMessageBox::warning(window, QObject::tr("Incorrect operation"), message, QMessageBox::Ok);
    window->statusBar()->showMessage(message, 3000);
}


SolverNotFoundException::SolverNotFoundException() {
}

void SolverNotFoundException::showErrorMessage(QMainWindow *window) const {
    QMessageBox::warning(window, QObject::tr("Solver not found"), QObject::tr("The solver information could not be found. Select Solve > Configure and enter the correct information."), QMessageBox::Ok);
}

ComparisonException::ComparisonException(const QString &message) : message(message) {}

void ComparisonException::showErrorMessage(QMainWindow *window) const {
    QMessageBox::warning(window, QObject::tr("Error while comparing"), message, QMessageBox::Ok);
    window->statusBar()->showMessage(message, 3000);
}
