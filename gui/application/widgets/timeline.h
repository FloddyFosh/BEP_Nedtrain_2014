#ifndef TIMELINE_H
#define TIMELINE_H

#include "widgets/zoomable.h"

class InstanceController;

/**
 * The widget showing the timeline at the top of the job and resource views.
 */
class Timeline : public Zoomable
{
    Q_OBJECT

    InstanceController *controller;
    bool comparing, _jobsDoneClick;
    int timeBefore;

protected:
    /**
	 * Returns step size between vertical lines, depends on hour settings and zoom factor
	 * @return step size
	 */
    int getStepsize();

public:
    /**
     * Custom constructor to set a fixed height and fixed size policy
     * @param ic instance controller
     * @param parent reference to parent widget
     * @param compare set to true if initialized in compare mode
     * @param
     */
    Timeline(InstanceController *ic, QWidget *parent = 0, bool compare = false, bool jobsDoneClick = false);

    /**
     * Removes the done interval by setting doneBefore to 0, and forces repaint.
     */
    void removeDoneInterval();

protected:
    void paintEvent(QPaintEvent *); ///< custom paint event to set the markers and print the timestamps
    void mousePressEvent(QMouseEvent *); ///< event to blur completed activities and undo blurring
    void enterEvent(QEvent *e); ///< event to change cursor on mouseover
};

#endif // TIMELINE_H
