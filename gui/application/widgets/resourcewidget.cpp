#include "resourcewidget.h"

#include "instancewidget.h"
#include "controller/controller.h"
#include "controller/instancecontroller.h"
#include "model/chain.h"
#include "model/frame.h"
#include "model/chainframe.h"
#include "model/requirement.h"
#include "util/resourcecalculator.h"
#include "activitywidget.h"

#include <QPainter>
#include <cmath>

ResourceWidget::ResourceWidget(Resource *r, InstanceController *c, QWidget *parent) :
    QWidget(parent), _resource(r), controller(c), matrixViewEnabled(false)
{
    offset=0;
    selected = QPoint(-1,-1); //dont select any part of the resource
    calculator = new ResourceCalculator(r);
    _hZoom = 8;
    _vZoom = 10;
    peak = -1;

    // set height fixed to sizehint
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    // report mouse movement to parent
    setMouseTracking(true);

    // dont draw background because then we would not see the vertical mouse follower guide line ruler thing
    setAutoFillBackground(false);

    connectSignals();
    connectActivities();

    calculateResourceProfile();
}

void ResourceWidget::connectSignals() {
    connect(_resource, SIGNAL(resourceChanged()), this, SLOT(calculateResourceProfile()));
    connect(_resource, SIGNAL(activityAdded(Activity*)), this, SLOT(newActivity(Activity*)));
    connect(controller->getInstanceWidget(), SIGNAL(viewButtonTriggered(bool)), this, SLOT(setMatrixViewEnabled(bool)));
}

void ResourceWidget::connectActivities() {
    foreach(Requirement * q, _resource->getRequirements()) {
        connect(q->activity(), SIGNAL(activityChanged()), this, SLOT(calculateResourceProfile()));
    }
}

void ResourceWidget::disconnectActivities() {
    foreach(Requirement * q, _resource->getRequirements()) {
        disconnect(q->activity(), SIGNAL(activityChanged()), this, SLOT(calculateResourceProfile()));
    }
}

void ResourceWidget::newActivity(Activity *a) {
    connect(a, SIGNAL(activityChanged()), this, SLOT(calculateResourceProfile()));
}

QSize ResourceWidget::sizeHint() const {
    return minimumSize();
}

void ResourceWidget::addResourceMark(int time){
    resourceMarks.append(time);
    updatePixmap();
}

void ResourceWidget::clearResourceMarks(){
    resourceMarks.clear();
    updatePixmap();
}

QSize ResourceWidget::minimumSize() const {
    return QSize(100 + calculator->getMaximumEet() * hZoom(), 8 * vZoom());
}

void ResourceWidget::calculateResourceProfile() {
    calculator->calculate(); // calculates all the profiles
    calculator->getExceedPolygon(size().width()/hZoom());

    if(selected != QPoint(-1,-1)){
        if(viewingResourceMatrix()){
            Activity* act = calculator->selectedActivity(selected);
            if(act){
                int rows = calculator->getRowCount();
                double factor = size().height()/rows+0.5;
                QPoint bottomLeft = QPoint((act->st()+offset)*hZoom(), selected.y()*factor);
                QPoint topRight = QPoint((act->st()+act->duration()+offset)*hZoom(),(selected.y()-1)*factor);
                pattern = QRect(bottomLeft, topRight);
                controller->shadeActivity(act, _resource);
            }
            else{
                pattern = QRect(-1,-1,-1,-1);
                selected = QPoint(-1,-1);
                controller->shadeActivities(-1,-1,_resource);
            }
        }
        else{
            //determine which part is selected
            QRect selectedRect = calculator->selectedResourceRegion(selected);
            int start = selectedRect.x();
            int end = selectedRect.x()+selectedRect.width();
            if(start!=-1 && end !=-1){
                int rows = calculator->getRowCount();
                double factor = size().height()/rows;
                pattern = QRect(QPoint((start+offset)*hZoom(), selected.y()*factor), QPoint((end+offset)*hZoom(),(selected.y()-1)*factor));
                controller->shadeActivities(start,end,_resource);
            }
            else{
                pattern = QRect(-1,-1,-1,-1);
                selected = QPoint(-1,-1);
                controller->shadeActivities(-1,-1,_resource);
            }
        }
    }

    updateGeometry();
    updatePixmap();
}

