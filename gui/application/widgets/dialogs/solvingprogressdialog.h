#ifndef SOLVINGPROGRESSDIALOG_H
#define SOLVINGPROGRESSDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QProgressBar>
#include <QLabel>
#include <QTextEdit>

#include "solve/solver.h"


class InstanceController;

/** When the user selects a solver from the solve menu to solve
  * the current instance, this dialog is shown to report on the
  * progress of the solver. There is a progress bar and an initially
  * hidden log area that contains the output of the solver. When
  * the user presses the "More" button, the log is shown. When the
  * user presses the cancel button, the cancel() method of the
  * Solver class is called.
  */
class SolvingProgressDialog : public QDialog
{
    Q_OBJECT

    Solver *solver;
    InstanceController * controller;
    Instance *instance;

    QDialogButtonBox *buttonbox;
    QPushButton *cancelButton, *moreButton;

    QLabel *statusLabel;
    QProgressBar *progressBar;
    QLabel *outcomeLabel;
    QTextEdit *log;

    int numSoftPrecedences;

public:
    /** Creates the dialog and starts the solver on the given instance.
      *
      * @param solver the solver to call start(instance) on
      * @param instance the instance to solve
      * @param parent set a parent to control placement of this dialog
      */
    explicit SolvingProgressDialog(Solver *solver, InstanceController *controller, QWidget *parent = 0);

public slots:
    void toggleMoreOrLess(bool); ///< show or hide the log area
    void solverFinished(int, QProcess::ExitStatus); ///< the solver's finished signal is connected to this slot, it changes the cancel button into an ok button and sets the progress to 100%
    void setProgress(int); ///< connect to the solver's progressMade signal to update the progress bar
};

#endif // SOLVINGPROGRESSDIALOG_H
