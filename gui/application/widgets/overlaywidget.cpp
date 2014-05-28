#include <QPainter>
#include "controller/instancecontroller.h"

OverlayWidget::OverlayWidget(InstanceController *controller, QWidget* parent) :
    QWidget(parent), widget(0), controller(controller)
{
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    isAddingPrecedence = false;
    isDrawingPrecedences = false;
    arrow = Arrow();
    hZoom = 8;

    penSoft.setColor(Qt::black);
    penSoft.setStyle(Qt::SolidLine);
    penSoft.setWidth(1);
    penHard.setColor(Qt::red);
    penHard.setStyle(Qt::SolidLine);
    penHard.setWidth(1);
}

void OverlayWidget::setPrecedencePoint(QPoint p){
    p1 = p;
    isAddingPrecedence = true;
}

void OverlayWidget::unsetPrecedencePoint(){
	isAddingPrecedence = false;
	repaint();
}

void OverlayWidget::moveLine(int x, int y){
	if(isAddingPrecedence){
		QPointF p1f = QPointF(p1);
		QPointF p2f = QPointF(x,y);
		arrow.setPoints(p1f,p2f);
		update(min(p1f.x(), p2f.x())-100,
				0,
				max(p1f.x(), p2f.x())+100,
				height());
	}
}

void OverlayWidget::drawDependencies(ActivityWidget* aWidget){
	stopDrawingDependencies();

    Group *g = controller->getInstance()->activityGroup(aWidget->activity());

    foreach(Activity *a, g->getActivities()) {
        InstanceWidget *iw = controller->getInstanceWidget();
        QMap<int, JobWidget*> jws = iw->getJobWidgets();
        JobWidget *jw = jws.value(g->getJob());
        ActivityWidget *aw = jw->getWidget(a);
        setHighlightAndVerticalLine(aw);
    }

	widget = aWidget;
	isDrawingPrecedences=true;

	repaint();
}

void OverlayWidget::setHighlightAndVerticalLine(ActivityWidget *aw) {
    foreach(ActivityWidget* a,aw->getAdjacentWidgets(ActivityWidget::InComing))
        a->setHighlight(true);
    foreach(ActivityWidget* a,aw->getAdjacentWidgets(ActivityWidget::OutGoing))
        a->setHighlight(true);

    foreach(ResourceDecrease *dec, aw->activity()->getDecreases()){
        int xPos = dec->getTill();
        int resId = dec->getResourceId();
        controller->getInstanceWidget()->addVerticalLine(xPos,resId);
    }
}

ActivityWidget* OverlayWidget::getActWidget(){
	return widget;
}

void OverlayWidget::stopDrawingDependencies(){
	if(isDrawingPrecedences){
		isDrawingPrecedences=false;
	    foreach(JobWidget * jw, controller->getInstanceWidget()->getJobWidgets()) {
	        foreach(ActivityWidget * aw, jw->getActivityWidgets()) {
	            aw->setHighlight(false);
	        }
	    }
		controller->getInstanceWidget()->removeVerticalLines();
		widget = 0;
		repaint();
	}
}

void OverlayWidget::stopDrawingDependencies(ActivityWidget* aw){
	if(widget == aw)
		stopDrawingDependencies();
}

void OverlayWidget::paintPrecedences(QPainter& painter){
	//return if we don't want the precedences to be painted
	if(!isDrawingPrecedences)
		return;

    Group *g = controller->getInstance()->activityGroup(widget->activity());

    foreach(Activity *a, g->getActivities()) {
        InstanceWidget *iw = controller->getInstanceWidget();
        QMap<int, JobWidget*> jws = iw->getJobWidgets();
        JobWidget *jw = jws.value(g->getJob());
        ActivityWidget *aw = jw->getWidget(a);

        paintPrecedences(painter, aw);
    }
}

void OverlayWidget::paintIncomingPrecedences(QPainter& painter, ActivityWidget *aWidget) {
    QList<Precedence *> incoming = aWidget->activity()->getIncomingPrecedences();
    QList<ActivityWidget*> allWidgets = controller->getActivityWidgets();

    foreach(Precedence* p, incoming) {
        if(!p->isPosted(controller->getInstanceWidget()->getFrameNumber()))
            continue;
        foreach(ActivityWidget* aw, allWidgets){
            if(aw->activity() == p->a1() && !p->isDisabled()) {
                if(p->isHard())
                    painter.setPen(penHard);
                else
                    painter.setPen(penSoft);
                paintPrecedence(painter, aw, aWidget, incoming.size() < 2);
            }
        }
    }
}

void OverlayWidget::paintOutgoingPrecedences(QPainter& painter, ActivityWidget *aWidget) {
    QList<Precedence *> outgoing = aWidget->activity()->getOutgoingPrecedences();
    QList<ActivityWidget*> allWidgets = controller->getActivityWidgets();

    foreach(Precedence* p, outgoing) {
        if(!p->isPosted(controller->getInstanceWidget()->getFrameNumber()))
            continue;
        foreach(ActivityWidget* aw, allWidgets){
            if(aw->activity() == p->a2() && !p->isDisabled()) {
                if(p->isHard())
                    painter.setPen(penHard);
                else
                    painter.setPen(penSoft);
                paintPrecedence(painter, aWidget, aw, true);
            }
        }
    }
}

void OverlayWidget::paintPrecedences(QPainter& painter, ActivityWidget *aWidget) {
    if(isDrawingPrecedences) {
        paintIncomingPrecedences(painter, aWidget);
        paintOutgoingPrecedences(painter, aWidget);
    }
}

void OverlayWidget::startPaintingFramePrecedences(QList<Precedence *> precedences) {
    framePrecedences = precedences;
    stopDrawingDependencies(); // also triggers repaint
}

void OverlayWidget::paintFramePrecedences(QPainter& painter) {
    QList<ActivityWidget*> allWidgets = controller->getActivityWidgets();
    ActivityWidget *aw1 = 0, *aw2 = 0;
    foreach(Precedence *p, framePrecedences) {
        if(p->isHard())
            painter.setPen(penHard);
        else
            painter.setPen(penSoft);
        foreach(ActivityWidget* aw, allWidgets){
            if(aw->activity() == p->a1())
                aw1 = aw;
            else if(aw->activity() == p->a2())
                aw2 = aw;
        }
        paintPrecedence(painter, aw1, aw2, true);
    }
}

void OverlayWidget::paintPrecedence(QPainter& painter, ActivityWidget* from, ActivityWidget* to, bool paintArrow) {
	QPointF fromPos = QPointF(from->getTailLocation());
	QPointF toPos   = QPointF(to->getFrontLocation());
	if(paintArrow) {
		Arrow arr(fromPos, toPos,3);
		painter.drawLines(arr.getArrowShape());
	} else
		painter.drawLine(fromPos,toPos);
}

void OverlayWidget::paintPrecedenceAdding(QPainter& painter){
	if(!isAddingPrecedence)
		return;

	QPen pen(Qt::darkGray,2);
	painter.setPen(pen);
	QVector<QLineF> arrowShape = arrow.getArrowShape();
	painter.drawLines(arrowShape);
}

void OverlayWidget::paintEvent(QPaintEvent *){
	raise(); // bring this widget to foreground
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    paintPrecedenceAdding(painter);
    paintPrecedences(painter);
    if(framePrecedences.size() > 0)
        paintFramePrecedences(painter);

}

void OverlayWidget::setHZoom(int h) {
    hZoom = h;
}
