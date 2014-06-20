#include "widgets/dialogs/formdialog.h"

#include "widgets/app_icon.h"

FormDialog::FormDialog(QWidget *parent) : QDialog(parent) {
    formLayout = new QFormLayout;
    verticalLayout = new QVBoxLayout;
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    verticalLayout->addLayout(formLayout);
}

void FormDialog::setUpLayout() {
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowIcon(AppIcon("icon.png"));
    verticalLayout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(apply()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    setLayout(verticalLayout);
}

void FormDialog::addFormField(const QString &labelText, QWidget *field) {
    formLayout->addRow(labelText, field);
}

void FormDialog::addFormField(const QString &labelText, QLayout *field) {
    formLayout->addRow(labelText, field);
}

void FormDialog::addFormField(QWidget *field) {
    formLayout->addRow(field);
}

void FormDialog::addWidget(QWidget *widget) {
    verticalLayout->addWidget(widget);
}

void FormDialog::addLayout(QLayout *layout) {
    verticalLayout->addLayout(layout);
}
