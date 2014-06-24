#include <QMessageBox>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QSettings>
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QAction>

#include "controller/instancecontroller.h"
#include "controller/exceptions.h"
#include "data/template_gateway.h"
#include "data/instance_reader.h"
#include "widgets/app_icon.h"
#include "widgets/dialogs/solverconfigdialog.h"
#include "widgets/dialogs/managetemplates.h"
#include "widgets/dialogs/activitydialog.h"
#include "widgets/dialogs/jobdialog.h"
#include "widgets/dialogs/resourcedialog.h"
#include "widgets/dialogs/importdialog.h"
#include "widgets/dialogs/comparedialog.h"
#include "widgets/comparewindow.h"
#include "widgets/instancemdisubwindow.h"
#include "util/language.h"

using namespace std;

MainWindow::MainWindow(Controller *controller)
    : controller(controller), languageActions (this)
{
    setAttribute(Qt::WA_DeleteOnClose);

    // init ui
    setupCentralWidget();
    createActions();
    createMenus();
    createToolbar();
    statusBar();
    setWindowIcon(AppIcon("icon.png"));
    setWindowTitle(tr("Activity scheduler"));
    setButtonState();
    installEventFilter(this);
    
    controller->setFramesSlider(framesSlider);

    // restore size and position settings
    readSettings();
}

void MainWindow::readSettings() {
    QSettings settings;
    restoreGeometry(settings.value("mainwin/geometry").toByteArray());
    restoreState(settings.value("mainwin/windowState").toByteArray());
    bool clear = settings.value("clearSoftPrecedences").toBool();
    autoClearPrecedencesAct->setChecked(clear);
    bool useHours = settings.value("useHoursOnTimeline").toBool();
    useHoursAct->setChecked(useHours);
    controller->setAutoClearPrecedences(clear);
    bool paintFeasibleIntervals = settings.value("paintFeasibleIntervals").toBool();
    paintFeasibleIntervalsAct->setChecked(paintFeasibleIntervals);
    bool paintFlexibilityIntervals = settings.value("paintFlexibilityIntervals").toBool();
    paintFlexibilityIntervalsAct->setChecked(paintFlexibilityIntervals);
    controller->setPaintingFeasibleIntervals(paintFeasibleIntervals);
    controller->setPaintingFlexibilityIntervals(paintFlexibilityIntervals);
    controller->setWorkingDirectory(settings.value("workingDirectory").toString());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    while (tabwidget->isInstanceActive()) {
        if (!tabwidget->closeSubWindow()) {
            // gebruiker heeft een niet afgesloten, dus stop.
            event->ignore();
            return;
        }
    }
    
    QSettings settings;
    settings.setValue("mainwin/geometry", saveGeometry());
    settings.setValue("mainwin/windowState", saveState());
    settings.setValue("clearSoftPrecedences", autoClearPrecedencesAct->isChecked());
    settings.setValue("useHoursOnTimeline", useHoursAct->isChecked());
    settings.setValue("paintFeasibleIntervals", paintFeasibleIntervalsAct->isChecked());
    settings.setValue("paintFlexibilityIntervals", paintFlexibilityIntervalsAct->isChecked());
    settings.setValue("workingDirectory", controller->getWorkingDirectory());
    delete controller;
    QMainWindow::closeEvent(event);
}

/** ACTIONS **/

void MainWindow::newInstance() {
    Instance *i = new Instance(this);
    tabwidget->addInstance(i);
    setButtonState();
    statusBar()->showMessage(tr("New instance loaded"), 3000);
}

void MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open instance from file"), controller->getWorkingDirectory());
    if (!fileName.isEmpty()) {
        loadFile(fileName);
        controller->setWorkingDirectory(QFileInfo (fileName).path());
    }
    setButtonState();
}

void MainWindow::openRecent() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString());
    setButtonState();
}

void MainWindow::newResource() {
    try {
        ResourceDialog rd(controller->getCurrentInstance(), 0, this);
        rd.exec();
    }
    catch(NoInstanceException const& e) {
        e.showErrorMessage(tr("No instance active, cannot add resource."), this);
    }
}

