#ifndef ABOUTDIALOG_H_
#define ABOUTDIALOG_H_

#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>

/**
 * Dialog with information about the application and authors.
 */
class AboutDialog : public QDialog
{
    Q_OBJECT

    QDialogButtonBox *buttonbox;
    QTextEdit *text;
    QLabel *logo;
    QHBoxLayout *horizontal;
    QVBoxLayout *vertical;

    void createLayout();
    void createSignals();

public:
    /**
     * Constructs a new about dialog.
     * @param parent reference to parent widget.
     */
    explicit AboutDialog(QWidget *parent = 0);
};

#endif /* ABOUTDIALOG_H_ */
