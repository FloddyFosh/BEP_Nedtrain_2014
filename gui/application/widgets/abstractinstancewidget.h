#ifndef ABSTRACTINSTANCEWIDGET_H_
#define ABSTRACTINSTANCEWIDGET_H_

#include "widgets/otssplitter.h"
#include "model/instance.h"
#include "widgets/jobwidget.h"
#include "widgets/jobheaderwidget.h"
#include "widgets/timeline.h"

#include <QScrollArea>
#include <QVBoxLayout>

class InstanceController;

class AbstractInstanceWidget : public OtsSplitter
{
    Q_OBJECT

protected:
    Instance *instance;
    InstanceController *instanceController; // does not have to be initialized

    // job splitter left (list)
    QWidget *jobHeaderWidget;
        QVBoxLayout *jobHeaderLayout;
            QWidget *jobWidgetTitleBar;
                QHBoxLayout *jobWidgetTitleBarLayout;
                    QLabel *jobWidgetTitle;
                    QScrollArea *jobHeaderScroller;
                        QWidget *jobHeaders;
                            QVBoxLayout *jobHeadersLayout;

    // job splitter right (schedule visualization)
    QWidget *jobsViewer;
        QVBoxLayout *jobsViewerLayout;
            QScrollArea *jobTimelineScroller;
                Timeline *jobTimeline;
            QScrollArea *jobsScroller;
                Zoomable *jobsZoomable;
                    QVBoxLayout *jobsLayout;

    QMap<int, JobWidget*> jobWidgets;

public:
    AbstractInstanceWidget(Instance *instance, Qt::Orientation orientation, QWidget *parent = 0);

protected:
    void createJobSplitter(OtsSplitter *jobSplitter);
    void createSplitter(QWidget *headers, QScrollArea *headerScroller, QVBoxLayout *headersLayout);

    void connectViewerSignals();

    void createHeaders(OtsSplitter *parentSplitter, QWidget *headerWidget, QVBoxLayout *headerLayout, QWidget *titleBar, QWidget *headers, QVBoxLayout *headersLayout, QScrollArea *headerScroller);
    void createViewer(OtsSplitter *parentSplitter, QWidget *viewer, QScrollArea *scroller, Zoomable *zoomable, QVBoxLayout *layout, Timeline* timeline, QScrollArea* timelineScroller);

    /**
     * Adds jobs from the instance to the job viewer.
     */
    void initJobs();

    /**
     * Connects the splitters from two OtsSplitters to each other.
     * @param a The one OtsSplitter.
     * @param b The other OtsSplitter.
     */
    void connectSplitters(OtsSplitter *a, OtsSplitter *b);

    /**
     * Connect two vertical scrollbars to each other: a <=> b
     * @param a The one scrollbar.
     * @param b The other scrollbar.
     */
    void connectVerticalScrollers(QScrollArea *a, QScrollArea *b);

    /**
     * Connect two horizontal scrollbars to each other: a <=> b
     * @param a The one scrollbar.
     * @param b The other scrollbar.
     */
    void connectHorizontalScrollers(QScrollArea *a, QScrollArea *b);

    /**
     * Connect two Zoomables to each other: a <=> b
     * @param a The one Zoomable.
     * @param b The other Zoomable.
     */
    void connectZoomables(Zoomable *a, Zoomable *b);
protected slots:
    /**
     * Adds a job to the viewer.
     * @param j the job
     */
    virtual void addJob(Job *j) = 0;
};


#endif /* ABSTRACTINSTANCEWIDGET_H_ */
