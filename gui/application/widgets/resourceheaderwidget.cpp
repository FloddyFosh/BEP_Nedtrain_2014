#include "resourceheaderwidget.h"
#include "dialogs/resourcedialog.h"
#include "dialogs/scrollfilter.h"
#include "widgets/app_icon.h"

#include <QMessageBox>

ResourceHeaderWidget::ResourceHeaderWidget(Resource *r, QWidget *parent) :
    HeaderWidget(parent), _resource(r)
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

    editButton = new QPushButton(AppIcon("edit.png"),"", this);
    editButton->setGeometry(10, 5, 10, 10);
    editButton->setFlat(true);

    ScrollFilter *filter = new ScrollFilter(this);
    spinBox->installEventFilter(filter);

    setContentsMargins(0, 0, 0, 0);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    setBackgroundRole(QPalette::Background);
    setAutoFillBackground(true);

    createLayout();
    connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(changeAmount(int)));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeResource()));
    connect(editButton, SIGNAL(clicked()), this, SLOT(editResource()));
}

QSize ResourceHeaderWidget::sizeHint() const {
    int vZoom = 10;
    return QSize(0, 8 * vZoom);
}

void ResourceHeaderWidget::createLayout() {
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(10,0,0,0);
    layout->addWidget(nameLabel);
    layout->addStretch();
    layout->addWidget(spinBox);
    layout->addWidget(editButton);
    layout->addWidget(removeButton);
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
