#ifndef GROUPWIDGET_H_
#define GROUPWIDGET_H_

#include "model/group.h"
#include <QWidget>

class JobWidget;
class InstanceController;
class ActivityWidget;

/**
 * A GroupWidget represents a group of activities in the interface. It contains
 * activities with a rectangle around them.
 */
class GroupWidget : public QWidget
{
    Q_OBJECT

    Group *group;
    QList<ActivityWidget *> activityWidgets;
    JobWidget *parent;
    int offsetX, offsetY;
    int oldST;
public:
    /**
     * Constructs a groupwidget object
     * @param group underlying group
     * @param activityWidgets activitywidgets corresponding to the group
     * @param parent parent jobwidget object
     */
    explicit GroupWidget(Group *group, QList<ActivityWidget *> activityWidgets, JobWidget * parent = 0);

    /**
     * Sets the position of the groupwidget.
     * @param offsetX x offset
     * @param offsetY y offset
     */
    void setPosition(int offsetX, int offsetY);

    /**
     * @return the group corresponding to the widget.
     */
    Group* getGroup();

    /**
     * @return the size of the group.
     */
    int size();

    /**
     * @return a list of activity widgets.
     */
    QList<ActivityWidget *> getActivityWidgets();

    /**
     * Determines the feasible interval of the group.
     * @param painter the painter to be used
     * @param offsetY the vertical offset, used in expanded mode
     */
    void determineFeasibleInterval(QPainter *painter, int offsetY);
    void paintFlexibilityInterval(QPainter *painter, int offsetY);
protected:
    void paintEvent(QPaintEvent *); ///< paints the rectangle around the activities.
    void updateGeometry(); ///< updates the size of the underlying widget.
private slots:
    void removeActivity(Activity *a); ///< removes the activity from the groupwidget.
};

#endif /* GROUPWIDGET_H_ */
