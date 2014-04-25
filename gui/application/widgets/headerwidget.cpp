#include "widgets/headerwidget.h"

#include <QPainter>
#include <QEvent>

HeaderWidget::HeaderWidget(QWidget *parent) : QWidget(parent) {}

void HeaderWidget::paintEvent(QPaintEvent *e) {
    QWidget::paintEvent(e);
    QPainter painter(this);
    painter.setPen(Qt::darkGray);
    painter.drawLine(0, geometry().height() - 1, geometry().size().width(), geometry().size().height() - 1);
}
