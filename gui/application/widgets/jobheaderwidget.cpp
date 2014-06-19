#include "dialogs/jobdialog.h"
#include "widgets/app_icon.h"
#include "controller/instancecontroller.h"

#include <QMessageBox>

JobHeaderWidget::JobHeaderWidget(Job *j, InstanceController *controller, QWidget *parent, bool comparing) :
    HeaderWidget(parent), _job(j), _expanded(false), controller(controller), compare(comparing), highLighted(false)
{
    nameLabel = new QLabel(_job->name(), this);
    nameLabel->setContentsMargins(10,3,0,0);

    if(!compare) {
        removeButton = new QPushButton(AppIcon("remove.png"),"", this);
        removeButton->setGeometry(10, 5, 10, 10);
        removeButton->setFlat(true);
        removeButton->setFocusPolicy(Qt::NoFocus);

        editButton = new QPushButton(AppIcon("edit.png"),"", this);
        editButton->setGeometry(10, 5, 10, 10);
        editButton->setFlat(true);
        editButton->setFocusPolicy(Qt::NoFocus);

        downButton = new QPushButton(AppIcon("down.png"),"", this);
        downButton->setGeometry(10, 5, 10, 10);
        downButton->setFlat(true);
        downButton->setFocusPolicy(Qt::NoFocus);

        upButton = new QPushButton(AppIcon("up.png"),"", this);
        upButton->setGeometry(10, 5, 10, 10);
        upButton->setFlat(true);
        upButton->setFocusPolicy(Qt::NoFocus);

        expandButton = new QPushButton("+", this);
        expandButton->setCheckable(true);
        expandButton->setMaximumWidth(20);
        expandButton->setMaximumHeight(20);
    }

    // make sure sizehint is used as size (at least for height)
    setContentsMargins(10, 2, 0, 0);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    setBackgroundRole(QPalette::Background);
    setAutoFillBackground(true);

    createLayout();

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

        //connect up and down button
        connect(upButton, SIGNAL(clicked()), this, SLOT(upJob()));
        connect(downButton, SIGNAL(clicked()), this, SLOT(downJob()));
    }
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

void JobHeaderWidget::createLayout() {
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    if(!compare) {
        layout->addWidget(expandButton,0, Qt::AlignTop|Qt::AlignLeft);
    }

    layout->addWidget(nameLabel,0, Qt::AlignTop|Qt::AlignLeft);

    if(!compare) {
        layout->addStretch();
        layout->addWidget(upButton,0, Qt::AlignTop|Qt::AlignRight);
        layout->addWidget(downButton,0, Qt::AlignTop|Qt::AlignRight);
        layout->addWidget(editButton,0, Qt::AlignTop|Qt::AlignRight);
        layout->addWidget(removeButton,0, Qt::AlignTop|Qt::AlignRight);
    }

    setLayout(layout);
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
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Are you sure?"));
    msgBox.setWindowIcon(AppIcon("icon.png"));
    msgBox.setText("Are you sure you want to remove this job with its activities?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    if(!(_job->hasActivities() && QMessageBox::No == msgBox.exec()))
        emit remove(_job);
}

void JobHeaderWidget::upJob() {
    int index = controller->getInstanceWidget()->getJobIndex(this);
    controller->getInstanceWidget()->relocateJobWidget(index,index-1);
}

void JobHeaderWidget::downJob() {
    int index = controller->getInstanceWidget()->getJobIndex(this);
    controller->getInstanceWidget()->relocateJobWidget(index, index+1);
}

void JobHeaderWidget::highlight(bool hl) {
    if(hl && !highLighted) {
        QPalette pal = palette();
        pal.setColor(backgroundRole(), QColor(255, 247, 160));
        setPalette(pal);
        nameLabel->setText("<b>" + _job->name() + "</b>");
        highLighted = true;
    }
    else if(highLighted) {
        setPalette(QPalette());
        nameLabel->setText(_job->name());
        highLighted = false;
    }
}
