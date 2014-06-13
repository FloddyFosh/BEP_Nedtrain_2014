#include "controller/instancecontroller.h"

InstanceController::InstanceController(Instance * instance, InstanceWidget * instanceWidget, Controller * controller)
: instance (instance), instanceWidget (instanceWidget), controller (controller) { }

Instance * InstanceController::getInstance() const {
    return instance;
}

InstanceWidget * InstanceController::getInstanceWidget() const {
    return instanceWidget;
}

void InstanceController::disconnectActivitiesFromResourceWidgets() {
    instanceWidget->disconnectActivitiesFromResourceWidgets();
}

void InstanceController::reconnectActivitiesToResourceWidgets() {
    instanceWidget->reconnectActivitiesToResourceWidgets();
}

void InstanceController::repaintJobWidgets() {
    instanceWidget->repaintJobWidgets();
}

void InstanceController::repaintResourceWidgets() {
    instanceWidget->repaintResourceWidgets();
}

void InstanceController::setupSlider(bool reset) {
    instanceWidget->setupSlider(reset);
}

void InstanceController::toLastFrame() {
    instanceWidget->toLastFrame();
}

int InstanceController::getFrameNumber(){
	return getInstanceWidget()->getFrameNumber();
}

bool InstanceController::isAtLastFrame(){
	return getInstanceWidget()->isAtLastFrame();
}

QMap<int, JobWidget*> InstanceController::getJobWidgets() {
    return instanceWidget->getJobWidgets();
}

QMap<int, ResourceWidget*> InstanceController::getResourceWidgets() {
    return instanceWidget->getResourceWidgets();
}

QList<ActivityWidget *> InstanceController::getActivityWidgets() {
    return instanceWidget->getActivityWidgets();
}

QColor InstanceController::getColorFor(QString tag) {
    return controller->getColorFor(tag);
}

void InstanceController::showDependencies(ActivityWidget* aw) {
    instanceWidget->getOverlay()->drawDependencies(aw);
}

ActivityWidget* InstanceController::getDependencyWidget() {
    return instanceWidget->getOverlay()->getActWidget();
}

void InstanceController::stopDrawingDependencies() {
    instanceWidget->getOverlay()->stopDrawingDependencies();
}

void InstanceController::stopDrawingDependencies(ActivityWidget* aw) {
    instanceWidget->getOverlay()->stopDrawingDependencies(aw);
}

bool InstanceController::isPaintingFeasibleIntervals() {
    return controller->isPaintingFeasibleIntervals();
}

void InstanceController::doPaintFeasibleIntervals() {
    instanceWidget->updateTimelineOffset();
}

bool InstanceController::isPaintingFlexibilityIntervals() {
    return controller->isPaintingFlexibilityIntervals();
}

void InstanceController::doPaintFlexibilityIntervals() {
    instanceWidget->updateTimelineOffset();
}

Controller * InstanceController::getParent() { return controller; }

void InstanceController::shadeActivities(int start, int end, Resource* r) {
    //clear shade of all resource widgets
    foreach(ResourceWidget* rw, getResourceWidgets()){
        if(rw->getResource() != r)
            rw->removeShade();
    }
    // clear shades of all activity widgets
    foreach(ActivityWidget* aw, getActivityWidgets()){
        aw->setShade(false);
    }
    // paint shade on activity widgets
    if(end != -1){
        foreach(ActivityWidget* aw, getActivityWidgets()){
            Activity* a = aw->activity();
            if(a->st() < end && a->st()+a->duration() > start && a->getRequiredAmount(r) > 0)
                aw->setShade(true);
        }
    }
}

void InstanceController::shadeResources(Job* j) {
    foreach(ResourceWidget* rw, getResourceWidgets()){
        rw->setShadedJob(j);
    }
}

void InstanceController::setStatusMessage(QString message, int timeout) {
    controller->setStatusMessage(message, timeout);
}

void InstanceController::peak(int time, int resource) {
    if(!getResourceWidgets().contains(resource)) return;
    ResourceWidget *rw = getResourceWidgets().value(resource);
    rw->paintPeak(time);
}

void InstanceController::focusResource(int resId){
    instanceWidget->focusResourceWidget(resId);
}
