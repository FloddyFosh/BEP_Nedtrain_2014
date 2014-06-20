#include <QScrollBar>
#include <QBoxLayout>
#include <QLayout>
#include <QAction>

#include "controller/instancecontroller.h"
#include "controller/exceptions.h"
#include "model/frame.h"
#include "model/chain.h"
#include "widgets/app_icon.h"

InstanceWidget::InstanceWidget(Instance * instance, Controller *controller, QWidget *parent) :
    AbstractInstanceWidget(instance, Qt::Vertical, parent), controller (controller), frameNumber(0)
{
    instanceController = new InstanceController(instance, this, controller);
    estGen = new EST_Generator(instanceController);
    hZoom = 8;
    vZoom = 10;
    selectedActivity = 0;
    blurTime = -1;
    precedenceEditing = 0;

    createViewers();
    setSplitterSizes();
    connectViewerSignals();
    initFromInstance();
}

InstanceWidget::~InstanceWidget() {
    delete instance;
    delete estGen;
}

void InstanceWidget::initFromInstance() {
    initJobs();
    initResources();

    connect(instance, SIGNAL(jobAdded(Job*)), this, SLOT(addJob(Job*)));
    connect(instance, SIGNAL(resourceAdded(Resource*)), this, SLOT(addResource(Resource*)));
}

void InstanceWidget::initResources() {
    resourceHeadersLayout->addStretch();
    resourcesLayout->addStretch();
    foreach(Resource * r, instance->getResources())
        addResource(r);
}

void InstanceWidget::setSplitterSizes() {
    QList<int> sizes;
    sizes << height() / 2 << height() / 2;
    setSizes(sizes);
    jobSplitter->setSizes(sizes);
    resourceSplitter->setSizes(sizes);
}

void InstanceWidget::connectViewerSignals() {
    AbstractInstanceWidget::connectViewerSignals();

    connect(jobsZoomable, SIGNAL(dimensionChanged(const QSize)), this, SLOT(resizeOverlay(const QSize)));
    connect(jobsZoomable, SIGNAL(mouseMoved(int,int)), this, SLOT(moveOverlayMouse(int,int)));

    connectVerticalScrollers(resourceHeaderScroller, resourcesScroller);

    connectHorizontalScrollers(jobsScroller, resourcesScroller);
    connectHorizontalScrollers(resourcesScroller, resourceTimelineScroller);

    connectZoomables(jobsZoomable, resourcesZoomable);
    connectZoomables(resourcesZoomable, resourceTimeline);
}

void InstanceWidget::createViewers() {
    jobSplitter = new OtsSplitter;
    resourceSplitter = new OtsSplitter;

    createJobSplitter(jobSplitter);
    createResourceSplitter();
    connectSplitters(jobSplitter, resourceSplitter);

    overlay = new OverlayWidget(instanceController, jobsZoomable);

    jobTimeline->installEventFilter(this);
    jobsZoomable->installEventFilter(this);
    resourceTimeline->installEventFilter(this);
    resourcesZoomable->installEventFilter(this);

    addWidget(jobSplitter);
    addWidget(resourceSplitter);
}

bool InstanceWidget::eventFilter(QObject *o, QEvent *e) {
    //casts to verify type of sending object
    JobWidget *jw = qobject_cast<JobWidget *>(o);
    ResourceWidget *rw = qobject_cast<ResourceWidget *>(o);
    ActivityWidget *aw = qobject_cast<ActivityWidget *>(o);

    if(e->type() == QEvent::Leave && (o==jobsZoomable || o==resourcesZoomable || o==jobTimeline ||
            o == resourceTimeline)){ // hide vertical mouse follower on mouse leave event
        setMouseX(-1);
    }
    // replicate change of width of jobs widget to resources widget
    else if (o == jobsZoomable && e->type() == QEvent::Resize) {
        QResizeEvent *resizeEvent = static_cast<QResizeEvent *>(e);

        jobTimeline->setFixedWidth(resizeEvent->size().width());
        resourcesZoomable->setFixedWidth(resizeEvent->size().width());
        resourceTimeline->setFixedWidth(resizeEvent->size().width());

    }
    else if (e->type() == QEvent::MouseMove && (o == jobTimeline || o == jobsZoomable || o == resourceTimeline ||
                                                  o == resourcesZoomable || jw || rw || aw)) {
        // replicate mouse cursor follower
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
        setMouseX(mouseEvent->x());
    }
    else if (e->type() == QEvent::Wheel && (o == jobTimeline || o == jobsZoomable || o == resourceTimeline ||
            o == resourcesZoomable)) {

        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(e);
        if(wheelEvent->modifiers().testFlag(Qt::ControlModifier)) {
            //Control button held down
            if(wheelEvent->delta() > 0) controller->zoomOut();
            else controller->zoomIn();
            wheelEvent->accept();
            return true;
        }
        else if(wheelEvent->modifiers().testFlag(Qt::ShiftModifier)) {
            //Shift button held down
            QScrollBar * slider = jobsScroller->horizontalScrollBar();
            int sliderv = slider->value();
            slider->setSliderPosition(slider->value() + wheelEvent->delta());
            setMouseX(wheelEvent->x() - (sliderv-slider->value()));
            wheelEvent->accept();
            return true;
        }
    }
    
    return QWidget::eventFilter(o, e);
}

