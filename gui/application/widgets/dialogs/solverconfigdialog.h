#ifndef SOLVERCONFIGDIALOG_H
#define SOLVERCONFIGDIALOG_H

#include "widgets/dialogs/listformdialog.h"
#include "solve/solver.h"

class Controller;

/**
 * A dialog to configure additional solver programs that can then be accessed
 * through the 'solve' menu. No configuration or post-processing needed. Just
 * create an instance of this class and call exec() on it.
 */
class SolverConfigDialog : public ListFormDialog
{
    Q_OBJECT

    QListWidget *solverList;
    QMap<Solver*, QListWidgetItem*> solverItems;

    QLineEdit *nameEdit;
    QLineEdit *binaryEdit;
    QPushButton *browseButton;
    QLineEdit *argumentsEdit;

    Solver *current;
    
    Controller * controller;

    /**
     * Loads solvers into dialog.
     */
    void initSolvers();

    /**
     * Changes information in editable fields to values of specified solver
     * @param s solver
     */
    void initTo(Solver *s);

public:
    /** Pass a parent widget to allow for smart placing of this dialog. */
    explicit SolverConfigDialog(Controller *, QWidget *parent = 0);

    /**
     * Changes the enabled property of all edit fields to specified value.
     * @param b value to be set
     */
    void setEditable(bool b);

private slots:
    void addToList(); ///< adds a new solver
    void removeFromList(); ///< removes selected solver
    void browse(); ///< opens browse dialog to select a file location and sets field if necessary
    void maybeChangeSelection(QListWidgetItem*, QListWidgetItem*); ///< slot to ask if the user wants to change the selected item or continue editing current item, invoked after item selection
    void apply(); ///< saves the current solver configuration
};

#endif // SOLVERCONFIGDIALOG_H