void ResourceWidget::resizeEvent(QResizeEvent *) {
    updatePixmap();
}

void ResourceWidget::updatePixmap() {
    //get horizontal offset iff an instance has been loaded
    Instance* instance = controller->getInstance();
    offset = instance->getTimeOffset();

    // determine size of this widget
    QSize hint = size();

    // clear pixmap
    pixmap = QPixmap(hint); // should maybe use sizeHint() instead?
                              // hint may be more accurate than actual size because
                              // it may have just changed due to calculateDemandProfile()
    pixmap.fill(QColor::fromRgbF(1, 1, 1, 0)); // fill background

    // init painter
    QPainter painter(&pixmap);
    painter.initFrom(this);
    painter.save();

    // turn upside-down
    painter.scale(1, -1);
    painter.translate(0, -hint.height());
    painter.save();

    // scale hZoom and row height
    int rows = calculator->getRowCount();
    painter.scale(hZoom(), hint.height() / (double) rows);
    painter.translate(offset,0);

    if(viewingResourceMatrix()){
        paintChainMatrix(painter);
    }
    else{
        //1. draw demand profile
        paintDemandProfile(painter); // uses scaled painter

        //2. draw resource profile
        paintResourceProfile(painter); //uses scaled painter

        //3. draw job profile
        paintJobProfile(painter);  //uses scaled painter

        //4. draw resources used by selected chain
        paintChainResources(painter);
    }

    painter.restore(); //restore scaled painter

    //5. draw selected region
    paintSelectedRegion(painter); //does not use scaled painter

    //6. draw resource marks
    paintResourceMarks(painter); //does not use scaled painter

    paintPeak(painter); //does not use scaled painter

    painter.restore();


    // draw bottom-border
    painter.setPen(QPen(QColor("black"), 0, Qt::SolidLine));
    painter.setPen(QPen(QBrush(Qt::darkGray), 2));
    painter.drawLine(0, hint.height() - 1, hint.width(), hint.height() - 1);

    // schedule paintEvent
    update();
}

void ResourceWidget::paintDemandProfile(QPainter &painter){
    int rows = calculator->getRowCount();

    QPolygon polygon = calculator->getDemandPolygon();

    if (!polygon.isEmpty()) {
        // create gradient
        QLinearGradient usedCapacityGradient(0, 0, 0, rows);
        usedCapacityGradient.setColorAt(0, Qt::green);
        usedCapacityGradient.setColorAt(_resource->capacity() / (double) rows, Qt::yellow);

        // setup painter style
        painter.setBrush(usedCapacityGradient);
        painter.setPen(QPen(QColor("black"), 0, Qt::SolidLine));

        // draw demand profile
        painter.drawPolygon(polygon);
    }
}

void ResourceWidget::paintResourceProfile(QPainter &painter){
    QPolygon polyline = calculator->getResourcePolyline(size().width()/hZoom()-offset);
    if (!polyline.isEmpty()) {
        painter.setPen(QPen(QColor("black"), 0, Qt::DotLine));
        painter.drawPolyline(polyline);
    }

    QPolygon exceed = calculator->getExceedPolygon(size().width()/hZoom()-offset);
    if(!exceed.isEmpty()){
        painter.setBrush(Qt::red);
        painter.setPen(Qt::NoPen);
        painter.drawPolygon(exceed);

        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor("black"), 0, Qt::SolidLine));
        painter.drawPolygon(calculator->getDemandPolygon());
    }
}
void ResourceWidget::paintJobProfile(QPainter &painter){
    QPolygon polygon = calculator->getJobPolygon();
    //polygon.translate(offset,0);

    if(!polygon.isEmpty()){
        painter.setBrush(QColor(0,0,0,120));
        painter.setPen(Qt::NoPen);
        painter.drawPolygon(polygon);
    }
}

