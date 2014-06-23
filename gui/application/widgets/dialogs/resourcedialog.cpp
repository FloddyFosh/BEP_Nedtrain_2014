#include <QMessageBox>
#include <QLabel>
#include <QPushButton>

#include "resourcedialog.h"
#include "resourcedecreasedialog.h"
#include "controller/exceptions.h"

ResourceDialog::ResourceDialog(Instance *i, Resource *r, QWidget *parent) :
    FormDialog(parent), i(i), r(r)
{
    if(r)
        setWindowTitle(tr("Edit Resource"));
    else
        setWindowTitle(tr("New Resource"));

    nameEdit = new QLineEdit(r ? r->name() : QString());
    capacityEdit = new QSpinBox;

    createLayout(r);
    setUpLayout();
}

void ResourceDialog::createLayout(Resource *r) {
    nameEdit->setMaxLength(40);

    capacityEdit->setMinimum(1);
    capacityEdit->setMaximum(99);
    capacityEdit->setValue(r ? r->capacity() : 1);

    addFormField(tr("&Name:"), nameEdit);
    addFormField(tr("&Capacity:"), capacityEdit);

    if(r)
        addLayout(getDecreaseLayout());
}

QVBoxLayout* ResourceDialog::getDecreaseLayout(){
	QVBoxLayout *boxlayout = new QVBoxLayout;

	// resource decrease list
	decreaseList = new QListWidget(this);
	updateDecreaseList();

	// add/remove buttons
	QHBoxLayout *addremovebox = new QHBoxLayout;
	QPushButton *addButton, *removeButton;
	addButton = new QPushButton(tr("Add"));
	removeButton = new QPushButton(tr("Remove"));
	addButton->setIcon(QIcon("application/icons/list-add.png"));
	removeButton->setIcon(QIcon("application/icons/list-remove.png"));
	addremovebox->addWidget(addButton);
	addremovebox->addWidget(removeButton);

	boxlayout->addWidget(new QLabel(tr("Decreased resource capacity:")));
	boxlayout->addWidget(decreaseList);
	boxlayout->addLayout(addremovebox);

	connect(addButton, SIGNAL(clicked()), this, SLOT(showDecreaseDialog()));
	connect(removeButton, SIGNAL(clicked()), this, SLOT(removeDecrease()));

	return boxlayout;
}

void ResourceDialog::updateDecreaseList(){
	decreaseList->clear();

	foreach(ResourceDecrease* d, r->getDecreases()){
		QString itemText = "";

		if(i->hoursOnTimeline()){
		    QTime from = QTime(0,0,0).addSecs(d->getFrom()*60);
		    QTime to = QTime(0,0,0).addSecs(d->getTill()*60);
		    int dayFrom = d->getFrom()/(60*24)+1;
		    int dayTo = d->getTill()/(60*24)+1;

		    itemText =  from.toString("h:mm") + " (" + tr("day") + " " + QString::number(dayFrom) + ") - " +
		                to.toString("h:mm")   + " (" + tr("day") + " " + QString::number(dayTo) + ") - " +
		                        QString::number(d->getAmount());
		}
		else{
		    itemText = QString::number(d->getFrom()) + " - " +
		            QString::number(d->getTill())   + " (" +
		            QString::number(d->getAmount()) + ")";
		}

		QListWidgetItem *item = new QListWidgetItem(itemText);
		item->setData(Qt::UserRole, QVariant::fromValue<void*>(d));
		decreaseList->addItem(item);
	}
}

void ResourceDialog::showDecreaseDialog(){
	ResourceDecreaseDialog* rdd = new ResourceDecreaseDialog(i, r, this);
	connect(rdd, SIGNAL(decreaseAdded()), this, SLOT(updateDecreaseList()));
	rdd->exec();
}

void ResourceDialog::removeDecrease(){
	QList<QListWidgetItem *> selected = decreaseList->selectedItems();

	foreach(QListWidgetItem* listItem, selected){
		QVariant v = listItem->data(Qt::UserRole);
		ResourceDecrease *d = reinterpret_cast<ResourceDecrease *>(v.value<void*>());
		r->removeDecrease(d);

		emit decreaseRemoved(d);
	}
	updateDecreaseList();

}

void ResourceDialog::apply() {
    QString name = nameEdit->text().trimmed();
    Resource *existing = i->containsResource(name);

    if(name.isEmpty()) {
        QMessageBox::information(this, tr("Invalid input"), tr("Please fill in a name."));
    } else if( (!r && existing) || (r && existing && existing->id() != r->id()) ) {
        QMessageBox::warning(this, tr("Resource exists already"), tr("A resource with the same name does exist already."));
    } else {
        if (!r) {
            int id = -1; // find first free id
            while (i->getResources().count(++id) > 0) {}
            try {
                i->addResource(id, capacityEdit->value(), name);
            } catch(InstanceManipulationException const& e) { e.printWarning(); }
        } else {
            r->setName(name);
            r->setCapacity(capacityEdit->value());
        }
        i->setUserChanges(true);
        accept();
    }
}

