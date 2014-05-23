#ifndef INSTANCEWIDGET_H
#define INSTANCEWIDGET_H

#include "widgets/abstractinstancewidget.h"
#include "widgets/resourcewidget.h"
#include "widgets/resourceheaderwidget.h"
#include "widgets/overlaywidget.h"
#include "model/est_generator.h"

class Controller;
class InstanceController;

/**
 * The main widget for visualising an instance. For every instance loaded, one
 * of these widgets is created and added to the QMdiArea in the QMainWindow.
 * The widget consists of a vertical splitter with the jobs in the top half and
 * the resources in the bottom half. Both halves are constructed using a horizontal
 * splitter with a header section left of the split and a content section right of
 * the split. Both areas (left and right) are laid out using a QVBoxLayout with
 * custom widgets in them. The custom widgets are Job(Header)Widget and Resource(Header)Widget.
 */
class InstanceWidget : public AbstractInstanceWidget
{
    Q_OBJECT

    int hZoom;
    int vZoom;
    int blurTime;
    
    Controller *controller;
    bool precedenceEditing;

    int frameNumber;
    EST_Generator *estGen;

    Activity *selectedActivity;

    QMap<int, JobHeaderWidget*> jobHeaderWidgets;
    QMap<int, ResourceHeaderWidget*> rHeaderWidgets;
    QMap<int, ResourceWidget*> rWidgets;

    // Main ui components
    OtsSplitter *jobSplitter;
    OtsSplitter *resourceSplitter;

    OverlayWidget *overlay;

    // resource splitter left (list)
    QScrollArea *resourceHeaderScroller;
        QWidget *resourceHeaders;
            QVBoxLayout *resourceHeadersLayout;

    // resource splitter right (schedule visualization)
    QWidget *resourcesViewer;
        QVBoxLayout *resourcesViewerLayout;
            QScrollArea *resourceTimelineScroller;
                Timeline *resourceTimeline;
            QScrollArea *resourcesScroller;
                Zoomable *resourcesZoomable;
                    QVBoxLayout *resourcesLayout;

    QList<ResourceWidget *> resourceWidgets;

    void createViewers();
    void createResourceSplitter();
    void setSplitterSizes();
    void initFromInstance();
    void setupButtons();
    void setPrecedenceEditing(bool);

    /**
     * Updates horizontal zoom factor of jobwidgets and zoomable widgets.
     * @param z zoom factor
     */
    void setZoom(int z);

    void initResources();
protected:
    void addJob(Job *); ///< connected to the instance's jobAdded SIGNAL
    void connectViewerSignals();
public:
    /**
     * Constructor, sets up the ui, adds widgets for jobs and activities
     * and creates a controller for this instance.
     * @param instance The instance that is displayed.
     */
    InstanceWidget(Instance *instance, Controller *controller, QWidget *parent = 0);

    virtual ~InstanceWidget();

    /**
     * Used to sync child widget sizes and the vertical mouse-following guide line
     * @param o object that caused event
     * @param e event
     * @return true iff event was accepted by event filter
     */
    bool eventFilter(QObject *o, QEvent *e);

    /**
     * Returns all JobWidgets belonging to this instance.
     * @return QMap of JobWidgets
     */
    QMap<int, JobWidget*> getJobWidgets();

    /**
     * Returns all ResourceWidgets belonging to this instance.
     * @return QMap of ResourceWidgets
     */
    QMap<int, ResourceWidget*> getResourceWidgets();

    /**
     * Returns all ActivityWidgets belonging to this instance.
     * @return QMap of ActivityWidgets
     */
    QList<ActivityWidget*> getActivityWidgets();

    /**
     * Returns the overlay that is used for drawing over activities and jobs.
     * @return pointer to the OverlayWidget
     */
    OverlayWidget* getOverlay();