void ResourceWidget::paintSelectedRegion(QPainter& painter){
     if(pattern.x() != -1){
        painter.save();
        painter.setBrush(Qt::BDiagPattern);
        painter.setPen(Qt::NoPen);
        painter.drawRect(pattern);
        painter.restore();
    }
}

void ResourceWidget::paintResourceMarks(QPainter& painter){
    double vScale = (double) height()/calculator->getRowCount();
    foreach(int time, resourceMarks){
        QPoint usage = calculator->getResourceUsageAt(time);
        painter.drawEllipse(QPointF((time+offset)*hZoom(), (double)usage.y()*vScale),4, 4);
    }
}

void ResourceWidget::paintChainResources(QPainter& painter){
    int frameNr = controller->getFrameNumber();
    Instance* inst = controller->getInstance();
    if(inst->getMaxFrameNr() != -1){
        ChainFrame* curFrame = (ChainFrame*) inst->getFrame(frameNr);
        if(QList<QPoint*>* selectedProfile = curFrame->getSelectedProfile()){
            if(_resource->id() != curFrame->getChain()->resourceId()) return;
            int rows = calculator->getRowCount();
            QLinearGradient grad(0, 0, 0, rows);
            grad.setColorAt(0, QColor(50,50,255));
            grad.setColorAt(_resource->capacity() / (double) rows, QColor(200,200,255));
            painter.setBrush(grad);
            painter.setPen(QPen(QColor("black"), 0, Qt::NoPen));
            painter.drawPolygon(calculator->getChainPolygon(selectedProfile));
        }
        if(QList<QPoint*>* usedProfile = curFrame->getUsedProfile()){
            if(_resource->id() != curFrame->getChain()->resourceId()) return;
            int rows = calculator->getRowCount();
            QLinearGradient grad(0, 0, 0, rows);
            grad.setColorAt(0, QColor(50,50,50));
            grad.setColorAt(_resource->capacity() / (double) rows, QColor(200,200,200));
            painter.setBrush(grad);
            painter.setPen(QPen(QColor("black"), 0, Qt::NoPen));
            painter.drawPolygon(calculator->getChainPolygon(usedProfile));
        }
        QPolygon outline = calculator->getDemandPolygon();
        painter.setPen(QPen(QColor("black"), 0, Qt::SolidLine));
        painter.setBrush(Qt::NoBrush);
        painter.drawPolygon(outline);
    }
}

void ResourceWidget::paintChainMatrix(QPainter &painter){
    Instance* instance = controller->getInstance();
    if(instance->getMaxFrameNr() == -1) return;
    ChainFrame* fr = (ChainFrame*) instance->getFrame(controller->getFrameNumber());

    int rows = calculator->getRowCount();
    painter.setPen(QPen(QColor("black"), 0, Qt::DotLine));
    for(int i=1;i<rows-1;i++){
        painter.drawLine(QPoint(0,i),QPoint(size().width()/hZoom()-offset,i));
    }

    QLinearGradient grad(0, 0, 0, rows);
    if(fr->getChain() && _resource->id() == fr->getChain()->resourceId()){
        grad.setColorAt(0, QColor(50,50,50));
        grad.setColorAt(_resource->capacity() / (double) rows, QColor(200,200,200));
    }
    else{
        grad.setColorAt(0, Qt::green);
        grad.setColorAt(_resource->capacity() / (double) rows, Qt::yellow);
    }
    painter.setBrush(grad);
    painter.setPen(QPen(QColor("black"), 0, Qt::SolidLine));

    foreach(Chain* ch, _resource->getChains().values()){
        if(fr->getChain() && ch->chainId() == fr->getChain()->chainId()
                && _resource->id() == fr->getChain()->resourceId()){
            grad.setColorAt(0, QColor(50,50,255));
            grad.setColorAt(_resource->capacity() / (double) rows, QColor(200,200,255));
            painter.setBrush(grad);
        }
        if(ch->chainId()+1 > _resource->capacity()){
            painter.setBrush(Qt::red);
        }
        QVector<QPolygon> activities = calculator->calculateChainMatrixPolygons(ch);
        foreach(QPolygon p, activities){
            painter.drawPolygon(p);
        }
        if(fr->getChain() && ch->chainId() == fr->getChain()->chainId()){
            grad.setColorAt(0, Qt::green);
            grad.setColorAt(_resource->capacity() / (double) rows, Qt::yellow);
            painter.setBrush(grad);
        }
    }
}

