#include <cmath>
#include <QPainter>
#include <QMenu>
#include <QAction>
#include <QDebug>

#include "dialogs/activitydialog.h"
#include "dialogs/activityinfodialog.h"
#include "dialogs/flexibilitydialog.h"
#include "controller/instancecontroller.h"

ActivityWidget::ActivityWidget(Activity *a, InstanceController *controller, JobWidget *parent, bool compare) : QWidget(parent),
    _activity(a), parent(parent), controller(controller), comparing(compare), stPropagator (NULL)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMouseTracking(true);
    setToolTip(_activity->name());
    selected = false;
    highlight= false;
    shade = false;
    changeDuration = false;
    selectedX = -1;
    oldST     = _activity->st();
    blurred = false;
    diff = false;
    color = Qt::gray;

    connect(a, SIGNAL(activityChanged()), this, SLOT(updateGeometry()));
    connect(a, SIGNAL(activityChanged()), this, SLOT(updateToolTip()));
    connect(this, SIGNAL(activitySelected(ActivityWidget *)), this, SLOT(showDependencies()));
}

QPoint ActivityWidget::getLocation() {
	return getLocation(width()/2);
}

QPoint ActivityWidget::getFrontLocation() {
    return getLocation(width()/3);
}

QPoint ActivityWidget::getTailLocation(){
	return getLocation(width()/3*2);
}

QPoint ActivityWidget::getLocation(int offsetX) {
    int xPos = pos().x() + offsetX + parent->pos().x();
    int yPos = pos().y() + height()/2 + parent->pos().y();
    return QPoint(xPos,yPos);
}

QList<ActivityWidget*> ActivityWidget::getAdjacentWidgets(AdjacentWidgets type){
	return getAdjacentWidgets(type, _activity);
}

QList<ActivityWidget*> ActivityWidget::getAdjacentWidgets(AdjacentWidgets type, Activity* a) {
	QList<ActivityWidget*> adjacentWidgets;
	QList<Precedence *> precedences = type==InComing ? a->getIncomingPrecedences() : a->getOutgoingPrecedences();
	foreach(Precedence* p, precedences){
        if(!p->isPosted(controller->getInstanceWidget()->getFrameNumber()) || p->isDisabled())
            continue;

		Job * jobOfActivity = type==InComing ? p->a1()->job() : p->a2()->job();
		JobWidget * actparent = controller->getInstanceWidget()->getJobWidgets().value(jobOfActivity->id());

		ActivityWidget * aw = type==InComing ? actparent->getWidget(p->a1()) : actparent->getWidget(p->a2());

		if(aw)
			adjacentWidgets.append(aw);

	}
	return adjacentWidgets;
}

void ActivityWidget::setHighlight(bool h){
	highlight = h;
	repaint();
}

QSize ActivityWidget::sizeHint() const {
    return QSize(_activity->duration() * parent->hZoom(), parent->vZoom());
}

void ActivityWidget::setSelected(bool s){
	selected = s;
}

void ActivityWidget::setShade(bool s){
	shade = s;
	repaint();
}

void ActivityWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QSize hint = sizeHint();
    QRect activityRect(1, 1, hint.width() - 2, hint.height() - 2);

    painter.setRenderHint(QPainter::Antialiasing, true);
    QColor pencolor = Qt::black;

    QColor activityColor = color;
    if (!comparing) {
        activityColor = controller->getColorFor(_activity->name());
    }

    if(!comparing && highlight){
    	int h = activityColor.hue();
    	activityColor = QColor::fromHsv(h, 50, 255);
    	pencolor = QColor::fromHsv(h,255,255);
    }

    if(!comparing && blurred){
        activityColor = QColor::fromHsv(activityColor.hue(), activityColor.saturation() / 4, 255);
        pencolor = Qt::gray;
    }

    painter.setBrush(QBrush(activityColor));
    painter.setPen(pencolor);
    painter.drawRoundedRect(activityRect, 4, 4);

    if(!comparing && shade){
    	painter.setBrush(QBrush(Qt::black, Qt::FDiagPattern));
    	painter.setPen(Qt::NoPen);
    	painter.drawRoundedRect(activityRect, 4, 4);
    }

}

Activity *ActivityWidget::activity(){
    return _activity;
}

