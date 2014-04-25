#ifndef SOLVEWITHOPTIONSDIALOG_H
#define SOLVEWITHOPTIONSDIALOG_H

#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>

#include "widgets/dialogs/formdialog.h"
#include "solve/solver.h"

class Controller;

/**
 * Dialog to provide solver arguments before solving. Makes it also possible to display solved
 * instance in a new tab.
 */
class SolveWithOptionsDialog : public FormDialog {
    Q_OBJECT

    Solver *solver;
    Controller *controller;
    QLineEdit *arguments;
    QCheckBox *newTab, *mutex, *grouping;
    QSpinBox *threshold;
    QString originalArgs;

public:
    /**
     * Constructs a dialog to solve with options
     * @param solver solver
     * @param controller controller
     * @param parent reference to parent widget
     */
    explicit SolveWithOptionsDialog(Solver *solver, Controller *controller, QWidget *parent = 0);
    QString createArguments(); ///< returns a string of arguments for the solver, based on the checked options

protected slots:
    void apply(); ///< invokes solver with specified arguments and duplicates current instance if necessary
private slots:
    void updateGrouping();
private:
    void loadArguments(); ///< checks the right options, based on the predefined solver parameters
};

#endif // SOLVEWITHOPTIONSDIALOG_H