void ResourceWidget::setMatrixViewEnabled(bool toggled){
    matrixViewEnabled = toggled;
    calculateResourceProfile();
}

bool ResourceWidget::viewingResourceMatrix(){
    Instance* instance = controller->getInstance();
    if(instance->getMaxFrameNr() == -1) return false;
    ChainFrame* chfr = (ChainFrame*) instance->getFrame(controller->getFrameNumber());
    bool isChainOverview = false;
    if(controller->getFrameNumber()+1 <= instance->getMaxFrameNr())
        isChainOverview = instance->getFrame(controller->getFrameNumber()+1)->getChain();
    bool isAtLastFrame = controller->getFrameNumber() == instance->getMaxFrameNr();
    bool frameEnabled = controller->getParent()->getFramesSlider()->isEnabled();
    bool containsChainFrame = false;
    for(int i = 0; i < instance->getMaxFrameNr(); i++) {
        if( (ChainFrame*) instance->getFrame(i)->getChain() != NULL  ) {
            containsChainFrame = true;
            break;
        }
    }
    return (matrixViewEnabled && frameEnabled && (chfr->getChain() || isChainOverview || (isAtLastFrame && containsChainFrame)));
}

void ResourceWidget::paintEvent(QPaintEvent *e) {
    QWidget::paintEvent(e);
    QPainter painter(this);
    painter.drawPixmap(e->rect(), pixmap, e->rect());
}

void ResourceWidget::removeShade(){
    pattern = QRect(-1,-1,-1,-1);
    selected = QPoint(-1,-1);
    calculateResourceProfile();
}

void ResourceWidget::setShadedJob(Job *j){
    calculator->setJob(j);
    removeShade();
}

Resource* ResourceWidget::getResource(){
    return _resource;
}

void ResourceWidget::mousePressEvent(QMouseEvent *event){
    controller->shadeResources(0); //dont show resource profile

    int x = event->x()/hZoom();
    int y = event->y();

    QSize hint = size();
    int rows = calculator->getRowCount();
    int row = ceil((double)(hint.height()-y)/hint.height()*rows);
    selected = QPoint(x-offset, row);
    calculateResourceProfile();
}

void ResourceWidget::setHZoom(int z) {
    _hZoom = z;
    calculateResourceProfile();
}
void ResourceWidget::setVZoom(int z) {
    _vZoom = z;
    calculateResourceProfile();
}

int ResourceWidget::hZoom() const {
    return _hZoom;
}

int ResourceWidget::vZoom() const {
    return _vZoom;
}

void ResourceWidget::paintPeak(int t) {
    peak = t;
    update();
}

void ResourceWidget::removePeak() {
    peak = -1;
    update();
}

void ResourceWidget::paintPeak(QPainter& painter) {
    if(peak >= 0) {
        painter.setPen(QPen(QColor("red"), 0, Qt::SolidLine));
        painter.drawLine(peak*hZoom(), 0, peak*hZoom(), height());
        painter.drawLine(peak*hZoom()+1, 0, peak*hZoom()+1, height());
        painter.drawLine(peak*hZoom()-1, 0, peak*hZoom()-1, height());
    }
}