void ActivityWidget::showActivityDialog(){
    ActivityDialog dialog(_activity->job()->instance(), _activity, this);
    dialog.exec();
}

void ActivityWidget::showActivityInfoDialog(){
    ActivityInfoDialog dialog(_activity->job()->instance(), _activity, this);
    dialog.exec();
}

void ActivityWidget::removeWidget(){
	parent->removeActivity(this);
}

void ActivityWidget::lock(bool locked) {
    controller->getInstance()->activityGroup(_activity)->setLocked(locked);
    controller->getInstance()->setUserChanges(true);
}

void ActivityWidget::showDependencies(){
    if(controller->getInstanceWidget()->isPrecedenceEditing()) {
        if(!controller->getInstanceWidget()->hasAddedFirstPrecedencePoint())
            controller->showDependencies(this);
    }
    else {
        ActivityWidget * aw_selected = controller->getDependencyWidget();
        
        if (aw_selected != NULL and aw_selected->_activity->group() == _activity->group())
            controller->stopDrawingDependencies(aw_selected);
        else
            controller->showDependencies(this);
    }
}

void ActivityWidget::moveLeft() {
    controller->getInstance()->setUserChanges(true);
    controller->getInstance()->activityGroup(_activity)->moveLeft(_activity);
    controller->getInstanceWidget()->disableActivitiesBeforeTime();
}

void ActivityWidget::moveRight() {
    controller->getInstance()->setUserChanges(true);
    controller->getInstance()->activityGroup(_activity)->moveRight(_activity);
    controller->getInstanceWidget()->disableActivitiesBeforeTime();
}

void ActivityWidget::setFlexibility() {
    FlexibilityDialog dialog(controller, _activity, this);
    dialog.exec();
}


void ActivityWidget::mouseMoveEvent(QMouseEvent *e){
    if(comparing) return;
	//workaround for showing vertical mouse follower when mouse is on activity widget
	controller->getInstanceWidget()->setMouseX(pos().x()+e->x());
	controller->getInstanceWidget()->moveOverlayMouse(getFrontLocation().x(), getFrontLocation().y());

	//set cursor
	if(e->x() < sizeHint().width()-5)
		if(selectedX != - 1 && controller->isAtLastFrame()) //user is moving activity and at last solved frame
			setCursor(Qt::ClosedHandCursor);
		else
			setCursor(Qt::ArrowCursor);
	else if(selectedX == -1 and _activity->group()->size() == 1) // user is not moving this activity
		setCursor(Qt::SizeHorCursor);

	if(changeDuration){
		int newDuration = round((double)e->x()/(double)parent->hZoom());
		if(_activity->duration() != newDuration) {
		    if (!controller->isAtLastFrame())
			    _activity->setDuration(newDuration);
			else
			    stPropagator->changeDurationIfPossible(newDuration);
		    controller->getInstance()->setUserChanges(true);
		}
	}
	else if (selectedX != -1 && controller->isAtLastFrame()){
	    int moveDistance = round((double)(e->x()+pos().x()-selectedX)/(double)parent->hZoom());
	    controller->stopDrawingDependencies(this);
        stPropagator->moveIfPossible(moveDistance);
	}
}

void ActivityWidget::mousePressEvent(QMouseEvent *event) {
    if (comparing) return;

    setSelected(!selected);
    if (event->button() == Qt::LeftButton) {
        stPropagator = new ST_Propagator (controller, _activity->group());
        if (event->x() > sizeHint().width() - 5) {
            if (_activity->group()->size() == 1)
                changeDuration = true;
        } else {
            emit activitySelected(this);
            selectedX = event->x() + pos().x();
        }
    }
}

void ActivityWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        changeDuration = false;
        selectedX = -1;
        if (stPropagator) {
            delete stPropagator;
            stPropagator = NULL;
        }
    }
}

