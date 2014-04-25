#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDateTimeEdit>

#include "widgets/mainwindow.h"
#include "widgets/dialogs/formdialog.h"

/**
 * Dialog to import instances in the currently loaded instance. Supports both 'normal' and hour mode.
 */
class ImportDialog : public FormDialog
{
    Q_OBJECT

    MainWindow* mw;
    InstanceController* controller;

    QLineEdit *fileEdit;
    QPushButton *browseButton;
    QSpinBox *timeEdit;
    QDateTimeEdit *timeEditHour;
public:
    /**
     * Constructor, initializes the ui
     * @param parent reference to parent widget
     * @param controller controller
     */
    explicit ImportDialog(MainWindow *parent, InstanceController *controller);

private slots:
    void apply(); ///< imports the selected file in the current instance
    void browse(); ///< opens dialog to select a file and updates fileEdit field
};

#endif // IMPORTDIALOG_H