void InstanceWidget::setMouseX(int x) {
    jobTimeline->setMouseX(x);
    jobsZoomable->setMouseX(x);
    resourceTimeline->setMouseX(x);
    resourcesZoomable->setMouseX(x);
}

void InstanceWidget::createResourceSplitter() {
    resourceHeaderWidget = new QWidget;
    resourceHeaderLayout = new QVBoxLayout;
    resourceWidgetTitleBar = new QWidget;
    resourceWidgetTitleBarLayout = new QHBoxLayout;
    resourceWidgetTitle = new QLabel(tr("<b>Resources</b>"));
    resourceWidgetViewButton = new QPushButton();
    resourceHeaderScroller = new QScrollArea;
    resourceHeaders = new QWidget;
    resourceHeadersLayout = new QVBoxLayout;
    resourcesViewer = new QWidget;
    resourcesScroller = new QScrollArea;
    resourcesZoomable = new Zoomable(instanceController, this);
    resourcesLayout = new QVBoxLayout;
    resourceTimelineScroller = new QScrollArea;
    resourceTimeline = new Timeline(instanceController, this, false, false);

    resourceWidgetTitleBar->setLayout(resourceWidgetTitleBarLayout);
    resourceWidgetTitleBarLayout->addSpacing(25);
    resourceWidgetTitleBarLayout->addWidget(resourceWidgetTitle);
    resourceWidgetTitleBarLayout->addWidget(resourceWidgetViewButton,0,Qt::AlignVCenter);
    resourceWidgetTitle->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    resourceWidgetTitle->setMargin(0);
    resourceWidgetTitle->setIndent(0);
    resourceWidgetViewButton->setMaximumSize(QSize(25,25));
    resourceWidgetViewButton->setCheckable(true);
    resourceWidgetViewButton->setIcon(AppIcon("resourceView.png"));
    resourceWidgetViewButton->setIconSize(QSize(15,15));
    resourceWidgetViewButton->setStatusTip(tr("Toggle the resource matrix view"));

    createHeaders(resourceSplitter, resourceHeaderWidget, resourceHeaderLayout, resourceWidgetTitleBar, resourceHeaders, resourceHeadersLayout, resourceHeaderScroller);
    createViewer(resourceSplitter, resourcesViewer, resourcesScroller, resourcesZoomable, resourcesLayout, resourceTimeline, resourceTimelineScroller);

    connect(resourceWidgetViewButton,SIGNAL(toggled(bool)),this,SLOT(viewButtonTriggeredSlot(bool)));
}

void InstanceWidget::addJob(Job *j) {
    JobHeaderWidget *jhw = new JobHeaderWidget(j, instanceController, jobHeaders, false);
    jobHeaderWidgets.insert(j->id(), jhw);
    jobHeadersLayout->insertWidget(jobHeadersLayout->count() - 1, jhw);

    JobWidget *jw = new JobWidget(j, instanceController, jobsZoomable, false);
    jw->installEventFilter(this);
    jobWidgets.insert(j->id(), jw);
    jobsLayout->insertWidget(jobsLayout->count() - 1, jw);

    connect(jhw, SIGNAL(expanded(bool)), jw, SLOT(doExpand(bool)));
    connect(jhw, SIGNAL(remove(Job *)), this, SLOT(removeJob(Job *)));
}

void InstanceWidget::removeJob(Job *j) {
    JobHeaderWidget *jhw = jobHeaderWidgets.value(j->id());
    JobWidget *jw = jobWidgets.value(j->id());
    jw->removeEventFilter(this);

    disconnect(jhw, SIGNAL(expanded(bool)), jw, SLOT(doExpand(bool)));
    disconnect(jhw, SIGNAL(remove(Job *)), this, SLOT(removeJob(Job *)));

    jobHeadersLayout->removeWidget(jhw);
    jobsLayout->removeWidget(jw);
    jobHeaderWidgets.remove(j->id());
    jobWidgets.remove(j->id());
    delete jhw;
    delete jw;

    instance->removeJob(j);
}

