#ifndef MANAGETEMPLATESDIALOG_H
#define MANAGETEMPLATESDIALOG_H

#include "widgets/dialogs/listformdialog.h"
#include "widgets/requirementstable.h"

#include <QFormLayout>

/**
 * Dialog to create and edit templates. State of UI elements is maintained by slots.
 */

class ManageTemplatesDialog : public ListFormDialog
{
    Q_OBJECT

    QListWidget *templateList;

    QFormLayout *form;
    QVBoxLayout *rightLayout;
    QLineEdit *nameEdit;
    QSpinBox *durationEdit;
    RequirementsTable *requirementsTable;
    QMap<Resource *, QSpinBox *> spinners;

    QPushButton *cancelButton;

    Instance *instance;
    ActivityTemplate *selectedTemplate;

    void createLayout();
    void createSignals();

    /**
     * Loads all templates from database into the dialog.
     */
    void initTemplates();

    /**
     * Loads informatino of specified template into editable fields.
     * @param activityTemplate activity template
     */
    void initTo(ActivityTemplate *activityTemplate);

public:
    /**
     * Constructs dialog to manage templates.
     * @param instance instance
     * @param parent parent widget to allow for smart placing of this dialog.
     */
    explicit ManageTemplatesDialog(Instance *instance, QWidget *parent = 0);

private slots:
    void addToList(); ///< adds a new template to the list
    void removeFromList(); ///< removes selected template from the list
    void setEditable(bool); ///<changes the enabled propert of all edit fields to specified value
    void maybeChangeSelection(QListWidgetItem*, QListWidgetItem*); ///< slot to ask if the user wants to change the selected item or continue editing current item, invoked after item selection
    void apply(); ///< saves selected template in the database
};

#endif // MANAGETEMPLATESDIALOG_H
