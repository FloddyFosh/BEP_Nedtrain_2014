#include "otssplitter.h"

OtsSplitter::OtsSplitter(QWidget *parent) :
    QSplitter(parent)
{
}

OtsSplitter::OtsSplitter(Qt::Orientation orientation, QWidget *parent) :
    QSplitter(orientation, parent)
{
}

void OtsSplitter::moveSplitter(int pos, int index) {
    // save signals blocked state
    bool blocked = signalsBlocked();

    blockSignals(true);

    QSplitter::moveSplitter(pos, index);

    // restore signals blocked state
    blockSignals(blocked);
}
