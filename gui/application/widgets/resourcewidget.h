#ifndef RESOURCEWIDGET_H
#define RESOURCEWIDGET_H

#include "util/resourcecalculator.h"
#include "activitywidget.h"

class InstanceController;

/** Shows the demand profile of this resource as a polygon. One big polygon is used for
  * the entire profile. The polygon is drawn using a green-to-yellow-to-red gradient to
  * visualize how much of the capacity is used. This widget is used in bottom-right
  * section of visualisation widget.
  */
class ResourceWidget : public QWidget
{
    Q_OBJECT

    Resource *_resource;
    QPixmap pixmap;
    QList<int> resourceMarks;
    int offset;
    InstanceController *controller;
    QPoint selected;
    QRect pattern;
    ResourceCalculator* calculator;
    int _hZoom;
    int _vZoom;
    int peak;

    int hZoom() const; ///< returns current horizontal zoom factor
    int vZoom() const; ///< returns current vertical zoom factor
    QSize sizeHint() const; ///< returns minimumSize()
    QSize minimumSize() const; ///< returns an appropriate minimum size based on the latest earliest end time (of all jobs using this resource)
    void paintDemandProfile(QPainter&); ///< paints the demand profile (gradient)
    void paintResourceProfile(QPainter&); ///< paints the resource profile (line around demand profile)
    void paintJobProfile(QPainter&); ///< adds shade to resource units by selected job
    void paintSelectedRegion(QPainter&); ///< adds shading to selected region
    void paintResourceMarks(QPainter& painter); /// adds resource marks (used for indicating a precedence between the end of a resource decrease and a task)
    void paintChainResources(QPainter& painter); /// paint the chain visualization resource profiles
    void paintChainMatrix(QPainter &painter); /// paint the view where chains are displayed in the resource view
    void paintPeak(QPainter& painter); ///< paints peak
    bool viewingResourceMatrix(); /// true iff the resource matrix view is currently being displayed
public:
    /**
     * Sets up UI: connect signals and calculate profile.
     * @param r resource represented by this resource widget
     * @param c the instance controller
     * @param parent reference to parent widget
     */
    explicit ResourceWidget(Resource *r, InstanceController *c, QWidget *parent = 0);
    void addResourceMark(int); ///< marks a point of the resource profile (to stress when showing dependencies)
    void clearResourceMarks(); ///< clears all resource marks
    void updatePixmap(); ///< draws the polygon to the pixmap and generates a paint event so that the pixmap can be transferred to the widget, the demand profile polygon is precalculated in the calculateDemandProfile() method. The resource profile polyline is precalculated in the calculateDemandProfile() method as well.
    void disconnectActivities(); ///< disconnect all activityChanged signals so that resource profile is not repainted
    void connectActivities(); ///< connect all activityChanged signals to repaint the resource profile when an activity changes
    void removeShade(); ///< removes the shade and calculates the resource profile
    Resource* getResource(); ///< returns the resource represented by this widget
    void setShadedJob(Job*); ///< sets the specified job in the resource calculator
    void setHZoom(int z); ///< sets horizontal zoom factor
    void setVZoom(int z); ///< sets vertical zoom factor
    void paintPeak(int t); ///< paints a vertical line, representing a peak
    void removePeak(); ///< removes the painted peak

protected:
    void paintEvent(QPaintEvent *); ///< the widget is (manually) double buffered, this function copies the buffer (QPixmap) over to the widget's paint surface
    void resizeEvent(QResizeEvent *); ///< makes sure the QPixmap supporting the painting of this widget is properly updated when the widget is resized
    void mousePressEvent(QMouseEvent *); ///< makes the resource widget shaded and then calculates the resource profile

public slots:
    void calculateResourceProfile(); ///< generates a list of timepoint values where the resource demand changes, so if the list is [ (t1, demand1), (t2, demand2) ] then the demand is equal to demand1 from t1 to t2 and changes to demand2 at t2, this list is used to describe the polygon that is drawn onto the widget
    void newActivity(Activity *); ///< connects the signals of the new activity so that this widget properly updates when the activity changes
};

#endif // RESOURCEWIDGET_H
