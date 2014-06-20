#include "widgets/abstractinstancewidget.h"

#include <QScrollBar>

AbstractInstanceWidget::AbstractInstanceWidget(Instance *instance, Qt::Orientation orientation, QWidget *parent)
    : OtsSplitter(orientation, parent), instance(instance), instanceController(0)
{
}

void AbstractInstanceWidget::initJobs() {
    jobHeadersLayout->addStretch();
    jobsLayout->addStretch();
    foreach(Job *j, instance->getJobs())
        addJob(j);
}

void AbstractInstanceWidget::connectViewerSignals() {
    connectVerticalScrollers(jobHeaderScroller, jobsScroller);
    connectHorizontalScrollers(jobsScroller, jobTimelineScroller);
    connectZoomables(jobsZoomable, jobTimeline);
}

void AbstractInstanceWidget::createJobSplitter(OtsSplitter *jobSplitter) {
    jobHeaderWidget = new QWidget;
    jobHeaderLayout = new QVBoxLayout;
    jobWidgetTitleBar = new QWidget;
    jobWidgetTitleBarLayout = new QHBoxLayout;
    jobWidgetTitle = new QLabel(tr("<b>Jobs</b>"));
    jobHeaderScroller = new QScrollArea;
    jobHeaders = new QWidget;
    jobHeadersLayout = new QVBoxLayout;
    jobsViewer = new QWidget;
    jobsScroller = new QScrollArea;
    jobsZoomable = new Zoomable(instanceController, this);
    jobsLayout = new QVBoxLayout;
    jobTimelineScroller = new QScrollArea;
    if(instanceController)
        jobTimeline = new Timeline (instanceController, this, false, true);
    else
        jobTimeline = new Timeline (0, 0, true, false);

    jobWidgetTitleBar->setLayout(jobWidgetTitleBarLayout);
    jobWidgetTitleBarLayout->addSpacing(25);
    jobWidgetTitleBarLayout->addWidget(jobWidgetTitle);
    jobWidgetTitleBarLayout->addSpacing(25);
    jobWidgetTitle->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    jobWidgetTitle->setMargin(0);
    jobWidgetTitle->setIndent(0);

    createHeaders(jobSplitter, jobHeaderWidget, jobHeaderLayout, jobWidgetTitleBar, jobHeaders, jobHeadersLayout, jobHeaderScroller);
    createViewer(jobSplitter, jobsViewer, jobsScroller, jobsZoomable, jobsLayout, jobTimeline, jobTimelineScroller);
}

void AbstractInstanceWidget::connectSplitters(OtsSplitter *a, OtsSplitter *b) {
    connect(a, SIGNAL(splitterMoved(int,int)), b, SLOT(moveSplitter(int,int)));
    connect(b, SIGNAL(splitterMoved(int,int)), a, SLOT(moveSplitter(int,int)));
}

void AbstractInstanceWidget::connectVerticalScrollers(QScrollArea *a, QScrollArea *b) {
    connect(a->verticalScrollBar(), SIGNAL(valueChanged(int)), b->verticalScrollBar(), SLOT(setValue(int)));
    connect(b->verticalScrollBar(), SIGNAL(valueChanged(int)), a->verticalScrollBar(), SLOT(setValue(int)));
}

void AbstractInstanceWidget::connectHorizontalScrollers(QScrollArea *a, QScrollArea *b) {
    connect(a->horizontalScrollBar(), SIGNAL(valueChanged(int)), b->horizontalScrollBar(), SLOT(setValue(int)));
    connect(b->horizontalScrollBar(), SIGNAL(valueChanged(int)), a->horizontalScrollBar(), SLOT(setValue(int)));
}

void AbstractInstanceWidget::connectZoomables(Zoomable *a, Zoomable *b) {
    connect(a, SIGNAL(hZoomChanged(int)), b, SLOT(setHZoom(int)));
    connect(b, SIGNAL(hZoomChanged(int)), a, SLOT(setHZoom(int)));
}

void AbstractInstanceWidget::createHeaders(
        OtsSplitter *parentSplitter,
        QWidget *headerWidget,
        QVBoxLayout *headerLayout,
        QWidget *titleBar,
        QWidget *headers,
        QVBoxLayout *layout,
        QScrollArea *scroller)
{
    headerWidget->setLayout(headerLayout);
    headerWidget->setMinimumWidth(150);
    headerLayout->addWidget(titleBar);
    headerLayout->addWidget(scroller);

    scroller->setWidget(headers);
    scroller->setWidgetResizable(true);
    scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    headers->setLayout(layout);
    headers->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    layout->setMargin(0);
    layout->setSpacing(0);
    headerLayout->setMargin(0);
    headerLayout->setSpacing(0);
    titleBar->layout()->setMargin(0);
    titleBar->layout()->setSpacing(0);
    titleBar->setMinimumHeight(25);
    headerWidget->setMaximumWidth(400);
    headerWidget->setContentsMargins(0, 0, 0, 0);

    parentSplitter->addWidget(headerWidget);
}

void AbstractInstanceWidget::createViewer(
        OtsSplitter *parentSplitter,
        QWidget *viewer,
        QScrollArea *scroller,
        Zoomable *zoomable,
        QVBoxLayout *layout,
        Timeline* timeline,
        QScrollArea* timelineScroller)
{
    QVBoxLayout *viewerLayout = new QVBoxLayout;
    viewer->setLayout(viewerLayout);
    viewer->setMinimumWidth(30);
    viewerLayout->addWidget(timelineScroller);
    timelineScroller->setWidget(timeline);
    viewerLayout->addWidget(scroller);
    scroller->setWidget(zoomable);
    zoomable->setLayout(layout);

    viewer->setMouseTracking(true);
    // set white (usually) background
    viewer->setBackgroundRole(QPalette::Base);
    viewer->setAutoFillBackground(true);

    viewerLayout->setMargin(0);
    viewerLayout->setSpacing(0);

    timelineScroller->setFixedHeight(25);
    timelineScroller->setWidgetResizable(true);
    timelineScroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    timelineScroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    scroller->setWidgetResizable(true);
    scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    layout->setMargin(0);
    layout->setSpacing(0);

    // final main vertical splitter for jobs
    parentSplitter->addWidget(viewer);
    parentSplitter->setChildrenCollapsible(false);
    parentSplitter->setHandleWidth(2);
}
