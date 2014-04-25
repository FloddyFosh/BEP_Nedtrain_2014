#include "scrollfilter.h"
#include <QEvent>
#include <QSpinBox>
#include <QWidget>

ScrollFilter::ScrollFilter(QObject *parent = 0) : QObject(parent) {}

bool ScrollFilter::eventFilter(QObject *obj, QEvent *event) {
    if(event->type() == QEvent::Wheel && qobject_cast<QAbstractSpinBox*>(obj)) {
        if(qobject_cast<QAbstractSpinBox*>(obj)->focusPolicy() == Qt::WheelFocus) {
            event->accept();
            return false;
        } else {
            event->ignore();
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}
