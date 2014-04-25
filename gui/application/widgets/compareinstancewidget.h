#ifndef COMPAREPARTIALWIDGET_H
#define COMPAREPARTIALWIDGET_H

#include "model/comparison.h"
#include "widgets/abstractinstancewidget.h"
#include "widgets/overlaywidget.h"
#include "util/randomcolorgenerator.h"

#include <QScrollArea>
#include <QVBoxLayout>

class CompareInstanceWidget : public AbstractInstanceWidget
{
    Q_OBJECT

    CompareInstanceWidget *connected; ///< another CompareInstanceWidget, which is connected to this one

public:
    /**
     * Constructs a CompareInstanceWidget and setups the lay-out.
     * @param instance The instance that should be drawn.
     * @param parent The parent widget.
     */
    CompareInstanceWidget(Instance *instance, QWidget *parent = 0);

    /**
     * Connects this CompareInstanceWidget to another,
     * so that the scrollbars will change both widgets.
     * @param other Another CompareInstanceWidget.
     */
    void connectTo(CompareInstanceWidget *other);

    /**
     * Sets the colour for an activity, which is grey by default.
     * @param activity The activity that needs to be coloured.
     * @param color The colour for the activity.
     */
    void setActivityColor(Activity *activity, const QColor &color);

private:
    /**
     * Returns the jobwidget corresponding to the specified job.
     * @param job the job
     * @return the job widget
     */
    JobWidget* getJobWidget(Job *job);

protected:

    /**
     * Adds a job to the viewer.
     * @param j the job
     */
    void addJob(Job *j);

    /**
     * Used to sync child widget sizes and the vertical mouse-following guide line
     * @param o object that caused event
     * @param e event
     * @return true iff event was accepted by event filter
     */
    bool eventFilter(QObject *o, QEvent *e);
};

#endif
