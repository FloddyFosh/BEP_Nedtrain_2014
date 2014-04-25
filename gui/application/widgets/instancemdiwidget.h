#ifndef INSTANCETABWIDGET_H
#define INSTANCETABWIDGET_H

#include <QMdiArea>
#include <QSlider>

#include "instancewidget.h"

class InstanceMdiSubWindow;

class MainWindow;

/** Simple QMdiArea subclass with custom addInstance and currentInstance methods
  * to hide the QMdiSubWindow instances that are used underneath.
  */
class InstanceMdiWidget : public QMdiArea
{
    Q_OBJECT

private:
    Controller *controller;
    QSlider * framesSlider;
public:
    /** Sets tabbed mode. */
    explicit InstanceMdiWidget(Controller *controller, QSlider * framesSlider, QWidget *parent = 0);

    /** Creates a QMdiSubWindow containing an InstanceWidget created off of the
      * give instance. */
    void addInstance(Instance *);

    /**
     * Sets the name and file path of a subwindow. When the subwindow is not initialized the active subwindow is used.
     * @param name New name of the subwindow
     * @param w The concerning subwindow
     */
    void updateFileName(QString name, QMdiSubWindow* w = 0);
    Instance *currentInstance() const; ///<  Returns the instance associated with the currently visible tab page.
    QList<Instance*> getAllInstances() const; ///< Returns a list with the instances associated with the tab pages.
    InstanceWidget *currentInstanceWidget() const; ///< Returns the instance widget associated with the currently visible tab page
    InstanceMdiSubWindow * currentInstanceWindow() const;
    bool isInstanceActive() const; ///< determines whether an instance is active
signals:
	void subwindowStateChanged();
public slots:
    void updateSlider(); ///< updates the properties of the slider, according to the current active instance
    void setTimelineMenuOptions(); ///< updates the menu when time is displayed in hours
    
    bool closeSubWindow(); ///< returns whether subwindow was closed
    bool currentInstanceSave(); ///< "save" the current instance
    bool currentInstanceSaveAs(); ///< "save as" the current instance
};

#endif // INSTANCETABWIDGET_H
