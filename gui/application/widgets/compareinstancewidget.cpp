#include "widgets/compareinstancewidget.h"

CompareInstanceWidget::CompareInstanceWidget(Instance *instance, QWidget *parent)
    : AbstractInstanceWidget(instance, Qt::Horizontal, parent), connected(0)
{
    createJobSplitter(this);
    connectViewerSignals();
    initJobs();
}

void CompareInstanceWidget::addJob(Job *j) {
    JobHeaderWidget *jhw = new JobHeaderWidget(j, 0, jobHeaders, true);
    jobHeadersLayout->insertWidget(jobHeadersLayout->count() - 1, jhw);

    JobWidget *jw = new JobWidget(j, 0, jobsZoomable, true);

    jobsLayout->insertWidget(jobsLayout->count() - 1, jw);
    jobWidgets.insert(j->id(), jw);
    connect(jhw, SIGNAL(expanded(bool)), jw, SLOT(doExpand(bool)));
    jhw->doExpand(true);
    jw->doExpand(true);
}

void CompareInstanceWidget::setActivityColor(Activity *activity, const QColor &color) {
    Job *job = activity->job();
    ActivityWidget* activityWidget = getJobWidget(job)->getWidget(instance->A(job->id(), activity->id()));
    activityWidget->setDiff(true);
    activityWidget->setColor(color);
}

JobWidget* CompareInstanceWidget::getJobWidget(Job *job) {
    return jobWidgets.value(job->id());
}

bool CompareInstanceWidget::eventFilter(QObject *o, QEvent *e) {
    if (o == jobsZoomable && e->type() == QEvent::Resize) {
        QResizeEvent *resizeEvent = static_cast<QResizeEvent *>(e);
        jobTimeline->setFixedWidth(resizeEvent->size().width());
        if(connected) {
            connected->jobsZoomable->setFixedWidth(resizeEvent->size().width());
            connected->jobTimeline->setFixedWidth(resizeEvent->size().width());
        }
    }
    return AbstractInstanceWidget::eventFilter(o, e);
}

void CompareInstanceWidget::connectTo(CompareInstanceWidget *other) {
    connected = other;
    connectSplitters(this, other);
    connectVerticalScrollers(jobsScroller, other->jobsScroller);
    connectHorizontalScrollers(jobsScroller, other->jobsScroller);
    connectZoomables(jobsZoomable, other->jobsZoomable);

    jobTimeline->installEventFilter(this);
    jobsZoomable->installEventFilter(this);
    other->jobTimeline->installEventFilter(this);
    other->jobsZoomable->installEventFilter(this);
}
