#include <QStatusBar>
#include <QAction>
#include <QDebug>
#include "controller/exceptions.h"
#include "controller/instancecontroller.h"
#include "widgets/dialogs/solvingprogressdialog.h"
#include "widgets/dialogs/solvewithoptionsdialog.h"
#include "widgets/dialogs/aboutdialog.h"
#include "data/instance_reader.h"
#include "data/template_gateway.h"
#include "data/templatedb.h"

Controller::Controller() : colorGenerator(), clearSoftPrecedences(false), untitledCounter (0) {
    TemplateGateway::instance = new TemplateDB();
}

Controller::~Controller() {
    delete TemplateGateway::instance;
}

void Controller::showMainWindow() {
    mainWindow = new MainWindow(this);
    mainWindow->show();
}

MainWindow* Controller::getMainWindow() {
    return mainWindow;
}

InstanceWidget* Controller::getCurrentInstanceWidget() {
    InstanceWidget* instance = mainWindow->getCurrentInstanceWidget();
    if(!instance)
        throw NoInstanceException();
    return instance;
}

InstanceController * Controller::getCurrentInstanceController() {
    return getCurrentInstanceWidget()->getInstanceController(); // TODO dit loopt raar
}

Instance* Controller::getCurrentInstance() {
    Instance *instance = mainWindow->getCurrentInstanceMdiWidget()->currentInstance();
    if(!instance)
        throw NoInstanceException();
    return instance;
}

void Controller::disableToolbarButtons(){
    mainWindow->setButtonState();
}

void Controller::startPaintingFramePrecedences(QList<Precedence *> precedences) {
    getCurrentInstanceWidget()->getOverlay()->startPaintingFramePrecedences(precedences);
}

QColor Controller::getColorFor(QString tag) {
    return colorGenerator.colorFor(tag);
}

void Controller::autoClearPrecedences() {
    QAction *action = qobject_cast<QAction *>(sender());
    clearSoftPrecedences = action->isChecked();
}

void Controller::setAutoClearPrecedences(bool clear) {
    clearSoftPrecedences = clear;
}

void Controller::solve() {
    try {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action) {
            QVariant v = action->data();
            Solver *s = reinterpret_cast<Solver *>(v.value<void*>());
            if (!s)
                throw SolverNotFoundException();
            mainWindow->setLastSolver(v);
            solve(s);
        }
    } catch(SolverNotFoundException const& e) {
        e.showErrorMessage(mainWindow);
    }
}

void Controller::solve(Solver *s) {
    try {
        Instance *instance = getCurrentInstance();
        getCurrentInstanceController()->stopDrawingDependencies();
        if(clearSoftPrecedences) {
            startPaintingFramePrecedences(QList<Precedence *> ());
            instance->clearSoftPrecedences();
        }
        // bah, don't wanna use s because then you get signal/slot problems when reusing same solver
        // should just fix signal/slot problems instead of using this ugly workaround
        Solver *s2 = Solver::load(s->getName());
        getCurrentInstanceWidget()->enableActivities();
        getCurrentInstanceWidget()->removePeaks();
        if(getFramesSlider()->isEnabled())
            getCurrentInstanceWidget()->toLastFrame();
        SolvingProgressDialog spd(s2, getCurrentInstanceController(), mainWindow);
        spd.exec();
        delete s2;
    } catch(NoInstanceException const& e) {
        e.showErrorMessage("Not solving because no instance is currently loaded.", mainWindow);
    } catch(SolverNotFoundException const& e) {
        e.showErrorMessage(mainWindow);
    }
}

void Controller::solveWithOptions() {
    try {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action) {
            QVariant v = action->data();
            Solver *s = reinterpret_cast<Solver *>(v.value<void*>());
            if (!s)
                throw SolverNotFoundException();
            mainWindow->setLastSolver(v);
            SolveWithOptionsDialog dialog(s, this, mainWindow);
            dialog.exec();
        }
    } catch(SolverNotFoundException const& e) {
        e.showErrorMessage(mainWindow);
    }
}

QList<Instance*> Controller::getAllInstances(){
	return mainWindow->getCurrentInstanceMdiWidget()->getAllInstances();
}

