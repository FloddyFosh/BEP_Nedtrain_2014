#include "controller/instancecontroller.h"
#include "controller/exceptions.h"

#include <QPainter>
#include <QMenu>
#include <QEvent>
#include <QAction>
#include <cassert>

JobWidget::JobWidget(Job *j, InstanceController *controller, QWidget *parent, bool compare) :
    QWidget(parent), controller(controller), comparing(compare), _job(j), expanded(false)
{
    // set height fixed to sizehint
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    setMouseTracking(true);

    _hZoom = 8;
    _vZoom = 10;

    addActivities();
    addGroups();

    connect(_job, SIGNAL(jobChanged()), this, SLOT(doLayout()));
    connect(_job, SIGNAL(activityAdded(Activity *)), this, SLOT(addActivity(Activity *)));
    connect(_job, SIGNAL(groupAdded(Group *)), this, SLOT(addGroup(Group *)));
    connect(_job, SIGNAL(groupRemoved(Group *)), this, SLOT(removeGroup(Group *)));

    connect(_job, SIGNAL(activityRemoved()), this, SLOT(updateGeometry()));
    connect(_job, SIGNAL(jobChanged()), this, SLOT(updateGeometry()));
}

Job const *JobWidget::job() const {
    return _job;
}

void JobWidget::doExpand(bool expanded) {
    this->expanded = expanded;
    updateGeometry();
    raise();
}

void JobWidget::addActivities() {
    foreach(Activity *a, _job->getActivities()) {
        addActivity(a);
    }
}

void JobWidget::addGroups() {
    foreach(Group *g, _job->getGroups()) {
        addGroup(g);
    }
}

void JobWidget::addActivity(Activity *a) {
    ActivityWidget *aw = new ActivityWidget(a, controller, this, comparing);
    activityWidgets.append(aw);
    widgetMap.insert(a, aw);
    aw->setVisible(true);
    updateGeometry();
}

void JobWidget::removeActivity(ActivityWidget *aw){
	controller->stopDrawingDependencies(aw);
	activityWidgets.removeOne(aw);
	widgetMap.remove(aw->activity());
	_job->instance()->removeActivity(aw->activity());
	delete(aw);
	doLayout();
	updateGeometry();
}

void JobWidget::addGroup(Group *group) {
    QList<ActivityWidget *> widgets;
    foreach(Activity *a, group->getActivities()) {
        widgets.append(widgetMap.value(a));
        activityWidgets.removeOne(widgetMap.value(a));
    }
    GroupWidget *gw = new GroupWidget(group, widgets, this);
    groupWidgets.insert(group, gw);
    groupWidgetList.append(gw);
    gw->setVisible(true);
    updateGeometry();
}

void JobWidget::removeGroup(Group *group) {
    GroupWidget *gw = groupWidgets.value(group);
    foreach(ActivityWidget *aw, gw->getActivityWidgets()) {
        activityWidgets.append(aw);
    }
    delete gw;
    groupWidgets.remove(group);
    groupWidgetList.removeOne(gw);
}

ActivityWidget* JobWidget::getWidget(Activity* a) {
    return widgetMap.contains(a) ? widgetMap.value(a) : 0;
}

GroupWidget* JobWidget::getWidget(Group* g) {
    return groupWidgets.contains(g) ? groupWidgets.value(g) : 0;
}

QSize JobWidget::sizeHint() const {
    return minimumSize();
}

QSize JobWidget::minimumSize() const {
    int verticalBlocks = 2;
    if (expanded) {
        verticalBlocks += _job->groupCount();
    }
    else {
        verticalBlocks += 1;
    }
    int endTime = _job->dueDate();

    foreach(Group * g, _job->getGroups()) {
        endTime = max(endTime, g->getST() + g->getDuration());
    }
    return QSize(endTime * hZoom(), verticalBlocks * vZoom());
}

void JobWidget::paintEvent(QPaintEvent *e) {
    // native painting
    QWidget::paintEvent(e);
    QPainter painter(this);

    paintAvailability(painter);
    paintGroups();

    // draw feasibility intervals
    if (!comparing && controller->isPaintingFeasibleIntervals() && controller->getInstance()->getMaxFrameNr() != -1) {
        int yOffset = 1;

        foreach(GroupWidget *groupWidget, groupWidgetList) {
            groupWidget->determineFeasibleInterval(&painter, yOffset);
            groupWidget->paintFlexibilityInterval(&painter, yOffset);
            if (expanded) yOffset++;
        }
        foreach(ActivityWidget *activityWidget, activityWidgets) {
            activityWidget->determineFeasibleInterval(&painter, yOffset);
            activityWidget->paintFlexibilityInterval(&painter, yOffset);
            if (expanded) yOffset++;
        }
    }
    // finally draw bottom-border
    painter.setPen(Qt::darkGray);
    painter.drawLine(0, geometry().height() - 1, geometry().size().width(), geometry().size().height() - 1);
}