void MainWindow::newJob() {
    try {
        JobDialog jd(controller->getCurrentInstance(), 0, this);
        jd.exec();
    }
    catch(NoInstanceException const& e) {
        e.showErrorMessage(tr("No instance active, cannot add job."), this);
    }
}

void MainWindow::newActivity() {
    try {
        if (controller->getCurrentInstance()->getJobs().isEmpty()) {
            QMessageBox::warning(this, tr("Cannot add activity"),tr("Add a job first"), QMessageBox::Ok);
            statusBar()->showMessage(tr("Add a job first."),2000);
            return;
        }
        Activity *a = new Activity;
        ActivityDialog ad(controller->getCurrentInstance(), a, this);
        ad.exec();
    }
    catch(NoInstanceException const& e) {
        e.showErrorMessage(tr("No instance active, cannot add activity."), this);
    }
}

void MainWindow::newPrecedence() {
    try {
        if(removePrecedenceAct->isChecked())
            controller->getCurrentInstanceWidget()->disablePrecedenceRemoving();
        if(!newPrecedenceAct->isChecked())
            controller->getCurrentInstanceWidget()->disablePrecedenceAdding();
        else {
            controller->getCurrentInstanceWidget()->enablePrecedenceAdding();
            controller->getMainWindow()->statusBar()->showMessage(tr("Click on two activities to add a precedence constraint between them."),3000);
        }
    }
    catch(NoInstanceException const& e) {
        newPrecedenceAct->setChecked(false);
        e.showErrorMessage(tr("No instance active, cannot add activity."), controller->getMainWindow());
    }
}

void MainWindow::removePrecedence() {
    try {
        if(newPrecedenceAct->isChecked())
            controller->getCurrentInstanceWidget()->disablePrecedenceAdding();
        if(!removePrecedenceAct->isChecked())
            controller->getCurrentInstanceWidget()->disablePrecedenceRemoving();
        else {
            controller->getCurrentInstanceWidget()->enablePrecedenceRemoving();
            controller->getMainWindow()->statusBar()->showMessage(tr("Click on two activities to remove a precedence constraint between them."),3000);
        }
    }
    catch(NoInstanceException const& e) {
        removePrecedenceAct->setChecked(false);
        e.showErrorMessage(tr("No instance active, cannot remove activity."), controller->getMainWindow());
    }
}

void MainWindow::uncheckPrecedenceAdding() {
    newPrecedenceAct->setChecked(false);
}

void MainWindow::uncheckPrecedenceRemoving() {
    removePrecedenceAct->setChecked(false);
}

void MainWindow::clearPrecedences() {
    controller->getCurrentInstance()->clearSoftPrecedences();
    controller->getCurrentInstanceWidget()->getOverlay()->startPaintingFramePrecedences(QList<Precedence *> ());
    controller->getCurrentInstanceWidget()->getOverlay()->repaint();
}

void MainWindow::configureSolvers() {
    SolverConfigDialog scd(controller, this);
    scd.exec();
    updateSolverActions();
}

void MainWindow::importInstance() {
    ImportDialog id(this, controller->getCurrentInstanceController());
    id.exec();
    controller->getCurrentInstanceController()->repaintResourceWidgets();
}

void MainWindow::updateSolverActions() {
    QList<Solver *> solvers = Solver::loadAll();
    solverActions.clear();

    for (int i = 0; i < solvers.size(); ++i) {
        Solver *s = solvers[i];

        QAction *action = new QAction(this);
        action->setText(tr("&%1 %2").arg(i + 1).arg(s->getName()));
        action->setData(QVariant::fromValue<void*>(s));
        disableIfInstanceEmpty(action);
        solverActions.push_back(action);
        connect(action, SIGNAL(triggered()), controller, SLOT(solve()));

        setLastSolver(QVariant::fromValue<void*>(s));
        solveLastAction->setToolTip(tr("Solve with ") + s->getName());
    }

    solveMenu->clear();
    solveMenu->addActions(solverActions);

    solveButtonMenu->clear();
    solveButtonMenu->addActions(solverActions);

    solveMenu->addSeparator();
    solveMenu->addAction(configureSolversAction);
}

