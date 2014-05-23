#include "jobdialog.h"
#include "controller/exceptions.h"

#include <QMessageBox>
#include <QDateTimeEdit>

JobDialog::JobDialog(Instance *i, Job *j, QWidget *parent) :
    FormDialog(parent), i(i), j(j)
{
    if (!j) {
        setWindowTitle(tr("New Job"));
    } else {
        setWindowTitle(tr("Edit Job"));
    }
    nameEdit = new QLineEdit(j ? j->name() : QString());
    nameEdit->setMaxLength(40);

    if(i->hoursOnTimeline()) {
        releaseDateTimeEdit = new QDateTimeEdit;
        if(j) releaseDateTimeEdit->setTime(QTime(0,0,0).addSecs(j->releaseDate()*60));
        releaseDateTimeEdit->setDisplayFormat("h:mm");

        releaseDayEdit = new QSpinBox;
        releaseDayEdit->setMinimum(1);
        releaseDayEdit->setMaximum(1 << 30);
        if(j) releaseDayEdit->setValue(j->releaseDate()/(60*24)+1);

        dueDateTimeEdit = new QDateTimeEdit;
        if(j) dueDateTimeEdit->setTime(QTime(0,0,0).addSecs(j->dueDate()*60));
        dueDateTimeEdit->setDisplayFormat("h:mm");

        dueDayEdit = new QSpinBox;
        dueDayEdit->setMinimum(1);
        dueDayEdit->setMaximum(1 << 30);
        if(j) dueDayEdit->setValue(j->dueDate()/(60*24)+1);
    }else {
        releaseDateEdit = new QSpinBox;
        releaseDateEdit->setMinimum(0);
        releaseDateEdit->setMaximum(1 << 30);
        releaseDateEdit->setValue(j ? j->releaseDate() : 0);
        dueDateEdit = new QSpinBox;
        dueDateEdit->setMinimum(0);
        dueDateEdit->setMaximum(1 << 30);
        dueDateEdit->setValue(j ? j->dueDate() : 1);
    }

    addFormField(tr("&Name:"), nameEdit);
    if(i->hoursOnTimeline()) {
        addFormField(tr("&Release day:"), releaseDayEdit);
        addFormField(tr("&Release time:"), releaseDateTimeEdit);
        addFormField(tr("&Due day:"), dueDayEdit);
        addFormField(tr("&Due time:"), dueDateTimeEdit);
    } else {
        addFormField(tr("&Release date:"), releaseDateEdit);
        addFormField(tr("&Due date:"), dueDateEdit);
    }
    setUpLayout();
}

void JobDialog::apply() {
    int releaseDate, dueDate;

    if(i->hoursOnTimeline()) {
        QTime release = releaseDateTimeEdit->time();
        releaseDate = QTime(0,0,0).secsTo(release)/60 + ((releaseDayEdit->value()-1)*60*24);

        QTime due = dueDateTimeEdit->time();
        dueDate = QTime(0,0,0).secsTo(due)/60 + ((dueDayEdit->value()-1)*60*24);
    } else {
        releaseDate = releaseDateEdit->value();
        dueDate = dueDateEdit->value();
    }

    QString name = nameEdit->text().trimmed();
    if(name.isEmpty())
        QMessageBox::information(this, tr("Invalid input"), tr("Please fill in a name."));
    else if(dueDate <= releaseDate)
        QMessageBox::information(this, tr("Invalid input"), tr("The due date should be after the release date.\nPlease correct your input."));
    else {
        if (!j) {
            try {
                i->addJob(releaseDate, dueDate, name);
            } catch(InstanceManipulationException const& e) { e.printWarning(); }
        } else {
            j->setName(name);
            j->setReleaseDate(releaseDate);
            j->setDueDate(dueDate);
        }
        i->setUserChanges(true);
        accept();
    }
}