void InstanceWidget::addResource(Resource *r) {
    ResourceHeaderWidget *rhw = new ResourceHeaderWidget(r, instanceController, resourceHeaders);
    rHeaderWidgets.insert(r->id(), rhw);
    resourceHeadersLayout->insertWidget(resourceHeadersLayout->count() - 1, rhw);

    ResourceWidget *rw = new ResourceWidget(r, instanceController, resourcesZoomable);
    rw->installEventFilter(this);
    rWidgets.insert(r->id(), rw);
    resourcesLayout->insertWidget(resourcesLayout->count() - 1, rw);
    resourceWidgets.append(rw);

    connect(rhw, SIGNAL(remove(Resource *)), this, SLOT(removeResource(Resource *)));
}

void InstanceWidget::removeResource(Resource *r) {
    ResourceHeaderWidget *rhw = rHeaderWidgets.value(r->id());
    ResourceWidget *rw = rWidgets.value(r->id());
    rw->removeEventFilter(this);

    disconnect(rhw, SIGNAL(remove(Resource *)), this, SLOT(removeResource(Resource *)));
    resourceHeadersLayout->removeWidget(rhw);
    resourcesLayout->removeWidget(rw);
    resourceWidgets.removeOne(rw);
    rWidgets.remove(r->id());

    delete rhw;
    delete rw;

    instance->removeResource(r->id());
}

int InstanceWidget::getJobIndex(JobHeaderWidget *jhw) {
    return jobHeadersLayout->indexOf(jhw);
}

int InstanceWidget::getResourceIndex(ResourceHeaderWidget *rhw) {
    return resourceHeadersLayout->indexOf(rhw);
}

void InstanceWidget::relocateJobWidget(int from, int to) {
    //Index shouldn't be outside of scope.
    if(from < 0 || to < 0) return;
    if(from > jobHeadersLayout->count()-2 || to > jobHeadersLayout->count()-2) return;

    JobHeaderWidget *jhw = static_cast<JobHeaderWidget*>(jobHeadersLayout->itemAt(from)->widget());
    JobWidget *jw = static_cast<JobWidget*>(jobsLayout->itemAt(from)->widget());

    jobHeadersLayout->removeWidget(jhw);
    jobsLayout->removeWidget(jw);
    jobHeadersLayout->insertWidget(to, jhw);
    jobsLayout->insertWidget(to, jw);
    jw->raise();
}

void InstanceWidget::relocateResourceWidget(int from, int to) {
    //Index shouldn't be outside of scope.
    if(from < 0 || to < 0) return;
    if(from > resourceHeadersLayout->count()-2 || to > resourceHeadersLayout->count()-2) return;

    ResourceHeaderWidget *rhw = static_cast<ResourceHeaderWidget*>(resourceHeadersLayout->itemAt(from)->widget());
    ResourceWidget *rw = static_cast<ResourceWidget*>(resourcesLayout->itemAt(from)->widget());

    resourceHeadersLayout->removeWidget(rhw);
    resourcesLayout->removeWidget(rw);
    resourceHeadersLayout->insertWidget(to, rhw);
    resourcesLayout->insertWidget(to, rw);
    rw->raise();
}

void InstanceWidget::disconnectActivitiesFromResourceWidgets() {
    foreach(ResourceWidget * rw, resourceWidgets) rw->disconnectActivities();
}

void InstanceWidget::reconnectActivitiesToResourceWidgets() {
    foreach(ResourceWidget * rw, resourceWidgets) rw->connectActivities();
}

void InstanceWidget::repaintJobWidgets() {
    foreach(JobWidget * jw, jobWidgets) {
        jw->repaint();
    }
}

void InstanceWidget::repaintResourceWidgets() {
    foreach(ResourceWidget * rw, resourceWidgets) rw->calculateResourceProfile();
}

QMap<int, JobWidget*> InstanceWidget::getJobWidgets() {
    return jobWidgets;
}

QMap<int, ResourceWidget*> InstanceWidget::getResourceWidgets() {
    return rWidgets;
}

QList<ActivityWidget*> InstanceWidget::getActivityWidgets() {
    QList<ActivityWidget*> allActivities;
    QMapIterator<int, JobWidget*> jobs(getJobWidgets());
    while(jobs.hasNext()) {
        JobWidget *job = jobs.next().value();
        QList<ActivityWidget*> activities = job->getActivityWidgets();
        allActivities.append(activities);
    }
    return allActivities;
}

