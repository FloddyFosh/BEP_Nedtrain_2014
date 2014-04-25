#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <QMainWindow>

class NoInstanceException {
public:
    NoInstanceException();
    void showErrorMessage(const QString &, QMainWindow *) const;
};

class InstanceManipulationException {
private:
    QString message;
public:
    InstanceManipulationException(const QString &);
    void printWarning() const;
    void showErrorMessage(QMainWindow *) const;
};

class SolverNotFoundException {
public:
    SolverNotFoundException();
    void showErrorMessage(QMainWindow *) const;
};

class ComparisonException {
private:
    QString message;
public:
    ComparisonException(const QString &);
    void showErrorMessage(QMainWindow *) const;
};

#endif /* EXCEPTIONS_H_ */