    /**
     * Draws a vertical line for representing a resource decrease,
     * that runs from a resource up to a job.
     * Also, resource marks are drawn where the line meets the resource profile.
     * @param xPos The x coordinate for the line.
     * @param resKey The id of the resource widget.
     */
    void addVerticalLine(int xPos, int resKey);

    /**
     * Sets the mouse x coordinate, so that the mouse following line is updated.
     * @param x The current x coordinate of the mouse.
     */
    void setMouseX(int x);
    
    /**
     * Setting up the slider and possibly resets frameNumber.
     * @param reset set true to reset frameNumber (to zero)
     */
    void setupSlider(bool reset);

    /**
     * Go to a new frame. The function updates activity widgets, resource widgets and precedences.
     * @param frameNumber number of the frame
     */
    void toFrameNumber(int frameNumber);

    /**
     * Blurs all activities completed on or before the specified time.
     * @param t completion time
     */
    void disableActivitiesBeforeTime(int t);

    /**
     * Returns true if precedence adding mode has been activated
     * @return true if activated
     */
    bool isPrecedenceEditing();

    /**
     * Returns true if a first precedence point was selected.
     * @return true if first point was selected
     */
    bool hasAddedFirstPrecedencePoint();

    /**
     * Updates all widgets that contain a zoom factor.
     */
    void updateZoom();

    void disconnectActivitiesFromResourceWidgets(); ///< disconnects all activity signals from resource widgets
    void reconnectActivitiesToResourceWidgets(); ///< reconnects all activity signals to resources
    void repaintResourceWidgets(); ///< calculates resource profile and repaints resource widgets
    void removeVerticalLines(); ///< clears all vertical lines and markers in the zoomables
    int getFrameNumber() const; ///< returns frameNumber
    bool isAtLastFrame() const; ///< return whether frame number is the last one
    void toLastFrame(); ///< go to the last frame
    void updateTimelineOffset(); ///< determines new start time of the schedule, calculates offsets and repaints widgets
    void zoomIn(); ///< multiplies horizontal zoom factor by 2 and sets this value in all concerned widgets
    void zoomOut(); ///< divides horizontal zoom factor by 2 and sets this value in all concerned widgets
    int getJobIndex(JobHeaderWidget*); ///< gets the index of the job in the headerwidget
    int getResourceIndex(ResourceHeaderWidget*); ///< gets the index of the resource in the headerwidget
    void relocateJobWidget(int, int); ///< switches the jobwidgets in the layout with the given indexes
    void relocateResourceWidget(int, int); ///< switches the resourcewidgets in the layout with the given indexes
    void updateWidgetsLayout(); ///< schedules a paint event for jobs layout and resources layout
    void enableActivities(); ///< removes blur from activities
    void disableActivitiesBeforeTime(); ///< determine which activities have been completed, and if so, it blurs the activity
    void enablePrecedenceAdding(); ///< enables precedence adding and connects signals for adding precedences
    void disablePrecedenceAdding(); ///< disables precedence adding and disconnects signals
    void enablePrecedenceRemoving(); ///< enables precedence removing and connects signals for removing precedences
    void disablePrecedenceRemoving(); ///< disables precedence removing and disconnects signals
    void removePeaks(); ///< removes all peaks
    InstanceController * getInstanceController(); //TODO move to another file

private slots:
    void addResource(Resource *); ///< connected to the instance's resourceAdded SIGNAL
    void removeJob(Job *); ///< connected to the job header widget's remove SIGNAL
    void removeResource(Resource *); ///< connected to resource header's remove SIGNAL
    void resizeOverlay(const QSize); ///< resizes the overlay

public slots:
    void moveOverlayMouse(int,int); ///< slot to update the position of the mouse pointer in the overlay
    void addPrecedencePoint(ActivityWidget *aw); ///< selects a precedence point and remembers the widget, or adds a precedence
    void removePrecedencePoint(ActivityWidget *aw); ///< selects a precedence point and remembers the widget, or removes a precedence

};

#endif