void InstanceWidget::resizeOverlay(const QSize size) {
    overlay->resize(size);
}

void InstanceWidget::moveOverlayMouse(int x, int y) {
    overlay->moveLine(x,y);
}

OverlayWidget* InstanceWidget::getOverlay() {
    return overlay;
}

QScrollArea* InstanceWidget::getResourceScrollArea(){
    return resourcesScroller;
}

void InstanceWidget::toFrameNumber(int frameNr) {
    if (instance->getMaxFrameNr() == -1) return;
    frameNumber = frameNr;

    setupButtons();
    instanceController->stopDrawingDependencies();
    disconnectActivitiesFromResourceWidgets();
    estGen->gotoFrame(frameNr);
    reconnectActivitiesToResourceWidgets();
    repaintResourceWidgets();

    QList<Precedence *> precedences_added (estGen->getAdded(frameNr));
    controller->startPaintingFramePrecedences(precedences_added);

    foreach(int i, instance->getJobs().keys()) {
        instanceController->highlightJob(i, false);
    }
    foreach(int i, instance->getResources().keys()) {
        instanceController->highlightResource(i, false);
    }

    Frame* frame = instance->getFrame(frameNr);
    foreach(int i, frame->getAffectedJobIds()) {
        instanceController->highlightJob(i, true);
    }
    foreach(int i, frame->getAffectedResIds()) {
        instanceController->highlightResource(i, true);
    }
    if(Chain* c = frame->getChain()) {
        instanceController->focusResource(c->resourceId());
    }
}

void InstanceWidget::toLastFrame() {
    toFrameNumber(controller->getFramesSlider()->maximum());
}

void InstanceWidget::setupButtons() {
    QSlider *slider = controller->getFramesSlider();
    QLabel *frameLabel = controller->getMainWindow()->frameLabel;
    QAction *prevButton = controller->getMainWindow()->frameLeftAct;
    QAction *nextButton = controller->getMainWindow()->frameRightAct;
    prevButton->setDisabled(frameNumber <= 0);
    nextButton->setDisabled(frameNumber >= slider->maximum());
    frameLabel->setText(tr("%1 of %2").arg(frameNumber+1).arg(slider->maximum()+1));
    slider->setValue(frameNumber);
}

void InstanceWidget::setupSlider(bool reset) {
    QSlider * theSlider = controller->getFramesSlider();
    if (reset) {
        frameNumber = 0;
    }
    theSlider->setRange(0, instance->getMaxFrameNr());
    theSlider->setDisabled(instance->getMaxFrameNr() == -1);
    theSlider->setValue(frameNumber);
    setupButtons();
}

void InstanceWidget::updateTimelineOffset() {
    instance->updateStartTime();

    jobTimeline->update();
    resourceTimeline->update();

    foreach(JobWidget *j, jobWidgets)
        j->doLayout();

    foreach(ResourceWidget *r, rWidgets)
        r->updatePixmap();

    updateWidgetsLayout();
}

void InstanceWidget::addVerticalLine(int xPos, int resKey) {
    resourcesZoomable->addVerticalLine(xPos);
    jobsZoomable->addVerticalLine(xPos);
    rWidgets[resKey]->addResourceMark(xPos);
}

void InstanceWidget::removeVerticalLines() {
    resourcesZoomable->clearVerticalLines();
    jobsZoomable->clearVerticalLines();

    foreach(ResourceWidget* rw, resourceWidgets)
        rw->clearResourceMarks();
}

int InstanceWidget::getFrameNumber() const {
    return frameNumber;
}

bool InstanceWidget::isAtLastFrame() const {
    return frameNumber == instance->getMaxFrameNr();
}

void InstanceWidget::zoomIn() {
    if(hZoom <= (1<<4)) hZoom *= 2;
    setZoom(hZoom);
}

void InstanceWidget::zoomOut() {
    if(hZoom >= 2) hZoom /= 2;
    setZoom(hZoom);
}

void InstanceWidget::setZoom(int z) {
    jobsZoomable->setHZoom(z);
    overlay->setHZoom(z);

    foreach(JobWidget *jw, jobWidgets) {
        jw->setHZoom(z);
        jw->updateGeometry();
    }

    foreach(ResourceWidget *rw, resourceWidgets) rw->setHZoom(z);
}

void InstanceWidget::updateZoom() {
    setZoom(hZoom);
}

void InstanceWidget::updateWidgetsLayout() {
    jobsLayout->update();
    resourcesLayout->update();
    raise();
}

InstanceController * InstanceWidget::getInstanceController() {
    return instanceController;
}