void MainWindow::setLastSolver(QVariant v) {
    Solver *s = reinterpret_cast<Solver *>(v.value<void*>());
    solveLastAction->setData(v);
    solveLastAction->setToolTip(tr("Solve with ") + s->getName());
    solveWithParamsAction->setData(v);
    solveWithParamsAction->setToolTip(tr("Set options and solve with ") + s->getName());
}

void MainWindow::manageTemplates() {
    try {
        ManageTemplatesDialog scd(controller->getCurrentInstance(), this);
        scd.exec();
    }
    catch(NoInstanceException const& e) {
        e.showErrorMessage(tr("No instance active, cannot manage templates."), this);
    }
}

void MainWindow::loadFile(const QString &fileName) {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    Instance *i = InstanceReader::load(fileName);
    QApplication::restoreOverrideCursor();

    if (i) {
        tabwidget->addInstance(i);
        statusBar()->showMessage(tr("File loaded"), 2000);
        rememberRecentFile(fileName);
    } else {
        QMessageBox::warning(this, tr("Activity scheduler"),
                             tr("Cannot read file %1:\nFile not found, not readable or parsing error.")
                             .arg(fileName));
        statusBar()->showMessage(tr("Cannot read file %1: File not found, not readable or parsing error.").arg(fileName), 2000);
    }
}

void MainWindow::updateWindowTitle(const QString &fileName) {
    setWindowFilePath(fileName);
}

void MainWindow::rememberRecentFile(const QString &fileName) {
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    updateRecentFileActions();
}

void MainWindow::loadLastInstance(){
	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();

	if(files.size()>0)
		loadFile(files[0]);
}

void MainWindow::updateRecentFileActions() {
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = min(files.size(), (int) MaxRecentFiles);
    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    separatorAct->setVisible(numRecentFiles > 0);
}

/** INITIALIZATION **/

