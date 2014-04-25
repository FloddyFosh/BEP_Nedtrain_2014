#include "dialogs/jobdialog.h"
#include "widgets/app_icon.h"
#include "controller/instancecontroller.h"

#include <QMessageBox>

JobHeaderWidget::JobHeaderWidget(Job *j, InstanceController *controller, QWidget *parent, bool comparing) :
    HeaderWidget(parent), _job(j), _expanded(false), controller(controller), compare(comparing)
{
    nameLabel = new QLabel(_job->name(), this);

    if(!compare) {
        removeButton = new QPushButton(AppIcon("remove.png"),"", this);
        removeButton->setGeometry(10, 5, 20, 20);
        removeButton->setFlat(true);

        editButton = new QPushButton(AppIcon("edit.png"),"", this);
        editButton->setGeometry(10, 5, 20, 20);
        editButton->setFlat(true);

        expandButton = new QPushButton("+", this);
        expandButton->setCheckable(true);
        expandButton->setMaximumWidth(20);
        expandButton->setMaximumHeight(20);
    }

    // make sure sizehint is used as size (at least for height)
    setContentsMargins(0, 2, 0, 0);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(10,0,0,0);

    if(!compare) {
        layout->addWidget(expandButton,0, Qt::AlignTop);
    }

    layout->addWidget(nameLabel,0, Qt::AlignTop);

    if(!compare) {
        layout->addStretch();
        layout->addWidget(editButton,0, Qt::AlignTop);
        layout->addWidget(removeButton,0, Qt::AlignTop);
    }

    setLayout(layout);

    connect(j, SIGNAL(activityAdded(Activity*)), this, SLOT(updateGeometry()));
    connect(j, SIGNAL(activityRemoved()), this, SLOT(updateGeometry()));
    connect(j, SIGNAL(jobChanged()), this, SLOT(updateGeometry()));
    connect(j, SIGNAL(groupAdded(Group *)), this, SLOT(updateGeometry()));
    connect(j, SIGNAL(groupRemoved(Group *)), this, SLOT(updateGeometry()));

    if(!compare){
        //connect toggle signals
        connect(expandButton, SIGNAL(toggled(bool)), this, SIGNAL(expanded(bool)));
        connect(expandButton, SIGNAL(toggled(bool)), this, SLOT(doExpand(bool)));

        //connect edit and remove button
        connect(removeButton, SIGNAL(clicked()), this, SLOT(removeJob()));
        connect(editButton, SIGNAL(clicked()), this, SLOT(editJob()));
    }
}

void JobHeaderWidget::updateGeometry() {
    QWidget::updateGeometry();
}

void JobHeaderWidget::doExpand(bool expanded) {
    this->_expanded = expanded;

    if(!compare) {
        expandButton->setText(_expanded ? "-" : "+");
    }

    updateGeometry();
}

QSize JobHeaderWidget::sizeHint() const {
    int vZoom = 10;
    int verticalBlocks = 2;
    if (_expanded) {
        verticalBlocks += _job->groupCount();
    }
    else {
        verticalBlocks += 1;
    }
    return QSize(0, verticalBlocks * vZoom);
}

void JobHeaderWidget::editJob(){
	JobDialog dialog(_job->instance(), _job, this);
	dialog.exec();
	nameLabel->setText(_job->name());
	_job->instance()->updateStartTime();
	controller->getJobWidgets().value(_job->id())->updateGeometry();
	controller->getInstanceWidget()->updateWidgetsLayout();
}

void JobHeaderWidget::removeJob() {
    controller->stopDrawingDependencies();
    if(!(_job->hasActivities() && QMessageBox::No == QMessageBox::question(this, tr("Are you sure?"), tr("Are you sure you want to remove this job with its activities?"),QMessageBox::Yes | QMessageBox::No)))
        emit remove(_job);
}
