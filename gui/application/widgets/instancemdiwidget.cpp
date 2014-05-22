#include <QFileInfo>
#include <QMdiSubWindow>
#include <QMessageBox>

#include "widgets/app_icon.h"
#include "controller/instancecontroller.h"

#include "instancemdisubwindow.h"

InstanceMdiWidget::InstanceMdiWidget(Controller *controller, QSlider * framesSlider, QWidget *parent) :
    QMdiArea(parent), controller(controller), framesSlider (framesSlider)
{
    setViewMode(QMdiArea::TabbedView);
    setActivationOrder(QMdiArea::ActivationHistoryOrder);
    setTabsClosable(true);
    connect(this, SIGNAL(subWindowActivated(QMdiSubWindow *)), controller, SLOT(disableToolbarButtons()));
    connect(this, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(updateSlider()));
    connect(this, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(setTimelineMenuOptions()));
}

void InstanceMdiWidget::addInstance(Instance *i) {
    InstanceWidget *iw = new InstanceWidget(i, controller, this);

    QMdiSubWindow *sub = new InstanceMdiSubWindow (iw->getInstanceController(), this);
    sub->setWidget(iw);
    sub->setAttribute(Qt::WA_DeleteOnClose);
    sub->setWindowIcon(AppIcon("icon.png"));
    if(i->getFileName().isEmpty())
        updateFileName(tr("untitled %1").arg(controller->getUntitledNumber()), sub);
    else
        updateFileName(i->getFileName(), sub);
    connect(i, SIGNAL(isModified(bool)), sub, SLOT(setWindowModified(bool)));
    connect(i, SIGNAL(userChange()), this, SLOT(updateSlider()));

    addSubWindow(sub);
    sub->show();
}

void InstanceMdiWidget::updateFileName(QString fileName, QMdiSubWindow* sub) {
    if(!sub)
        sub = currentSubWindow();
    sub->setWindowFilePath(fileName);
    sub->setWindowTitle(QFileInfo(fileName).fileName()+"[*]");
}

Instance *InstanceMdiWidget::currentInstance() const {
    InstanceWidget *iw = currentInstanceWidget();
    return iw ? iw->getInstanceController()->getInstance() : 0;
}

QList<Instance*> InstanceMdiWidget::getAllInstances() const {
	QList<QMdiSubWindow *> subWindows = subWindowList();
	QList<Instance*> res;
	foreach(QMdiSubWindow* sw, subWindows){
		QWidget *w = sw->widget();
		InstanceWidget *iw = static_cast<InstanceWidget *>(w);
		res.append(iw->getInstanceController()->getInstance());
	}
	return res;
}

InstanceWidget *InstanceMdiWidget::currentInstanceWidget() const {
    if (!currentSubWindow())
        return 0;

    QWidget *w = currentSubWindow()->widget();
    InstanceWidget *iw = static_cast<InstanceWidget *>(w);
    return iw;
}

InstanceMdiSubWindow * InstanceMdiWidget::currentInstanceWindow() const {
    QMdiSubWindow * sub = currentSubWindow();
    if (!sub) return 0;
    return static_cast<InstanceMdiSubWindow *> (sub);
}

void InstanceMdiWidget::updateSlider() {
    if (!currentInstanceWidget())
        controller->disableHistory(0);
    else if(currentInstance()->hasUserChanges())
        controller->disableHistory(1);
    else
        currentInstanceWidget()->setupSlider(false);
}

void InstanceMdiWidget::setTimelineMenuOptions() {
    controller->getMainWindow()->synchronizeMenuWithInstance();
}

bool InstanceMdiWidget::isInstanceActive() const {
    return currentInstanceWidget() != NULL;
}

bool InstanceMdiWidget::closeSubWindow() {
    QMdiSubWindow * sub = currentSubWindow();
    if (!sub) return false;
    sub->close();
    return currentSubWindow() != sub;
}

bool InstanceMdiWidget::currentInstanceSave() {
    InstanceMdiSubWindow * sub = currentInstanceWindow();
    if (sub) return sub->save();
    else {
        QMessageBox::warning(this, tr("Save"), tr("There is no instance to save."), QMessageBox::Ok);
        return false;
    }
}

bool InstanceMdiWidget::currentInstanceSaveAs() {
    InstanceMdiSubWindow * sub = currentInstanceWindow();
    if (sub) return sub->saveAs();
    else {
        QMessageBox::warning(this, tr("Save"), tr("There is no instance to save."), QMessageBox::Ok);
        return false;
    }
}