void MainWindow::createActions() {
    /* file menu */
    newAct = new QAction(tr("&New Instance"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new instance"));
    newAct->setIcon(AppIcon("new.png"));

    openAct = new QAction(tr("&Open Instance..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing instance"));
    openAct->setIcon(AppIcon("open.png"));

    importAct = new QAction(tr("&Import Instance..."), this);
    importAct->setStatusTip(tr("Import an existing instance"));
    disableIfInstanceEmpty(importAct);

    saveAct = new QAction(tr("&Save Instance"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the current instance"));
    saveAct->setIcon(AppIcon("save.png"));
    disableIfInstanceEmpty(saveAct);

    saveAsAct = new QAction(tr("&Save Instance As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the current instance under a new name"));
    disableIfInstanceEmpty(saveAsAct);

    closeAct = new QAction(tr("&Close Instance"), this);
    closeAct->setStatusTip(tr("Close the current instance"));
    closeAct->setIcon(AppIcon("close.png"));
    disableIfInstanceEmpty(closeAct);

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecent()));
    }

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    /* edit menu */
    newResourceAct = new QAction(tr("New &Resource..."), this);
    newResourceAct->setStatusTip(tr("Add a new resource to the current instance"));
    newResourceAct->setIcon(AppIcon("add_resource_profile.png"));
    disableIfInstanceEmpty(newResourceAct);

    newJobAct = new QAction(tr("New &Job..."), this);
    newJobAct->setStatusTip(tr("Add a new job to the current instance"));
    newJobAct->setIcon(AppIcon("add_project.png"));
    disableIfInstanceEmpty(newJobAct);

    newActivityAct = new QAction(tr("New &Activity..."), this);
    newActivityAct->setStatusTip(tr("Add a new activity to the current instance"));
    newActivityAct->setIcon(AppIcon("add_task.png"));
    disableIfInstanceEmpty(newActivityAct);

    newPrecedenceAct = new QAction(tr("Add precedence &constraint"), this);
    newPrecedenceAct->setStatusTip(tr("Add a precedence constraint to the current instance"));
    newPrecedenceAct->setIcon(AppIcon("add_constraint.png"));
    newPrecedenceAct->setCheckable(true);
    disableIfInstanceEmpty(newPrecedenceAct);

    removePrecedenceAct = new QAction(tr("Remove precedence &constraint"), this);
    removePrecedenceAct->setStatusTip(tr("Remove precedence constraint"));
    removePrecedenceAct->setIcon(AppIcon("remove_constraint.png"));
    removePrecedenceAct->setCheckable(true);
    disableIfInstanceEmpty(removePrecedenceAct);

    clearPrecedencesAct = new QAction(tr("Clear generated constraints"), this);
    clearPrecedencesAct->setStatusTip(tr("Clear all solver generated constraints of the current instance"));
    disableIfInstanceEmpty(clearPrecedencesAct);

    autoClearPrecedencesAct = new QAction(tr("Automatically clear generated constraints before solving"), this);
    autoClearPrecedencesAct->setStatusTip(tr("Automatically clear generated constraints before solving"));
    autoClearPrecedencesAct->setCheckable(true);

    useHoursAct = new QAction(tr("Show hours on timeline"), this);
    useHoursAct->setStatusTip(tr("Show hours on timeline"));
    useHoursAct->setCheckable(true);
    useHoursAct->setChecked(false);
    disableIfInstanceEmpty(useHoursAct);

    manageTemplatesAct = new QAction(tr("&Manage templates..."), this);
    manageTemplatesAct->setStatusTip(tr("Add or remove activity templates"));
    disableIfInstanceEmpty(manageTemplatesAct);

    paintFeasibleIntervalsAct = new QAction(tr("Paint feasible intervals"), this);
    paintFeasibleIntervalsAct->setStatusTip(tr("Paint the feasible interval for each activity"));
    paintFeasibleIntervalsAct->setCheckable(true);

    paintFlexibilityIntervalsAct = new QAction(tr("Paint flexibility intervals"), this);
    paintFlexibilityIntervalsAct->setStatusTip(tr("Paint the flexible interval for each activity"));
    paintFlexibilityIntervalsAct->setCheckable(true);

    /* solve menu */
    configureSolversAction = new QAction(tr("&Configure..."), this);
    configureSolversAction->setStatusTip(tr("Configure available solvers"));
    
    solveLastAction = new QAction(tr("Solve"), this);
    solveLastAction->setStatusTip(tr("Solve the current instance"));
    solveLastAction->setIcon(AppIcon("solve.png"));

    disableIfInstanceEmpty(solveLastAction);

    solveWithParamsAction = new QAction(tr("Set options and solve"), this);
    solveWithParamsAction->setStatusTip(tr("Set options and solve the current instance"));
    solveWithParamsAction->setIcon(AppIcon("solve-params.png"));
    disableIfInstanceEmpty(solveWithParamsAction);

    aboutAct = new QAction(tr("&About..."), this);
    aboutAct->setShortcuts(QKeySequence::HelpContents);

    /* zoom in and out */
    zoomInAct = new QAction(tr("Zoo&m in"), this);
    zoomOutAct = new QAction(tr("Zoom o&ut"), this);
    disableIfInstanceEmpty(zoomInAct);
    disableIfInstanceEmpty(zoomOutAct);
    zoomInAct->setIcon(AppIcon("zoom_in.png"));
    zoomOutAct->setIcon(AppIcon("zoom_out.png"));
    zoomInAct->setStatusTip(tr("Zoom in the instance"));
    zoomOutAct->setStatusTip(tr("Zoom in the instance"));

    /* compare */
    compareAct = new QAction(tr("Compare"), this);
    disableIfInstanceEmpty(compareAct);
    compareAct->setIcon(AppIcon("compare.png"));
    compareAct->setStatusTip(tr("Compare two instances"));

    /* replay buttons */
    frameLeftAct = new QAction(tr("Rewind"), this);
    frameRightAct = new QAction(tr("Forward"), this);
    frameLeftAct->setIcon(AppIcon("go-previous.png"));
    frameRightAct->setIcon(AppIcon("go-next.png"));
    frameLeftAct->setStatusTip(tr("Previous frame"));
    frameRightAct->setStatusTip(tr("Next frame"));
    frameLeftAct->setDisabled(true);
    frameRightAct->setDisabled(true);

    /* Create Signals */
    createSignals();

    /* language menu */
    Language * t; foreach(t, Language::loadAll()) {
        QAction * langAction = new QAction(t->name, this);
        langAction->setCheckable(true);
        languageActions.addAction(langAction);
        if (t->id == Language::defaultLang()->id) langAction->setChecked(true);
        connect(langAction, SIGNAL(triggered()), t, SLOT(setDefaultLang()));
        connect(langAction, SIGNAL(triggered()), this, SLOT(langUpdated()));
    }
}

void MainWindow::createSignals() {
    connect(newAct, SIGNAL(triggered()), this, SLOT(newInstance()));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
    connect(importAct, SIGNAL(triggered()), this, SLOT(importInstance()));

    connect(saveAct, SIGNAL(triggered()), tabwidget, SLOT(currentInstanceSave()));
    connect(saveAsAct, SIGNAL(triggered()), tabwidget, SLOT(currentInstanceSaveAs()));

    connect(newResourceAct, SIGNAL(triggered()), this, SLOT(newResource()));
    connect(closeAct, SIGNAL(triggered()), tabwidget, SLOT(closeSubWindow()));
    connect(newJobAct, SIGNAL(triggered()), this, SLOT(newJob()));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
    connect(newActivityAct, SIGNAL(triggered()), this, SLOT(newActivity()));
    connect(newPrecedenceAct, SIGNAL(triggered()), this, SLOT(newPrecedence()));
    connect(this, SIGNAL(keyPressEvent(QKeyEvent *)), this, SLOT(disablePrecedenceAdding(QKeyEvent *)));

    connect(removePrecedenceAct, SIGNAL(triggered()), this, SLOT(removePrecedence()));
    connect(clearPrecedencesAct, SIGNAL(triggered()), this, SLOT(clearPrecedences()));
    connect(autoClearPrecedencesAct, SIGNAL(triggered()), controller, SLOT(autoClearPrecedences()));

    connect(useHoursAct, SIGNAL(triggered()), this, SLOT(hoursTimelineChanged()));
    connect(manageTemplatesAct, SIGNAL(triggered()), this, SLOT(manageTemplates()));

    connect(paintFeasibleIntervalsAct, SIGNAL(triggered()), controller, SLOT(doPaintFeasibleIntervals()));
    connect(paintFlexibilityIntervalsAct, SIGNAL(triggered()), controller, SLOT(doFlexibilityIntervals()));

    connect(configureSolversAction, SIGNAL(triggered()), this, SLOT(configureSolvers()));
    connect(solveLastAction, SIGNAL(triggered()), controller, SLOT(solve()));
    connect(solveWithParamsAction, SIGNAL(triggered()), controller, SLOT(solveWithOptions()));

    connect(aboutAct, SIGNAL(triggered()), controller, SLOT(showAbout()));

    connect(zoomInAct, SIGNAL(triggered()), controller, SLOT(zoomIn()));
    connect(zoomOutAct, SIGNAL(triggered()), controller, SLOT(zoomOut()));

    connect(compareAct, SIGNAL(triggered()), this, SLOT(compareInstances()));

    connect(frameLeftAct, SIGNAL(triggered()), controller, SLOT(rewind()));
    connect(frameRightAct, SIGNAL(triggered()), controller, SLOT(forward()));
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(importAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(closeAct);

    separatorAct = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActs[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    updateRecentFileActions();

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(newResourceAct);
    editMenu->addAction(newJobAct);
    editMenu->addAction(newActivityAct);
    editMenu->addAction(newPrecedenceAct);
    editMenu->addAction(removePrecedenceAct);
    editMenu->addSeparator();
    editMenu->addAction(useHoursAct);
    editMenu->addSeparator();
    editMenu->addAction(clearPrecedencesAct);
    editMenu->addAction(autoClearPrecedencesAct);
    editMenu->addSeparator();
    editMenu->addAction(manageTemplatesAct);
    editMenu->addSeparator();
    editMenu->addAction(paintFeasibleIntervalsAct);
    editMenu->addAction(paintFlexibilityIntervalsAct);

    solveButtonMenu = new QMenu();
    solveMenu = menuBar()->addMenu(tr("&Solve"));
    updateSolverActions();
    
    langMenu = menuBar()->addMenu(tr("&Language"));
    languageActions.setExclusive(true);
    QAction * langAct; foreach(langAct, languageActions.actions()) langMenu->addAction(langAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::createToolbar() {
	QToolBar * toolbar = new QToolBar("Tools",this);
	toolbar->setObjectName("toolBar"); // to save its state in settings file
	toolbar->setFloatable(false);
	toolbar->setMovable(false);
	toolbar->addAction(newAct);
	toolbar->addAction(openAct);
	toolbar->addAction(saveAct);
    // toolbar->addAction(closeAct);
	toolbar->addSeparator();
	toolbar->addAction(newResourceAct);
	toolbar->addAction(newJobAct);
	toolbar->addAction(newActivityAct);
    toolbar->addAction(newPrecedenceAct);
    toolbar->addAction(removePrecedenceAct);
    toolbar->addSeparator();
    toolbar->addAction(zoomInAct);
    toolbar->addAction(zoomOutAct);
	toolbar->addSeparator();

    solveLastAction->setMenu(solveButtonMenu);
	toolbar->addAction(solveLastAction);
    toolbar->addAction(solveWithParamsAction);
	toolbar->addSeparator();
	toolbar->addAction(compareAct);
	toolbar->addSeparator();

    frameLabel = new QLabel(tr("%1 of %2").arg(0).arg(0));
    framesSlider = new QSlider(Qt::Horizontal);
    framesSlider->setDisabled(true);
    framesSlider->setSingleStep(1);
    framesSlider->setPageStep(1);
	toolbar->addAction(frameLeftAct);
	toolbar->addWidget(frameLabel);
    toolbar->addAction(frameRightAct);
	toolbar->addWidget(framesSlider);
	
	addToolBar(toolbar);
}

InstanceMdiWidget* MainWindow::getCurrentInstanceMdiWidget() {
    return tabwidget;
}

InstanceWidget* MainWindow::getCurrentInstanceWidget() {
    return tabwidget->currentInstanceWidget();
}

void MainWindow::disableIfInstanceEmpty(QAction* act) {
	disabledActions.append(act);
}

void MainWindow::setButtonState() {
	bool b = tabwidget->currentInstance() != 0;
    foreach(QAction* act, disabledActions) {
    	 act->setEnabled(b);
    	 if(!b)
    	     act->setChecked(false);
    }
}

void MainWindow::setupCentralWidget() {
    tabwidget = new InstanceMdiWidget(controller, framesSlider, this);
    setCentralWidget(tabwidget);
}

void MainWindow::langUpdated() {
    QMessageBox::information(this, tr("Language changed"), tr("Language changed. Restart application to view in chosen language."));
}

void MainWindow::hoursTimelineChanged() {
    controller->getCurrentInstance()->setHoursOnTimeline(useHoursAct->isChecked());
    getCurrentInstanceWidget()->updateTimelineOffset();
    getCurrentInstanceWidget()->enableActivities();
}

void MainWindow::synchronizeMenuWithInstance() {
    try{
        useHoursAct->setChecked(controller->getCurrentInstance()->hoursOnTimeline());
    } catch(NoInstanceException const& e) {}
}

void MainWindow::disablePrecedenceAdding(QKeyEvent * event) {
    try{
        if (event->key() == Qt::Key_Escape) {
            controller->getCurrentInstanceWidget()->disablePrecedenceAdding();
            controller->getCurrentInstanceWidget()->disablePrecedenceRemoving();
        }
    } catch(NoInstanceException const& e) {}
}

void MainWindow::invokeTestMethod(QString action, QString param) {
    if (action == "loadFile") {
        loadFile(param);
    }
    else if (action == "closeSubWindow") {
        tabwidget->closeSubWindow();
    }
    else if(action == "newInstance") {
        newInstance();
    }
}

void MainWindow::compareInstances() {
	CompareDialog* compareDialog = new CompareDialog(controller,this);
	compareDialog->exec();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event){
    if (event->type() == QEvent::ShortcutOverride) {
        QKeyEvent *ev = static_cast<QKeyEvent *>(event);
        if (ev->modifiers() == Qt::CTRL && ev->key() == Qt::Key_W) {
            QMdiSubWindow *sub = tabwidget->currentSubWindow();

            if (!sub) {
                close();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
