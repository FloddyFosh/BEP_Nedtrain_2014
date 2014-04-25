#ifndef ABOUTDIALOG_H_
#define ABOUTDIALOG_H_

#include <QDialog>

/**
 * Dialog with information about the application and authors.
 */
class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructs a new about dialog.
     * @param parent reference to parent widget.
     */
    explicit AboutDialog(QWidget *parent = 0);
};

#endif /* ABOUTDIALOG_H_ */
