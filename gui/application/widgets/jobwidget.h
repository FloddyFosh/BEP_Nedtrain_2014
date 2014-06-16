#ifndef JOBWIDGET_H
#define JOBWIDGET_H

#include <QProcess>
#include "activitywidget.h"
#include "groupwidget.h"

class InstanceController;

class MainWindow;

/** Shows how the activities of this job are scheduled in time. Used in top-right section
  * of visualisation widget.
  */
class JobWidget : public QWidget
{
    Q_OBJECT

    InstanceController *controller;
    bool comparing;
    int _hZoom;
    int _vZoom;

protected:
    Job *_job;
    bool expanded;
    QList<ActivityWidget *> activityWidgets;
    QMap<Activity*, ActivityWidget*> widgetMap;
    QMap<Group *, GroupWidget *> groupWidgets;
    QList<GroupWidget *> groupWidgetList;
public:
    /**
     * Sets up UI: create activity widgets and connect signals.
     * @param job the job which has to be represented by this widget
     * @param controller controller
     * @param parent reference to parent widget
     */
    explicit JobWidget(Job *job, InstanceController *controller, QWidget *parent = 0, bool compare = false);

    /**
     * Returns the activity widget associated with the specified activity.
     * @param a activity for which the widget has to be returned
     * @return the associated activity widget
     */
    ActivityWidget* getWidget(Activity *a);

    /**
	 * Returns the group widget associated with the specified group.
	 * @param g group for which the widget has to be returned
	 * @return the associated group widget
	 */
    GroupWidget* getWidget(Group* g);

    /**
     * Sets horizontal zoom factor.
     * @param z zoom factor
     */
    void setHZoom(int z);

    /**
     * Sets vertical zoom factor.
     * @param z zoom factor
     */
    void setVZoom(int z);

    /**
     * Paints a feasibility interval
     * @param painter The painter to use
     * @param xStart Start point of interval (est)
     * @param xEnd End point of the interval (lst+duration)
     * @param height Height of the to be painted interval
     */
    void paintFeasibilityInterval(QPainter &painter, int xStart, int xEnd, int height, int yOffset);
    void paintFlexibilityInterval(QPainter &painter, int xStart, int xEnd, int height, int yOffset);

    Job const *job() const; ///< return associated job
    QSize sizeHint() const; ///< returns minimumSize()
    QSize minimumSize() const; ///< return minimum size based on vZoom, hZoom, number of activities, expanded or collapsed state and due date
    QList<ActivityWidget*> getActivityWidgets(); ///< returns list of activity widgets belonging to this job widget
    int hZoom() const; ///< returns horizontal zoom factor
    int vZoom() const; ///< returns vertical zoom factor
    int offsetX();

protected:
    void solverFinished(QProcess::ExitStatus state);
    void paintEvent(QPaintEvent *); ///< do native painting, then add availability interval and mark problem areas and finally draw a bottom border
    void resizeEvent(QResizeEvent *); ///< relayout activity widgets on a resizeEvent (probably caused by a expand/collapse action by the user)
    bool event(QEvent *); ///< do custom layout with doLayout() method when a layout event is received
    void contextMenuEvent(QContextMenuEvent*); ///< shows context menu
    void mousePressEvent(QMouseEvent *);
    virtual void addActivities();
private:
    void paintAvailability(QPainter &painter); ///< paints the green availibility interval for this job
    void paintGroups(); ///< have groups paint their rectangles
    void addGroups();
public slots:
    virtual void addActivity(Activity *); ///< if an activity is added to the associated job, this slot should be called
    void removeActivity(ActivityWidget *); ///< this slot should be called to remove an activity from the widget
    virtual void doLayout(); ///< updates the geometry of all the child (activity) widgets
    void doExpand(bool); ///< sets the expanded flag and calls updateGeometry()
    void updateGeometry(); ///< exposes native updateGeometry() as a slot
    void showJobInfoDialog(); ///< slot to show the job info dialog
    void showResourceUsage(); ///< shades the resource usage of this job widget
    void addGroup(Group *group); ///< add group to the job widget
    void removeGroup(Group *group); ///< remove the specified group from the jobwidget.
};

#endif // JOBWIDGET_H
