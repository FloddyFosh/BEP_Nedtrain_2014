#include "timeline.h"

#include "model/instance.h"
#include "controller/instancecontroller.h"
#include "widgets/instancewidget.h"

#include <QPainter>
#include <QMouseEvent>

Timeline::Timeline(InstanceController * controller, QWidget *parent, bool compare, bool jobsDoneClick) :
    Zoomable(controller,parent), controller (controller), comparing(compare), _jobsDoneClick(jobsDoneClick)
{
    timeBefore = (comparing ? 0 : - controller->getInstance()->getTimeOffset());
    setBackgroundRole(QPalette::Dark);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedHeight(23);
}

int Timeline::getStepsize() {
    int stepHours[12] = {90, 90, 30, 15, 15, 5, 1, 1, 1, 1, 1, 1};
    int stepNormal[12] = {60, 60, 20, 10, 10, 5, 1, 1, 1, 1, 1, 1};

    int v = hZoom();
    int stepIndex = 0;
    while (v >>= 1) stepIndex++; //determine log base 2, doesnt work for v=0, fortunately hZoom()>0

    if(!comparing && controller->getInstance()->hoursOnTimeline())
        return stepHours[stepIndex];
    else
        return stepNormal[stepIndex];
}

void Timeline::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    QTime time;
    if(!comparing) time = controller->getInstance()->getTimeLineStartTime();
    int stepsize = getStepsize();

    for (int i = 0; i < width(); i += stepsize) {
        if(i > 0) painter.drawLine(i*hZoom(), 0, i*hZoom(), height());

        if(!comparing && controller->getInstance()->hoursOnTimeline()){
            painter.drawText(i*hZoom() + 3, 0, i*hZoom() + 50, height(), Qt::AlignVCenter, time.toString("H:mm"));
            time = time.addSecs(stepsize*60);
        }else{
            painter.drawText(i*hZoom() + 3, 0, i*hZoom() + 50, height(), Qt::AlignVCenter, tr("%0").arg(i));
        }
    }
    int resX = (timeBefore + (comparing ? 0 : controller->getInstance()->getTimeOffset())) * hZoom();
    //paint line below completed interval
    painter.setPen(Qt::green);
    painter.drawLine(0, height()-1, resX, height()-1);
    painter.drawLine(0, height()-2, resX, height()-2);

    Zoomable::paintEvent(e);
}

void Timeline::mousePressEvent(QMouseEvent *event){
    if(!comparing && _jobsDoneClick) {
        if (event->button() == Qt::LeftButton) {
            timeBefore = event->x()/hZoom() - controller->getInstance()->getTimeOffset();
            controller->getInstanceWidget()->disableActivitiesBeforeTime(timeBefore);
        }
        else if (event->button() == Qt::RightButton) {
            timeBefore = - controller->getInstance()->getTimeOffset();
            controller->getInstanceWidget()->enableActivities();
        }

        update();
    }
}

void Timeline::enterEvent(QEvent *e) {
    setCursor(Qt::PointingHandCursor);
    Zoomable::enterEvent(e);
}

void Timeline::removeDoneInterval() {
    timeBefore = (comparing ? 0 : - controller->getInstance()->getTimeOffset());
    update();
}
