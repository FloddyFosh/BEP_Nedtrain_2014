#ifndef COMPAREDIALOG_H
#define COMPAREDIALOG_H

#include <QComboBox>
#include "widgets/dialogs/formdialog.h"
#include "controller/controller.h"

class CompareDialog : public FormDialog
{
    Q_OBJECT
    Controller *controller;
    QComboBox *first, *second;
public:
    explicit CompareDialog(Controller *c, QWidget *parent = 0);


private slots:
	void apply();

};

#endif // COMPAREDIALOG_H