void InstanceWidget::enablePrecedenceRemoving() {
    setPrecedenceEditing(true);
    foreach(ActivityWidget *aw, getActivityWidgets())
        connect(aw, SIGNAL(activitySelected(ActivityWidget *)), this, SLOT(removePrecedencePoint(ActivityWidget *)));
}

void InstanceWidget::disablePrecedenceRemoving() {
    setPrecedenceEditing(false);
    foreach(ActivityWidget *aw, getActivityWidgets())
        disconnect(aw, SIGNAL(activitySelected(ActivityWidget *)), this, SLOT(removePrecedencePoint(ActivityWidget *)));
    controller->getMainWindow()->uncheckPrecedenceRemoving();
}

void InstanceWidget::enablePrecedenceAdding() {
    setPrecedenceEditing(true);
    foreach(ActivityWidget *aw, getActivityWidgets())
        connect(aw, SIGNAL(activitySelected(ActivityWidget *)), this, SLOT(addPrecedencePoint(ActivityWidget *)));
}

void InstanceWidget::disablePrecedenceAdding() {
    setPrecedenceEditing(false);
    foreach(ActivityWidget *aw, getActivityWidgets())
        disconnect(aw, SIGNAL(activitySelected(ActivityWidget *)), this, SLOT(addPrecedencePoint(ActivityWidget *)));
    controller->getMainWindow()->uncheckPrecedenceAdding();
}

void InstanceWidget::setPrecedenceEditing(bool editing) {
    precedenceEditing = editing;
    selectedActivity = 0;
    getOverlay()->unsetPrecedencePoint(); // remove arrow if there was one
}

bool InstanceWidget::isPrecedenceEditing() {
    return precedenceEditing;
}

void InstanceWidget::addPrecedencePoint(ActivityWidget *aw) {
    Activity* activity = aw->activity();
    if(selectedActivity) {
        try {
            instance->addPrecedence(selectedActivity, activity, true);
            instance->setUserChanges(true);
            instance->emitChanged();
        } catch(InstanceManipulationException const& e) {
            e.showErrorMessage(controller->getMainWindow());
        }
        disablePrecedenceAdding();
    } else {
        selectedActivity = activity;
        getOverlay()->setPrecedencePoint(aw->getLocation()); //show arrow while adding
    }
}

void InstanceWidget::removePrecedencePoint(ActivityWidget *aw) {
    Activity* activity = aw->activity();
    if(selectedActivity) {
        try {
            instance->removePrecedence(selectedActivity, activity);
            instance->removePrecedence(activity, selectedActivity);
            instance->setUserChanges(true);
            instance->emitChanged();
        } catch(InstanceManipulationException const& e) { e.printWarning(); }
        disablePrecedenceRemoving();
    } else {
        selectedActivity = activity;
    }
}

bool InstanceWidget::hasAddedFirstPrecedencePoint() {
    return selectedActivity != 0;
}

void InstanceWidget::disableActivitiesBeforeTime(int t) {
    foreach(ActivityWidget* aw, getActivityWidgets()) {
        if (aw->activity()->st()+aw->activity()->duration() <= t)
            aw->setBlur(true);
        else
            aw->setBlur(false);
    }
    blurTime = t;
}

void InstanceWidget::disableActivitiesBeforeTime() {
    disableActivitiesBeforeTime(blurTime);
}

void InstanceWidget::enableActivities() {
    foreach(ActivityWidget* aw, getActivityWidgets())
        aw->setBlur(false);
    blurTime = -1;
    jobTimeline->removeDoneInterval();
}

void InstanceWidget::removePeaks() {
    foreach(ResourceWidget *rw, getResourceWidgets()) rw->removePeak();
}

void InstanceWidget::focusResourceWidget(int resId) {
    ResourceHeaderWidget* rh = rHeaderWidgets.value(resId);
    int rhPosition = getResourceIndex(rh);

    QScrollBar* sb = resourcesScroller->verticalScrollBar();
    if(resourceHeadersLayout->count() <= 2) return;
    double size = (double) resourceHeadersLayout->count();
    double pos = rhPosition / (size-2.0);
    sb->setSliderPosition(pos * sb->maximum());
}

void InstanceWidget::highlightResource(int resId, bool hl) {
    rHeaderWidgets.value(resId)->highlight(hl);
}

void InstanceWidget::highlightJob(int jobId, bool hl) {
    jobHeaderWidgets.value(jobId)->highlight(hl);
}

void InstanceWidget::viewButtonTriggeredSlot(bool toggled){
    emit viewButtonTriggered(toggled);
}
