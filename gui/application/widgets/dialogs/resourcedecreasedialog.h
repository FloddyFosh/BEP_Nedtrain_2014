#ifndef RESOURCEDECREASEDIALOG_H
#define RESOURCEDECREASEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QFormLayout>
#include <QDialogButtonBox>

#include "model/instance.h"

/**
 * Dialog for creating or editing resources. Pass a NULL pointer for the resource object
 * to add a new resource. All processing is done in the apply() slot and outside of this dialog
 * the developer does not have to worry about updating the resource. The UI should be
 * updated based on the signals emitted by the instance and resource object.
 */
class ResourceDecreaseDialog : public QDialog
{
    Q_OBJECT

    QFormLayout *formlayout;
    QSpinBox *capacityEdit, *fromEdit, *tillEdit;
    QDateTimeEdit *fromDateTimeEdit;
    QDateTimeEdit *toDateTimeEdit;
    QSpinBox *fromDayEdit;
    QSpinBox *toDayEdit;
    QDialogButtonBox *buttonbox;
    QVBoxLayout* boxlayout;

    Resource *resource;
    Instance *instance;

    void createLayout(Instance *instance, Resource *res);
    void createSignals();

public:
    ResourceDecreaseDialog(Instance*, Resource*, QWidget*);

private slots:
    /**
     * Adds the resource decrease to the instance.
     */
    void apply();

signals:
    /**
     * Emitted when a resource decrease is being added.
     */
	void decreaseAdded();
};

#endif // RESOURCEDECREASEDIALOG_H
