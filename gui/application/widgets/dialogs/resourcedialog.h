#ifndef RESOURCEDIALOG_H
#define RESOURCEDIALOG_H

#include <QSpinBox>
#include <QListWidget>

#include "widgets/dialogs/formdialog.h"
#include "../../model/instance.h"

/**
 * Dialog for creating or editing resources. Pass a NULL pointer for the resource object
 * to add a new resource. All processing is done in the apply() slot and outside of this dialog
 * the developer does not have to worry about updating the resource. The UI should be
 * updated based on the signals emitted by the instance and resource object.
 */
class ResourceDialog : public FormDialog
{
    Q_OBJECT

    Instance *i;
    Resource *r;

    QLineEdit *nameEdit;
    QSpinBox *capacityEdit;

    QPushButton *applyButton;
    QPushButton *cancelButton;

    QListWidget* decreaseList;
    QVBoxLayout* getDecreaseLayout();

public:
    /**
     * Constructor. Pass an instance object and a job that belongs to this instance
     * or an instance object and a NULL pointer.
     * @param instance instance
     * @param resource resource for which the dialog is created
     * @param parent reference to parent widget
     */
    explicit ResourceDialog(Instance *instance, Resource *resource, QWidget *parent = 0);

private slots:
    void apply(); ///< applies the changes to the resource. If a NULL pointer was passed, a new resource is added to the instance. Otherwise, the changes are applied to the passed resource object
    void showDecreaseDialog(); ///< shows a decrease dialog
    void removeDecrease(); ///< removes selected decrease from resource
    void updateDecreaseList(); ///< updates the decrease list in the interface

signals:
	void decreaseRemoved(ResourceDecrease*); ///< emitted after removal of resource decrease
};

#endif // RESOURCEDIALOG_H