void JobWidget::paintAvailability(QPainter &painter) {
    painter.save();
    QRect available(hZoom() * (_job->releaseDate()+offsetX()), 0, hZoom() * (_job->dueDate() - _job->releaseDate()), height());
    painter.fillRect(available, QColor::fromHsvF(.3, 1, 1, .2));
    painter.restore();
}

void JobWidget::paintGroups() {
    int yOffset = 1;
    foreach(GroupWidget *gw, groupWidgetList) {
        gw->setPosition(offsetX(), yOffset);
        assert(gw->parentWidget()==this);
        if(expanded) yOffset++;
    }
}

void JobWidget::paintFeasibilityInterval(QPainter &painter, int xStart, int xEnd, int height, int yOffset){
	int yStart = vZoom() * yOffset;
	int yEnd   = vZoom() * yOffset + height;
	xEnd -= 2;
	painter.setPen(QPen (QColor(170,215,170), 1, Qt::SolidLine));
	painter.drawLine(xStart,(yStart+yEnd)/2,xEnd,(yStart+yEnd)/2);
	painter.setPen(QPen (Qt::darkGray, 1, Qt::SolidLine));
	painter.drawLine(xStart,yStart,xStart,yEnd);
	painter.drawLine(xEnd,  yStart,xEnd,  yEnd);
}

void JobWidget::paintFlexibilityInterval(QPainter &painter, int xStart, int xEnd, int height, int yOffset) {
    int yStart = vZoom() * yOffset;
    int yEnd   = vZoom() * yOffset + height;
    xEnd -= 2;
    painter.setPen(QPen (QColor(0,0,255), 1, Qt::SolidLine));
    painter.drawLine(xStart,(yStart+yEnd)/2,xEnd,(yStart+yEnd)/2);
    painter.setPen(QPen (QColor(0,0,255), 1, Qt::SolidLine));
    painter.drawLine(xStart,yStart,xStart,yEnd);
    painter.drawLine(xEnd,  yStart,xEnd,  yEnd);
}

void JobWidget::resizeEvent(QResizeEvent *e) {
    doLayout();
    e->accept();
}

bool JobWidget::event(QEvent *e) {
    if (e->type() == QEvent::LayoutRequest) {
        doLayout();
        e->accept();
        return true;
    } else {
        return QWidget::event(e);
    }
}

void JobWidget::updateGeometry() {
    QWidget::updateGeometry();
}

void JobWidget::doLayout() {
    int y_offset = 1;

    int aantal = 0;
    // update group locations
    foreach(GroupWidget *gw, groupWidgetList) {
        foreach(ActivityWidget *aw, gw->getActivityWidgets()) {
            aantal ++;
            aw->updateActivityPosition(y_offset);
        }
        if (expanded) y_offset++;
    }

    // update locations of remaining activities
    foreach(ActivityWidget *aw, activityWidgets) {
        aantal ++;
        aw->updateActivityPosition(y_offset);
        if (expanded) y_offset++;
    }
    assert(aantal == _job->getActivities().size());
    update();
}

QList<ActivityWidget*> JobWidget::getActivityWidgets() {
    QList<ActivityWidget*> widgets;
    foreach(ActivityWidget *aw, activityWidgets)
        widgets.append(aw);
    foreach(GroupWidget *gw, groupWidgetList)
        foreach(ActivityWidget *aw, gw->getActivityWidgets())
            widgets.append(aw);

    return widgets;
}

void JobWidget::contextMenuEvent(QContextMenuEvent *event){
    if(comparing) return;
    QMenu menu(this);
    QAction* showResourceUsage = new QAction(tr("Show resource usage"), this);
    connect(showResourceUsage, SIGNAL(triggered()), this, SLOT(showResourceUsage()));
    menu.addAction(showResourceUsage);
    menu.exec(event->globalPos());
}

void JobWidget::mousePressEvent(QMouseEvent *event) {
    if (comparing) return;

    if (event->button() == Qt::LeftButton)
        controller->stopDrawingDependencies();
}

void JobWidget::showResourceUsage(){
	controller->shadeResources(_job);
}

void JobWidget::setHZoom(int z) {
    _hZoom = z;
}
void JobWidget::setVZoom(int z) {
    _vZoom = z;
}

int JobWidget::hZoom() const {
    return _hZoom;
}

int JobWidget::vZoom() const {
    return _vZoom;
}

int JobWidget::offsetX() {
    int offset = 0;
    try {
        if(!comparing && _job->instance()->hoursOnTimeline())
            offset = _job->instance()->getTimeOffset();
    } catch(NoInstanceException const& e) {
        e.showErrorMessage(tr("The program encountered a failure. Please restart."), controller->getParent()->getMainWindow());
    }
    return offset;
}
