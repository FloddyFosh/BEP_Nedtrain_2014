#include "resourceheaderwidget.h"
#include "dialogs/resourcedialog.h"
#include "dialogs/scrollfilter.h"
#include "widgets/app_icon.h"
#include "controller/instancecontroller.h"

#include <QMessageBox>

ResourceHeaderWidget::ResourceHeaderWidget(Resource *r, InstanceController *controller, QWidget *parent) :
    HeaderWidget(parent), _resource(r), controller(controller)
{
    nameLabel = new QLabel(_resource->name(), this);
    nameLabel->setGeometry(35, 0, 300, 30);

    spinBox = new FilteredSpinBox(this);
    spinBox->setGeometry(35, 100, 20, 30);
    spinBox->setValue(_resource->capacity());
    spinBox->setFocusPolicy(Qt::StrongFocus);

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

    ScrollFilter *filter = new ScrollFilter(this);
    spinBox->installEventFilter(filter);

    setContentsMargins(0, 0, 0, 0);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    setBackgroundRole(QPalette::Background);
    setAutoFillBackground(true);

    createLayout();

    //connect spinbox signal
    connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(changeAmount(int)));

    //connect edit and remove button
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeResource()));
    connect(editButton, SIGNAL(clicked()), this, SLOT(editResource()));

    //connect up and down button
    connect(upButton, SIGNAL(clicked()), this, SLOT(upResource()));
    connect(downButton, SIGNAL(clicked()), this, SLOT(downResource()));
}

QSize ResourceHeaderWidget::sizeHint() const {
    int vZoom = 10;
    return QSize(0, 8 * vZoom);
}

void ResourceHeaderWidget::createLayout() {
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(10,5,0,0);
    layout->addWidget(nameLabel,0, Qt::AlignTop|Qt::AlignLeft);
    layout->addStretch();
    layout->addWidget(spinBox, 0, Qt::AlignTop|Qt::AlignRight);
    layout->addWidget(upButton,0, Qt::AlignTop|Qt::AlignRight);
    layout->addWidget(downButton,0, Qt::AlignTop|Qt::AlignRight);
    layout->addWidget(editButton, 0, Qt::AlignTop|Qt::AlignRight);
    layout->addWidget(removeButton, 0, Qt::AlignTop|Qt::AlignRight);
    setLayout(layout);
}

void ResourceHeaderWidget::changeAmount(int value) {
    _resource->setCapacity(value);
    _resource->instance()->setUserChanges(true);
    emit resourceAmountChanged();
}

void ResourceHeaderWidget::removeResource() {
    if(_resource->hasActivities())
        QMessageBox::warning(this, tr("Cannot remove"), tr("This resource is required by activities and cannot be removed."),QMessageBox::Ok);
    else
        emit remove(_resource);
}

void ResourceHeaderWidget::editResource(){
	ResourceDialog* dialog = new ResourceDialog(_resource->instance(), _resource, this);
	connect(dialog, SIGNAL(decreaseRemoved(ResourceDecrease*)), this, SIGNAL(decreaseRemoved(ResourceDecrease*)));
	dialog->exec();
	spinBox->setValue(_resource->capacity());
	nameLabel->setText(_resource->name());
}

void ResourceHeaderWidget::upResource() {
    int index = controller->getInstanceWidget()->getResourceIndex(this);
    controller->getInstanceWidget()->relocateResourceWidget(index,index-1);
}

void ResourceHeaderWidget::downResource() {
    int index = controller->getInstanceWidget()->getResourceIndex(this);
    controller->getInstanceWidget()->relocateResourceWidget(index, index+1);
}
