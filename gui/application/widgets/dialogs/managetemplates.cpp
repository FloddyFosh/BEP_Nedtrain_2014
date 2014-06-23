#include "managetemplates.h"

#include "data/template_gateway.h"
#include "widgets/app_icon.h"

#include <QMessageBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>

ManageTemplatesDialog::ManageTemplatesDialog(Instance *inst, QWidget *parent) :
    ListFormDialog(parent), instance(inst)
{
    setWindowTitle(tr("Manage templates"));

    nameEdit = new QLineEdit;
    durationEdit = new QSpinBox;
    durationEdit->setMinimum(1);
    durationEdit->setMaximum(INT_MAX);

    QFormLayout *form = new QFormLayout;
    form->addRow(tr("&Name:"), nameEdit);
    form->addRow(tr("&Duration:"), durationEdit);

    // requirements table
    requirementsTable = new RequirementsTable(instance);
    requirementsTable->setEditableBy(this);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addLayout(form);
    rightLayout->addWidget(requirementsTable);
    setRightPanelLayout(rightLayout);

    connect(nameEdit, SIGNAL(textChanged(QString)), this, SLOT(setModified()));
    connect(durationEdit, SIGNAL(valueChanged(QString)), this, SLOT(setModified()));

    setUpLayout();
    initTemplates();
}

void ManageTemplatesDialog::initTemplates() {
    setEditable(false);
    formList->clear();
    foreach(ActivityTemplate *a, TemplateGateway::getAll()) {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setData(Qt::UserRole, QVariant::fromValue<void *>(a));
        newItem->setText(a->getName());
        formList->addItem(newItem);
    }
}

void ManageTemplatesDialog::maybeChangeSelection(QListWidgetItem *newItem, QListWidgetItem *oldItem) {
    // prevent loops
    if (oldItem == newItem || !newItem) {
        return;
    }

    bool makeChange = true;
    if (modified) {
        int result = QMessageBox::question(this, tr("Unsaved changes"), tr("You have unsaved changes. Do you want to save them?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel);
        if (result == QMessageBox::Yes) {
            oldItem->setText(nameEdit->text());
            apply();
        }
        else if (result == QMessageBox::Cancel) {
            makeChange = false;
            formList->setCurrentItem(oldItem);
        }
    }
    if(makeChange) {
        QVariant v = newItem->data(Qt::UserRole);
        ActivityTemplate *a = reinterpret_cast<ActivityTemplate *>(v.value<void *>());
        initTo(a);
    }
}

void ManageTemplatesDialog::initTo(ActivityTemplate *a) {
    selectedTemplate = a;
    if(!a->isCompatibleWith(instance)) {
        QString message = tr("The current template cannot be edited. Open an instance with the following resources or add them to the current instance:\n")
                            +"\n"+a->getResourceNames();
        QMessageBox::warning(this, tr("Incomplete resources"), message);
        setEditable(false);
    } else {
        setEditable(true);
        nameEdit->setText(a->getName());
        durationEdit->setValue(a->getDuration());
        requirementsTable->setActivityTemplate(a);
        modified = false;
    }
}

void ManageTemplatesDialog::setEditable(bool editable) {
    nameEdit->setDisabled(!editable);
    durationEdit->setDisabled(!editable);
    requirementsTable->setDisabled(!editable);
    applyButton->setDisabled(!editable);
    removeButton->setDisabled(!editable);
}

void ManageTemplatesDialog::apply() {
    selectedTemplate->setName(nameEdit->text().trimmed());
    selectedTemplate->setDuration(durationEdit->value());
    requirementsTable->applyToActivity();
    QList<QListWidgetItem *> selected = formList->selectedItems();
    if(selected.size() == 1)
        selected.first()->setText(nameEdit->text());

    if(selectedTemplate->getID() == -1)
        TemplateGateway::save(selectedTemplate);
    else
        TemplateGateway::update(selectedTemplate);

    modified = false;
}

void ManageTemplatesDialog::addToList() {
    ActivityTemplate *a = new ActivityTemplate("", 0);
    addObjectToList(a);
}

void ManageTemplatesDialog::removeFromList() {
    if (QMessageBox::Yes == QMessageBox::question(this, tr("Really remove template?"), tr("Are you sure you want to remove this template?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
        TemplateGateway::remove(selectedTemplate);
        setEditable(false);
        initTemplates();

        modified = false;
    }
}
