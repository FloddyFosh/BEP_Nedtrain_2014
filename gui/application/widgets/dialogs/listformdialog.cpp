#include "widgets/dialogs/listformdialog.h"
#include "widgets/app_icon.h"

#include <QMessageBox>
#include <QPushButton>

ListFormDialog::ListFormDialog(QWidget *parent) : QDialog(parent) {
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowIcon(AppIcon("icon.png"));

    modified = false;

    formList = new QListWidget(this);
    applyButton = new QPushButton(tr("&Apply"));
    addButton = new QPushButton(tr("&Add"));
    removeButton = new QPushButton(tr("&Remove"));
    addRemoveBox = new QHBoxLayout;

    createLayout();
}

void ListFormDialog::createLayout() {
    applyButton->setDefault(true);

    addButton->setIcon(AppIcon("list-add.png"));
    removeButton->setIcon(AppIcon("list-remove.png"));

    addRemoveBox->addWidget(addButton);
    addRemoveBox->addWidget(removeButton);
}

void ListFormDialog::setRightPanelLayout(QLayout *layout) {
    rightPanel = layout;
}

void ListFormDialog::setUpLayout() {
    rightPanel->addWidget(applyButton);

    verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(formList);
    verticalLayout->addLayout(addRemoveBox);

    horizontalLayout = new QHBoxLayout;
    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->addLayout(rightPanel);

    connect(addButton, SIGNAL(clicked()), this, SLOT(addToList()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeFromList()));
    connect(formList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(maybeChangeSelection(QListWidgetItem*,QListWidgetItem*)));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));

    setLayout(horizontalLayout);
}

void ListFormDialog::setModified() {
    modified = true;
}

QListWidgetItem* ListFormDialog::addObjectToList(QObject *object) {
    if (modified) {
        int result = QMessageBox::question(this, tr("Unsaved changes"), tr("You have unsaved changes. Do you want to save them?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel);
        if (result == QMessageBox::Yes)
            apply();
        else if (result == QMessageBox::Cancel)
            return 0;
    }
    modified = false;

    QListWidgetItem *item = new QListWidgetItem(tr("(new)"));
    item->setData(Qt::UserRole, QVariant::fromValue<void*>(object));

    formList->addItem(item);
    formList->setCurrentItem(item);

    return item;
}
