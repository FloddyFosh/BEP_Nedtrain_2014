#ifndef INSTANCECONTROLLER_H_
#define INSTANCECONTROLLER_H_

#include "controller/controller.h"

/**
 * Controller class for any specific instance. Handles requests from widgets corresponding to this instance.
 */
class InstanceController : public QObject {
    Q_OBJECT

public:
    /** Constructor. Pass an instance object, the corresponding instance and the global controller.
      * @param instance instance
      * @param instanceWidget instance widget
      * @param controller global controller
      */
    InstanceController(Instance * instance, InstanceWidget * instanceWidget, Controller * controller);
    /**
     * @return the corresponding instance
     */
    Instance * getInstance() const;
    /**
     * @return the corresponding instance widget
     */
    InstanceWidget * getInstanceWidget() const;
    
    /**
     * Disconnects the resource widgets from signals emitted by activities after updating.
     * This function is called just before the solver is executed, because otherwise the resource profile will be repainted many times.
     */
    void disconnectActivitiesFromResourceWidgets();
    /**
     * Reconnects the resource widgets to signals emitted by activities after updating.
     * This function is called after the solver is executed to let the resource widgets resume the usual behaviour of listening to the model for updates.
     */
    void reconnectActivitiesToResourceWidgets();
    /**
     * Repaints the resource widgets manually (that is, not by a signal).
     */
    void repaintResourceWidgets();
    /**
     * Initializes the GUI slider.
     * @param reset if set, the active frame will be set to the first frame. otherwise, the current active frame (for this istance) will remain active.
     */
    void setupSlider(bool reset);
    /**
     * The active frame is set to the last frame.
     */
    void toLastFrame();
    
    /**
     * @return the frame number of the currently displayed frame
     */
    int getFrameNumber();

    /**
     * @return A flag whether the instance has been solved
     */
    bool isAtLastFrame();

    /**
     * @return map from job ID's to the corresponding job widgets.
     */
    QMap<int, JobWidget*> getJobWidgets();
    /**
     * @return map from resource ID's to the corresponding resource widgets.
     */
    QMap<int, ResourceWidget*> getResourceWidgets();
    /**
     * @return map from activity ID's to the corresponding activity widgets.
     */
    QList<ActivityWidget*> getActivityWidgets();
    /**
     * @return the QColor belonging to this tag. if this is the first time this tag is used, return a new color.
     * @param tag tag name
     */
    QColor getColorFor(QString tag);
    /**
     * The precedence constraints pointing to, or pointing from this activity widget, are drawn.
     * @param aw the activity widget for which the dependencies must be drawn.
     */
    void showDependencies(ActivityWidget* aw);
    /**
     * @return the current activity widgets for which the dependencies are drawn, or NULL if this holds for no activity widget in the current instance
     */
    ActivityWidget* getDependencyWidget();
    
    /**
     * Hides the arrow following the mouse to select another activity.
     */
    void stopDrawingDependencies();
    /**
     * Hides the arrow following the mouse to select another activity, if the arrow is currently visible and it points from the given activity widget.
     * @param aw activity widget
     */
    void stopDrawingDependencies(ActivityWidget* aw);
    
    /**
     * @return whether the option to paint feasible intervals is currently set.
     */
    bool isPaintingFeasibleIntervals();
    /**
     * activate the painting of feasible intervals.
     */
    void doPaintFeasibleIntervals();
    
    /**
     * @return the global controller
     */
    Controller * getParent();
    
    /**
     * for the given time interval and resource, shades the activities using this resource in this time interval.
     * @param start start of the interval
     * @param end end of the interval
     * @param r resource
     */
    void shadeActivities(int start, int end, Resource* r);
    /**
     * for the given job, shades the parts of the resources in the resource profile used by activities in this job.
     * @param j job
     */
    void shadeResources(Job* j);
    /**
     * sets the status bar message to the given string
     * @param message message
     * @param timeout timeout
     */
    void setStatusMessage(QString message, int timeout = 0);
private:
    Instance * instance;
    InstanceWidget * instanceWidget;
    Controller * controller;
    
    Activity *selectedActivity;

public slots:
    void peak(int,int,int); ///< solvers peak signal is connected to this slot, it paints peaks in the resource profile.
};

#endif /* INSTANCECONTROLLER_H_ */
