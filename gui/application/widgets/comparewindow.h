#ifndef COMPAREWINDOW_H_
#define COMPAREWINDOW_H_

#include <QMainWindow>

#include "model/instance.h"
#include "model/comparison.h"
#include "widgets/compareinstancewidget.h"

/**
 * This window contains two CompareInstanceWidgets to show the differences between
 * two instances. It uses a Comparison object, which contains the instances and
 * provides functions to determine whether they have the same jobs, resources and
 * activities.
 */
class CompareWindow : public QMainWindow
{
    Q_OBJECT
    Comparison *comparison;
    OtsSplitter *view;
    CompareInstanceWidget *top, *bottom;

public:
    /**
     * Constructs a new compare window.
     */
    CompareWindow(Comparison *c);

    /**
     * Destructs the window.
     */
    ~CompareWindow();
private:
    /**
     * Initializes the layout.
     */
    void initLayout();

    /**
     * Determines which activities have different start times in both instances,
     * and, if so, sets the same color.
     */
    void setDiffActivityWidgets();
};

#endif
