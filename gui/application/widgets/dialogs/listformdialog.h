#ifndef LISTFORMDIALOG_H_
#define LISTFORMDIALOG_H_

#include <QDialog>
#include <QHBoxLayout>
#include <QListWidget>

/**
 * ListFormDialog serves as a template for dialogs
 * that add, remove and edit items from a list.
 * The list is displayed on the left and the edit form form on the right.
 */
class ListFormDialog : public QDialog {
    Q_OBJECT

    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLayout *rightPanel;
public:
    explicit ListFormDialog(QWidget *parent = 0);
protected:
    bool modified;
    QPushButton *applyButton, *addButton, *removeButton;
    QHBoxLayout *addRemoveBox;
    QListWidget *formList;

    /**
     * Sets the layout that is used for display a form on the right side of the dialog.
     * @param layout The layout, which can be any QLayout.
     */
    void setRightPanelLayout(QLayout *);

    /**
     * Sets up the layout and connects widgets to signals.
     */
    void setUpLayout();

    /**
     * Adds an object to the list, after asking the user for saving changes.
     * @param object An object, like a Solver or ActivityTemplate, that needs to be added.
     * @return Returns a pointer to the created QListWidgetItem.
     */
    QListWidgetItem* addObjectToList(QObject *object);

protected slots:
    virtual void apply() = 0; ///< This slot should be implemented by subclasses.
    virtual void addToList() = 0; ///< This slot should be implemented by subclasses.
    virtual void removeFromList() = 0; ///< This slot should be implemented by subclasses.
    virtual void maybeChangeSelection(QListWidgetItem *newItem, QListWidgetItem *oldItem) = 0;
                                    ///< This slot should be implemented by subclasses.

    /**
     * Sets the modified flag to true.
     */
    void setModified();
};

#endif /* LISTFORMDIALOG_H_ */
