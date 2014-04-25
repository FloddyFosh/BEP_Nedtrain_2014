#ifndef SOLVER_H
#define SOLVER_H

#include <QProcess>

#include "model/instance.h"

/**
 * Encapsulates an external solver program. The settings of the solvers
 * are saved using the QSettings api.
 */
class Solver : public QObject
{
    Q_OBJECT

    QString name; ///< a descriptive name of the solver, this is used in the Solve menu in the app
    QString binary; ///< the path to the binary
    QString arguments; ///< any argument string that the solver needs
    QProcess process; ///< used for starting/ending and communicating with the solver process

    Instance *instance; ///< instance that is solved
    bool solved;
    int peakResource;
    int mutexJob;
    QVector<Frame *> replayFrames; ///< vector used for generating frames after solving
    QMap<Activity *, bool> wasLocked;
public:
    /** Constructor, creates a Solver object that can be used to change settings or to
      * run specific solvers.
      *
      * @param name a descriptive name of the solver, this is used in the Solve menu in the app
      * @param binary the path to the binary
      * @param arguments any argument string that the solver needs
      * @param parent unused
      */
    explicit Solver(QString name, QString binary, QString arguments, QObject *parent = 0);

    /** Returns a list of all Solver objects (pointers) available from the QSettings.
      */
    static QList<Solver *> loadAll();

    /** Loads a specific solver from QSettings by solver name.
      */
    static Solver *load(QString name);

    /** Removes the current solver from the QSettings.
      */
    void remove();

    /** Saves the current solver to the QSettings register.
      */
    void save();

    QString getName() const; ///< returns the name
    QString getBinary() const; ///< returns the name of the binary
    QString getArguments() const; ///< returns the argument string

    void setName(QString); ///< changes the solver name
    void setBinary(QString); ///< changes the solver binary
    void setArguments(QString); ///< changes the solver argument string

    /** Starts the external solver process. The result of the instance's toString method is fed to
      * the solver's stdin. When there is output on the solver's stdout or stderr, it is sent to
      * anyone who is listening through the appropriate signals.
      *
      * @param i the instance to be solved
      */
    bool start(Instance *i);

    QProcess* getProcess(); ///< returns the process corresponding to the solver.
    void setSolved(bool); ///< sets the solved variable, representing the result of solving the instance.
    bool isSolved(); ///< returns true iff the instance was solved successfully.
    int getPeakResource(); ///< returns the id of the resource with an unresolvable conflict, if solving failed.
    int getMutexJob(); ///< returns the id of the job where the mutual exclusive requirement could not be satisfied, if solving failed.

private:
    void processProgressLine(QByteArray &line); ///< processes a progress line outputted by the solver.
    void processPrecedenceLine(QByteArray &line); ///< processes a precedence line outputted by the solver.
    void processStateLine(QByteArray &line);  ///< processes a state line outputted by the solver.
    void processStateGroups(QList<QByteArray> fields, Frame *frame);  ///< processes a state line with groups outputted by the solver.
    void processPeakLine(QByteArray &line);  ///< processes a peak line outputted by the solver.
    void processMutexLine(QByteArray &line);  ///< processes a mutex line outputted by the solver.
    void eatRemainingOutput(QList<QByteArray> &fields); ///< processes unused output.

signals:
    void progressMade(int progress); ///< emitted when a progress report is received from the solver, should be between 0 and 100
    void statusReceived(QString message); ///< emitted when a status message is received from the solver, these are all lines of text starting with "STATUS: "
    void messageReceived(QString message); ///< emitted when any message is received from the solver
    void newEarliestStartTime(int job, int activity, int est); ///< emitted when a new earliest start time (EST) for an activity is received from the solver
    void finished(int, QProcess::ExitStatus); ///< emitted when the solver process finishes, the exitcode is passed in the signal and can be used to check for success or failure
    void newPrecedenceConstraint(int j1, int a1, int j2, int a2); ///< emitted when a new precedence constraint is posted between activities.
    void peak(int time, int resource, int capacity); ///< emitted when the solver encountered a peak.

public slots:
    void processOutput(); ///< internal slot for handling solver output (both stdout and stderr).
    void cancel(); ///< slot for cancelling the solving action, will kill the solver process.
    void solverFinished(int, QProcess::ExitStatus); ///< slot called when solver finishes.
};

#endif // SOLVER_H
