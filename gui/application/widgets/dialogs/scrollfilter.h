#ifndef SCROLLFILTER_H_
#define SCROLLFILTER_H_

#include <QObject>

/**
 * Class containing event filter to make immune for scrolling.
 */
class ScrollFilter : public QObject {
public:
    /**
     * Constructs scroll filter
     * @param parent parent object
     */
    ScrollFilter(QObject *parent);

    /**
     * Custom eventfilter to make immune for scrolling.
     * @param obj object that caused the event
     * @param event event
     * @return true iff event was accepted by event filter
     */
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif /* SCROLLFILTER_H_ */
