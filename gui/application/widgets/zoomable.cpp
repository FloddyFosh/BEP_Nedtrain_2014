#include "zoomable.h"

#include "controller/instancecontroller.h"
#include "model/instance.h"

#include <QPainter>
#include <QEvent>
#include <QResizeEvent>

using namespace std;

Zoomable::Zoomable(InstanceController *c, QWidget *parent) : QWidget(parent), controller(c), _hZoom(8), _vZoom(10), mouseX(-1) {
    setMouseTracking(true);
}

int Zoomable::hZoom() const {
    return _hZoom;
}

int Zoomable::vZoom() const {
    return _vZoom;
}

void Zoomable::setHZoom(int hZoom) {
    if (hZoom != _hZoom && hZoom > 0) {
        _hZoom = hZoom;
        emit hZoomChanged(hZoom);
    }
}

void Zoomable::setMouseX(int x) {
    if (mouseX != x) {
        int oldMouseX = mouseX;
        mouseX = x;
        update(min(mouseX, oldMouseX), 0, max(mouseX+10, oldMouseX+10), height());
        emit mouseXChanged(mouseX);
    }
}

void Zoomable::resizeEvent(QResizeEvent *event){
    emit dimensionChanged(event->size());
    event->accept();
}

void Zoomable::addVerticalLine(int xPos){
	lineList.append(xPos);
	repaint();
}
void Zoomable::clearVerticalLines(){
	lineList.clear();
	repaint();
}

void Zoomable::mouseMoveEvent(QMouseEvent* e){
	emit mouseMoved(e->x(),e->y());
}

void Zoomable::paintEvent(QPaintEvent *e) {
    QWidget::paintEvent(e);
    QPainter painter(this);
    if (e->rect().left() <= mouseX && e->rect().right() >= mouseX) {
        painter.setPen(Qt::black);
        painter.drawLine(mouseX, 0, mouseX, height());
    }

    if(controller) {
        int offset = controller->getInstance()->getTimeOffset();
        painter.setPen(QPen(QBrush(Qt::darkGreen),1,Qt::DashLine));
        foreach(int xPos, lineList){
            xPos += offset;
            painter.drawLine(xPos*_hZoom,0,xPos*_hZoom,height());
        }
    }
}
