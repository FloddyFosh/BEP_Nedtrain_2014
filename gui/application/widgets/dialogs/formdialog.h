#ifndef FORMDIALOG_H_
#define FORMDIALOG_H_

#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>

/**
 * FormDialog serves as a template for dialogs with a form.
 */
class FormDialog : public QDialog {
    Q_OBJECT

    QFormLayout *formLayout;
    QVBoxLayout *verticalLayout;
    QDialogButtonBox *buttonBox;

public:
    explicit FormDialog(QWidget *parent = 0);
protected:

    /**
     * Adds a field to the form.
     * @param labelText The label text for this field.
     * @param field A QWidget that is a form field.
     */
    void addFormField(const QString &labelText, QWidget *field);

    /**
     * Adds a field to the form.
     * @param labelText The label text for this field.
     * @param field A QLayout that is a form field.
     */
    void addFormField(const QString &labelText, QLayout *field);

    /**
     * Adds a field to the form.
     * @param field A QWidget that is a form field.
     */
    void addFormField(QWidget *field);

    /**
     * Adds a widget below the form.
     * @param widget A QWidget that should be added.
     */
    void addWidget(QWidget *widget);

    /**
     * Adds a layout below the form.
     * @param widget A QLayout that should be added.
     */
    void addLayout(QLayout *layout);

    /**
     * Sets up the layout and connects widgets to signals.
     */
    void setUpLayout();
protected slots:
    virtual void apply() = 0; ///< This slot should be implemented by subclasses.
};

#endif /* FORMDIALOG_H_ */
