#include "est_generator.h"
#include "controller/instancecontroller.h"
#include "frame.h"

EST_Generator::EST_Generator(InstanceController * controller) : controller (controller) { }

void EST_Generator::gotoFrame(int frameNr) {
	Instance *instance = controller->getInstance();

    if (!(0 <= frameNr && frameNr <= instance->getMaxFrameNr())) { return; }
    Frame *frame = instance->getFrame(frameNr);
    frame->display(instance);
}

QList<Precedence *> EST_Generator::getAdded(int frameNr){
	Instance *instance = controller->getInstance();
    QList<Precedence *> constraints;
    foreach(Precedence * p, instance->getSoftPrecedences()) {
        if (!p->isHard() && p->getFrameNrs().count(frameNr)) constraints.append(p);
    }
    foreach(Precedence * p, instance->getHardPrecedences()) {
        if (p->isHard() && p->getFrameNrs().count(frameNr)) constraints.append(p);
    }
    return constraints;
}

