#include "filteredspinbox.h"
#include <QEvent>

FilteredSpinBox::FilteredSpinBox(QWidget *parent) : QSpinBox(parent) {}

void FilteredSpinBox::focusInEvent(QFocusEvent*) {
     setFocusPolicy(Qt::WheelFocus);
}

void FilteredSpinBox::focusOutEvent(QFocusEvent*) {
     setFocusPolicy(Qt::StrongFocus);
}
