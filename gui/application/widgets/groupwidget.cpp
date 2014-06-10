#include "widgets/groupwidget.h"
#include "widgets/jobwidget.h"
#include <QPainter>
#include <QDebug>
#include <cassert>

GroupWidget::GroupWidget(Group *group, QList<ActivityWidget *> widgets, JobWidget * parent)
    : QWidget(parent), group(group), activityWidgets(widgets), parent(parent), offsetX(0), offsetY(0)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TransparentForMouseEvents); // so that activity widgets can still be clicked
    assert(activityWidgets.count() == group->getActivities().count());
    connect(group, SIGNAL(activityRemoved(Activity *)), this, SLOT(removeActivity(Activity *)));
    oldST = group->getST();
}

void GroupWidget::paintEvent(QPaintEvent *) {
    if (group->size() == 1) return;
    QPainter painter(this);
    painter.setPen(Qt::gray);
    painter.drawRect(QRect(1,1,width()-2,height()-2));
}

void GroupWidget::updateGeometry() {
    QWidget::updateGeometry();
}

void GroupWidget::setPosition(int offsetX, int offsetY) {
    GroupWidget::offsetX = offsetX;
    GroupWidget::offsetY = offsetY;
    setGeometry(QRect((group->getST()+offsetX)*parent->hZoom()-2, offsetY*parent->vZoom()-3, group->getDuration()*parent->hZoom()+3, parent->vZoom()+5));
}

void GroupWidget::determineFeasibleInterval(QPainter *painter, int offsetY) {
    QSize hint (activityWidgets.at(0)->sizeHint());
    int xStart = parent->hZoom() * (group->getEST() + parent->offsetX());
    int xEnd   = parent->hZoom() * (group->getLST() + group->getDuration() + parent->offsetX());
    parent->paintFeasibilityInterval(*painter, xStart, xEnd, hint.height(), offsetY);
}

void GroupWidget::paintFlexibilityInterval(QPainter *painter, int offsetY) {
    // do not paint intervals when not set
    if(group->getESTFlex() < 0 || group->getLSTFlex() < 0) return;

    QSize hint (activityWidgets.at(0)->sizeHint());
    int xStart = parent->hZoom() * (group->getESTFlex());
    int xEnd   = parent->hZoom() * (group->getLSTFlex() + group->getDuration());
    parent->paintFlexibilityInterval(*painter, xStart, xEnd, hint.height(), offsetY);
}

int GroupWidget::size() {
    return activityWidgets.size();
}

Group* GroupWidget::getGroup() {
    return group;
}

QList<ActivityWidget *> GroupWidget::getActivityWidgets() {
    return activityWidgets;
}

void GroupWidget::removeActivity(Activity *a) {
    foreach(ActivityWidget *aw, activityWidgets) {
        if(aw->activity() == a)
            activityWidgets.removeOne(aw);
    }
}
