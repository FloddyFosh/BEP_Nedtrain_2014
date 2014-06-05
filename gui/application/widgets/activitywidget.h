#ifndef ACTIVITYWIDGET_H
#define ACTIVITYWIDGET_H

#include <QWidget>
#include <QPaintEvent>

#include "model/activity.h"
#include "model/st_propagator.h"

class JobWidget;

class InstanceController;

/** Widget representing an activity. Used in the upper half of the InstanceWidget to
  * show the colored blobs in the job/time overview. The paintEvent method is overrided
  * to provide custom visuals.
  */
class ActivityWidget : public QWidget
{
    Q_OBJECT

    Activity *_activity;
    JobWidget *parent;
    InstanceController *controller;
    bool highlight, shade, blurred, selected, changeDuration, comparing, diff;
    int selectedX, oldST;
    QAction *infoAct,*editAct,*removeAct,*dependenciesAct,*moveLeftAct,*moveRightAct, *lockAct, *flexibilityAct;
    QColor color;

    QPoint getLocation(int offsetX); ///< returns the location of the widget, with an offset for the x coordinate
    ST_Propagator * stPropagator;
public:

    enum AdjacentWidgets{InComing, OutGoing};

    /**
     * Constructs widget for given activity. Connects activity signals to update slots in this class.
     * @param a The activity which is painted in this activity widget
     * @param controller The controller of the application
     * @param parent The job widget that contains this activity widget
     */
    explicit ActivityWidget(Activity *a, InstanceController *controller, JobWidget * parent = 0, bool compare = false);

    void setSelected(bool); ///< sets the 'selected' flag. An activity is selected with a mouse press
    QSize sizeHint() const; ///< calculates the exact size this widget should have based on the hZoom and vZoom settings and the activity duration
    void paintEvent(QPaintEvent *); ///< fetches a color from the RandomColorGenerator and paints this widget using QPainter
    Activity *activity(); ///< @return the activity that this widget is associated with
    QPoint getLocation(); ///< @return the centre location of the widget, taking the location of the jobwidget into account
    QPoint getTailLocation(); ///< @return the front location of the widget, taking the location of the jobwidget into account
    QPoint getFrontLocation(); ///< @return the tail location of the widget, taking the location of the jobwidget into account
    QList<ActivityWidget*> getAdjacentWidgets(AdjacentWidgets type);
    QList<ActivityWidget*> getAdjacentWidgets(AdjacentWidgets type, Activity* a); ///< @return a list of activity widgets which have a precedence relation to or from this widget
    void setHighlight(bool); ///< sets the highlight flag. An activitywidget is highlighted when this widget has a precedence relation to or from another widget which is currently showing the precedence relations.
    void setShade(bool); ///< sets the shade flag. An activity is shaded when it uses a part resource that currently selected
    void setBlur(bool); ///< set the blur flag. An activity is blurred if it has been finished in the past.
    void setDiff(bool); ///< set diff flag. Activities with diff flag set are painted red in compare mode.
    void setColor(QColor c); ///< changes color of the activity widget, in compare mode.
    void saveOldSTs(); ///< saves the old st of this activitywidget and its successors and predecessors (used for calculating move distance while dragging)
    void saveOldST(AdjacentWidgets type); /// saves the old st of this activitywidget and its successors
    bool canMove(int moveDistance); ///< checks whether this activity can be moved
    bool canMoveGroup(int moveDistance); ///< checks whether this group can be moved
    void moveGroupRight(int moveDistance); ///< move associated group to the right
    void moveGroupLeft(int moveDistance); ///< move associated group to the left
    /**
     * Moves this activity and its successors with distance 'moveDistance' with respect to the old start time
     * @param moveDistance the distance the activity must be moved
     */
    void moveActivityChainToRight();
    /**
	 * Moves this activity and its predecessors with distance 'moveDistance' with respect to the old start time
	 * @param moveDistance the distance the activity must be moved
	 */
    void moveActivityChainToLeft();
    /**
     * Update the position of the activity widget.
     * @param aw ActivityWidget, whose position is updated.
     * @param offsetY Vertical offset.
     */
    void updateActivityPosition(int offsetY);

    void determineFeasibleInterval(QPainter *painter, int offsetY);
    void paintFlexibilityInterval(QPainter *painter, int offsetY);

protected:
    void mouseMoveEvent(QMouseEvent *); ///< overrided method to receive mouse move events.
    void mousePressEvent(QMouseEvent *); ///< overrided method to receive mouse press events.
    void mouseReleaseEvent(QMouseEvent *); ///< overrided method to receive mouse press events.
    void contextMenuEvent(QContextMenuEvent*); ///< shows context menu

public slots:
    /** Exposed updateGeometry method as a slot so that when an activity changes
      * duration, it can signal its widget. In the construction of this widget
      * the signals of the activity object are linked to this slot.
      */
    void updateGeometry();
    void updateToolTip(); ///< sets the name of this activity as tool tip
    void showActivityDialog(); ///< slot to show the activity dialog
    void showActivityInfoDialog();///< slot to show the activity info dialog
    void removeWidget(); ///< remove this activity
    void showDependencies(); ///< controls whether this activity widget shows precedence relations
    void moveLeft(); ///< if this activity belongs to a group, this method can move the activity to the left inside this group
    void moveRight(); ///< if this activity belongs to a group, this method can move the activity to the right inside this group
    void setFlexibility(); ///< slot to show dialog to set flexibility
    void lock(bool locked); ///< locks or unlocks the activity

signals:
	void activitySelected(ActivityWidget *); ///< signal to indicate that this activity was selected.
};

#endif // ACTIVITYWIDGET_H
