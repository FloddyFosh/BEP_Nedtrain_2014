#ifndef INSTANCEMDISUBWINDOW_H
#define INSTANCEMDISUBWINDOW_H

#include <QMdiSubWindow>
#include <QCloseEvent>

class InstanceController;

class InstanceMdiSubWindow : public QMdiSubWindow
{
    Q_OBJECT

private:
    InstanceController *controller;
public:
    /** Sets tabbed mode. */
    explicit InstanceMdiSubWindow(InstanceController *controller, QWidget *parent = 0);
    void closeEvent(QCloseEvent * closeEvent);
    
    bool save(); ///< handles the save action
    bool saveAs(); ///< handles the save as action
    bool saveFile(const QString &fileName); ///< save the active instance
private:
    void updateFileName(const QString &fileName);
};

#endif