void ActivityWidget::contextMenuEvent(QContextMenuEvent *event) {
    if (comparing) return;

    QMenu myMenu;
    infoAct = new QAction(tr("&Properties"), this);
    infoAct->setStatusTip(tr("Show info about activity %1").arg(_activity->name()));
    connect(infoAct, SIGNAL(triggered()), this, SLOT(showActivityInfoDialog()));

    editAct = new QAction(tr("&Edit"), this);
    editAct->setStatusTip(tr("Edit activity %1").arg(_activity->name()));
    connect(editAct, SIGNAL(triggered()), this, SLOT(showActivityDialog()));

    removeAct = new QAction(tr("&Remove"), this);
    removeAct->setStatusTip(tr("Remove activity %1").arg(_activity->name()));
    connect(removeAct, SIGNAL(triggered()), this, SLOT(removeWidget()));

    dependenciesAct = new QAction(tr("&Show dependencies"), this);
    dependenciesAct->setStatusTip(tr("Show dependencies of %1").arg(_activity->name()));
    dependenciesAct->setCheckable(true);
    dependenciesAct->setChecked(controller->getDependencyWidget() == this);
    connect(dependenciesAct, SIGNAL(triggered()), this, SLOT(showDependencies()));

    lockAct = new QAction(tr("&Lock position"), this);
    lockAct->setStatusTip(tr("Lock %1 at this position").arg(_activity->name()));
    lockAct->setCheckable(true);
    lockAct->setChecked(controller->getInstance()->activityGroup(_activity)->isLocked());
    connect(lockAct, SIGNAL(triggered(bool)), this, SLOT(lock(bool)));

    myMenu.addAction(infoAct);
    myMenu.addAction(editAct);
    myMenu.addAction(removeAct);
    myMenu.addAction(dependenciesAct);
    myMenu.addAction(lockAct);

    Group *g = controller->getInstance()->activityGroup(_activity);

    if (g->getST() != _activity->st()) {
        moveLeftAct = new QAction(tr("&Move left"), this);
        moveLeftAct->setStatusTip(tr("Move the activity to the left."));
        myMenu.addAction(moveLeftAct);
        connect(moveLeftAct, SIGNAL(triggered()), this, SLOT(moveLeft()));
    }
    if (g->getST() + g->getDuration()
            != _activity->st() + _activity->duration()) {
        moveRightAct = new QAction(tr("&Move right"), this);
        moveRightAct->setStatusTip(tr("Move the activity to the right."));
        myMenu.addAction(moveRightAct);
        connect(moveRightAct, SIGNAL(triggered()), this, SLOT(moveRight()));
    }
    if (_activity->group()->size() == 1) {
        flexibilityAct = new QAction(tr("&Set flexibility (current: %1)").arg(_activity->getFlex()), this);
        flexibilityAct->setStatusTip(tr("Set the flexibility of %1").arg(_activity->name()));
        connect(flexibilityAct, SIGNAL(triggered()), this, SLOT(setFlexibility()));
        myMenu.addAction(flexibilityAct);
    }

    myMenu.exec(event->globalPos());
}

void ActivityWidget::updateToolTip() {
    setToolTip(_activity->name());
}

void ActivityWidget::updateGeometry() {
    QWidget::updateGeometry();
}

void ActivityWidget::setBlur(bool b) {
    blurred = b;
    update();
}

void ActivityWidget::setDiff(bool b) {
    diff = b;
    update();
}

void ActivityWidget::setColor(QColor c) {
    color = c;
    update();
}

void ActivityWidget::updateActivityPosition(int offsetY) {
    QPoint topleft((_activity->st()+parent->offsetX()) * parent->hZoom(), offsetY * parent->vZoom());
    QSize hint = sizeHint();
    QPoint bottomright(topleft.x() + hint.width(), topleft.y() + hint.height());
    setGeometry(QRect(topleft, bottomright));
}

void ActivityWidget::determineFeasibleInterval(QPainter *painter, int offsetY) {
    QSize hint(sizeHint());
    int xStart = parent->hZoom() * (activity()->est() + parent->offsetX());
    int xEnd   = parent->hZoom() * (activity()->lst() + activity()->duration());
    parent->paintFeasibilityInterval(*painter, xStart, xEnd, hint.height()-2, offsetY);
}

void ActivityWidget::paintFlexibilityInterval(QPainter *painter, int offsetY) {
    // do not paint intervals when not set
    if(activity()->estFlex() < 0 || activity()->lstFlex() < 0) return;

    QSize hint(sizeHint());
    int xStart = parent->hZoom() * (activity()->estFlex());
    int xEnd   = parent->hZoom() * (activity()->lstFlex() + activity()->duration());
    parent->paintFlexibilityInterval(*painter, xStart, xEnd, hint.height()-2, offsetY);
}