void Controller::duplicateInstance() {
    Instance *instance = InstanceReader::copy(getCurrentInstance());
    if(clearSoftPrecedences)
        instance->clearSoftPrecedences();
    mainWindow->getCurrentInstanceMdiWidget()->addInstance(instance);
}

void Controller::showAbout() {
    AboutDialog aboutDialog(mainWindow);
    aboutDialog.exec();
}

void Controller::setFramesSlider(QSlider * framesSlider) {
    this->framesSlider = framesSlider;
    connect(getFramesSlider(), SIGNAL(valueChanged(int)), this, SLOT(toFrame(int)));
}

QSlider * Controller::getFramesSlider() {
    return framesSlider;
}

void Controller::disableHistory(int range) {
    disconnect(getFramesSlider(), SIGNAL(valueChanged(int)), this, SLOT(toFrame(int)));
    getFramesSlider()->setDisabled(true);
    getFramesSlider()->setRange(range, range);
    getMainWindow()->frameLabel->setText(tr("%1 of %2").arg(0).arg(0));
    getMainWindow()->frameLeftAct->setDisabled(true);
    getMainWindow()->frameRightAct->setDisabled(true);
    connect(getFramesSlider(), SIGNAL(valueChanged(int)), this, SLOT(toFrame(int)));
}

void Controller::rewind() {
    getFramesSlider()->setValue(getFramesSlider()->value() - 1);
}

void Controller::forward() {
    getFramesSlider()->setValue(getFramesSlider()->value() + 1);
}

void Controller::toFrame(int frameNr) {
    try {
        getCurrentInstanceController()->getInstanceWidget()->toFrameNumber(frameNr);
    } catch (NoInstanceException const& e) { }
}

void Controller::zoomIn() {
    try{
        getCurrentInstanceWidget()->zoomIn();
        getCurrentInstanceWidget()->updateTimelineOffset();
    } catch(NoInstanceException const& e) { }
}

void Controller::zoomOut() {
    try{
        getCurrentInstanceWidget()->zoomOut();
        getCurrentInstanceWidget()->updateTimelineOffset();
    } catch(NoInstanceException const& e) { }
}

bool Controller::isPaintingFeasibleIntervals() {
    return paintFeasibleIntervals;
}

void Controller::setPaintingFeasibleIntervals(bool paintFeasibleIntervals) {
    Controller::paintFeasibleIntervals = paintFeasibleIntervals;
}

bool Controller::isPaintingFlexibilityIntervals() {
    return paintFlexibilityIntervals;
}

void Controller::setPaintingFlexibilityIntervals(bool paintFlexibilityIntervals) {
    Controller::paintFlexibilityIntervals = paintFlexibilityIntervals;
}

void Controller::doPaintFeasibleIntervals() {
    QAction *action = qobject_cast<QAction *>(sender());
    paintFeasibleIntervals = action->isChecked();

    try { getCurrentInstanceController()->doPaintFeasibleIntervals(); }
    catch(NoInstanceException const& e) { }
}

void Controller::setFlexTimes() {
    if(isPaintingFlexibilityIntervals()) {
        foreach(Instance * i, getAllInstances()) {
            foreach(Group* g, i->getGroups()) {
                g->setST(g->getESTFlex());
            }
        }
    }
}

void Controller::doFlexibilityIntervals() {
    QAction *action = qobject_cast<QAction *>(sender());
    paintFlexibilityIntervals = action->isChecked();
    setFlexTimes();
    try {
        getCurrentInstanceController()->doPaintFlexibilityIntervals();
    } catch(NoInstanceException const& e) {
         // ignore the action when there is no open instance
    }
}

void Controller::setStatusMessage(QString message, int timeout) {
    getMainWindow()->statusBar()->showMessage(message, timeout);
}

QString Controller::getWorkingDirectory() {
    return workingDirectory;
}

void Controller::setWorkingDirectory(QString workingDirectory) {
    Controller::workingDirectory = workingDirectory;
}

int Controller::getUntitledNumber() {
    return ++ untitledCounter;
}

bool Controller::develop = false;
