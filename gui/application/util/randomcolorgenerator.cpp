#include "randomcolorgenerator.h"

#include <cstdlib> // for RAND_MAX

RandomColorGenerator::RandomColorGenerator(double s, double v) :
    _s(s), _v(v)
{
    _h = qrand() / double(RAND_MAX);
}

QColor RandomColorGenerator::next() {
    _h += golden_ratio_conjugate;
    if (_h > 1) _h -= 1;
    return QColor::fromHsvF(_h, _s, _v);
}

QColor RandomColorGenerator::colorFor(QString tag) {
    if (colors[tag] == QColor()) {
        QColor thenext = next();
        colors[tag] = thenext;
        return thenext;
    } else {
        return colors[tag];
    }
}
