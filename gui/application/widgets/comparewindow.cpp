#include "widgets/comparewindow.h"
#include "app_icon.h"

#include <QSettings>

CompareWindow::CompareWindow(Comparison *c) : comparison(c) {
    view = new OtsSplitter(Qt::Vertical, this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Compare instances"));
    setWindowIcon(AppIcon("icon.png"));
    setCentralWidget(view);

    initLayout();
    setDiffActivityWidgets();
}

CompareWindow::~CompareWindow() {
    delete comparison;
}

void CompareWindow::initLayout() {
    top = new CompareInstanceWidget(comparison->getFirstInstance());
    bottom = new CompareInstanceWidget(comparison->getSecondInstance());
    view->addWidget(top);
    view->addWidget(bottom);

    QList<int> sizes;
    sizes << height() / 2 << height() / 3;
    view->setSizes(sizes);
    top->setSizes(sizes);
    bottom->setSizes(sizes);

    top->connectTo(bottom);
}

void CompareWindow::setDiffActivityWidgets() {
    RandomColorGenerator colorGenerator;

    foreach(Job *job, comparison->getFirstInstance()->getJobs()) {
        foreach(Activity *activity, job->getActivities()) {
            if(comparison->hasDifferences(activity)) {
                QColor color = colorGenerator.colorFor(activity->name());
                top->setActivityColor(activity, color);
                bottom->setActivityColor(activity, color);
            }
        }
    }
}
