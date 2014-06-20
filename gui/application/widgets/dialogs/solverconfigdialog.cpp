#include "solverconfigdialog.h"
#include "controller/controller.h"
#include "widgets/app_icon.h"

#include <QFormLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>

SolverConfigDialog::SolverConfigDialog(Controller * controller, QWidget *parent) :
    ListFormDialog(parent), current(0), controller (controller)
{
    setWindowTitle(tr("Configure Solvers"));

    nameEdit = new QLineEdit;
    binaryEdit = new QLineEdit;
    browseButton = new QPushButton(tr("&Browse..."));
    argumentsEdit = new QLineEdit;

    QHBoxLayout *binaryLayout = new QHBoxLayout;
    binaryLayout->addWidget(binaryEdit);
    binaryLayout->addWidget(browseButton);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    QFormLayout *form = new QFormLayout;
    form->addRow(tr("Name:"), nameEdit);
    form->addRow(tr("Binary:"), binaryLayout);
    form->addRow(tr("Arguments:"), argumentsEdit);
    rightLayout->addLayout(form);
    setRightPanelLayout(rightLayout);

    connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
    connect(nameEdit, SIGNAL(textChanged(QString)), this, SLOT(setModified()));
    connect(binaryEdit, SIGNAL(textChanged(QString)), this, SLOT(setModified()));
    connect(argumentsEdit, SIGNAL(textChanged(QString)), this, SLOT(setModified()));

    setUpLayout();
    initSolvers();
}

void SolverConfigDialog::initSolvers() {
    setEditable(false);
    formList->clear();
    solverItems.clear();
    QList<Solver *> solvers = Solver::loadAll();
    for (int i = 0; i < solvers.size(); ++i) {
        Solver *s = solvers[i];
        QListWidgetItem *item = new QListWidgetItem(s->getName());
        item->setData(Qt::UserRole, QVariant::fromValue<void*>(s));
        formList->addItem(item);
        solverItems.insert(s, item);
    }
}

void SolverConfigDialog::maybeChangeSelection(QListWidgetItem *newItem, QListWidgetItem *oldItem) {
    // prevent loops
    if (oldItem == newItem || !newItem) {
        return;
    }

    if (modified) {
        int result = QMessageBox::question(this, tr("Unsaved changes"), tr("You have unsaved changes. Do you want to save them?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel);
        if (result == QMessageBox::Yes) {
            // update ui (might not be necessary)
            oldItem->setText(nameEdit->text());

            // store settings
            apply();
        } else if (result == QMessageBox::Cancel) {
            formList->setCurrentItem(oldItem);
        } else {
            QVariant v = newItem->data(Qt::UserRole);
            Solver *s = reinterpret_cast<Solver *>(v.value<void*>());
            initTo(s);
        }
    } else {
        QVariant v = newItem->data(Qt::UserRole);
        Solver *s = reinterpret_cast<Solver *>(v.value<void*>());
        initTo(s);
    }
}

void SolverConfigDialog::initTo(Solver *s) {
    current = s;
    nameEdit->setText(s->getName());
    binaryEdit->setText(s->getBinary());
    argumentsEdit->setText(s->getArguments());
    setEditable(true);
    modified = false;
}

void SolverConfigDialog::setEditable(bool editable) {
    nameEdit->setDisabled(!editable);
    binaryEdit->setDisabled(!editable);
    argumentsEdit->setDisabled(!editable);
    browseButton->setDisabled(!editable);
    applyButton->setDisabled(!editable);
    removeButton->setDisabled(!editable);
}

void SolverConfigDialog::browse() {
    QString binary = QFileDialog::getOpenFileName(this, tr("Select solver binary"), controller->getWorkingDirectory());
    if (!binary.isEmpty()) {
        binaryEdit->setText(binary);
        controller->setWorkingDirectory(QFileInfo (binary).path());
    }
}

void SolverConfigDialog::apply() {
    current->setName(nameEdit->text());
    current->setBinary(binaryEdit->text());
    current->setArguments(argumentsEdit->text());
    solverItems.value(current)->setText(nameEdit->text());
    modified = false;
}

void SolverConfigDialog::addToList() {
    Solver *s = new Solver(tr("(new)"), QString(), QString());
    QListWidgetItem* item = addObjectToList(s);
    if(item) {
        solverItems.insert(s, item);
        apply();
    }
}

void SolverConfigDialog::removeFromList() {
    if(current && QMessageBox::Yes == QMessageBox::question(this, tr("Really remove solver?"), tr("Are you sure you want to remove this solver?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
        current->remove();
        formList->removeItemWidget(solverItems.value(current));
        solverItems.remove(current);
        modified = false;
        current = 0;
        initSolvers();
    }
}
