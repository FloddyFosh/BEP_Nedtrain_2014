#ifndef RESOURCEHEADERWIDGET_H
#define RESOURCEHEADERWIDGET_H

#include <QLabel>
#include <QPushButton>

#include "widgets/headerwidget.h"
#include "widgets/filteredspinbox.h"
#include "../model/instance.h"

class InstanceController;

/**
 * Row header widgets. Used in the bottom-left section of the main visualisation
 * widget, one for every resource in the instance. Shows resource name. Each row
 * is always 8 vZoom units high. The resource demand is scaled such that it fits
 * nicely in the available vertical space.
 */
class ResourceHeaderWidget : public HeaderWidget
{
    Q_OBJECT

    Resource *_resource;
    InstanceController *controller;
    QLabel *nameLabel;
    FilteredSpinBox *spinBox;
    QPushButton *removeButton, *editButton, *upButton, *downButton;

    void createLayout();

public:
    /**
     * Constructor, makes sure the height exactly follows the sizeHint().
     * @param r resource which the header has to represent
     * @param controller the instance controller
     * @param parent reference to parent widget
     */
    explicit ResourceHeaderWidget(Resource *r, InstanceController *controller, QWidget *parent = 0);

    QSize sizeHint() const; ///< returns fixed size, 8 vZoom units high and 0 pixels wide, vertical dimension is set to be fixed while horizontal dimension grows to the available space

private slots:
    void changeAmount(int value); ///< changes resource amount and emits resourceAmountChanged signal
    void removeResource(); ///< removes resource iff there is no demand for the resource, emits remove signal
    void editResource();  ///< activates resource dialog
    void upResource(); ///< sets the resource one higher in the layout index
    void downResource(); ///< sets the resource one lower in the layout index

signals:
    void resourceAmountChanged(); ///< emitted when resource amount changes
    void remove(Resource *); ///< emitted when the resource has been removed
    void decreaseRemoved(ResourceDecrease *); ///< emitted when a resource decrease belonging to the resource has been removed
};

#endif // RESOURCEHEADERWIDGET_H
