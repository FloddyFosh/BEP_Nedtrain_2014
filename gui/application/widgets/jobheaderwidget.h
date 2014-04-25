#ifndef JOBHEADERWIDGET_H
#define JOBHEADERWIDGET_H

#include <QPushButton>
#include <QLabel>

#include "model/job.h"
#include "widgets/headerwidget.h"

class InstanceController;

/**
 * Row header widgets. Used in the top-left section of the main visualisation
 * widget, one for every job in the instance. Shows job name and has a button
 * to expand the job "row".
 */
class JobHeaderWidget : public HeaderWidget
{
    Q_OBJECT

    Job *_job;
    bool _expanded;
    InstanceController *controller;
    bool compare;
    QPushButton *expandButton, *removeButton, *editButton;
    QLabel *nameLabel;

public:
    /**
     * Constructor, makes sure the height exactly follows the sizeHint().
     * @param job the job which has to be represented by this widget
     * @param controller the instance controller
     * @param parent reference to parent widget
     */
    explicit JobHeaderWidget(Job *job, InstanceController *controller, QWidget *parent = 0, bool compare = true);

    QSize sizeHint() const; ///< gives the widget size depending on the current hZoom, vZoom and expanded state

signals:
    void expanded(bool); ///< emitted when the expanded state changes (button is pressed or depressed)
    void remove(Job *); ///< emitted when removing the job represented by the widget.

private slots:
    void updateGeometry(); ///< make updateGeometry available as slot to connect the expanded signal to
    void removeJob(); ///< remove the job represented by this row.
    void editJob(); ///< shows edit job dialog

public slots:
    void doExpand(bool); ///< handler for the expanded button
};

#endif // JOBHEADERWIDGET_H
