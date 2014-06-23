#ifndef RANDOMCOLORGENERATOR_H
#define RANDOMCOLORGENERATOR_H

#include <QColor>
#include <QMap>

/** Generate esthetically pleasing color set by varying only hue. This assumes we do not
 *  know before-hand how many colors we need. A singleton pattern is used.
 *
 * You can use a QString as a 'tag' object, every time you ask a colorFor a tag, you will
 * get the same color. If the tag has not been used before, next() is called internally
 * to obtain a fresh color.
 *
 *  @see http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
 */
class RandomColorGenerator
{
private:
    static const double golden_ratio_conjugate = 0.618033988749895; ///< 1 divided through the golden ratio, approximately
    double _h; ///< hue
    double _s; ///< saturation
    double _v; ///< value
    QMap<QString, QColor> colors; ///< map from tag strings to colors

public:
    RandomColorGenerator(double s = 0.45, double v = 0.95);
    QColor next(); ///< get a nice looking color
    QColor colorFor(QString tag); ///< get a color for a specific string, if the string is the same, the color will be the same between different invocations of the method

};

#endif // RANDOMCOLORGENERATOR_H
