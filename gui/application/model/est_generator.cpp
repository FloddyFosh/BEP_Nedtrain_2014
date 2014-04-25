#include "est_generator.h"
#include "controller/instancecontroller.h"
#include "frame.h"

EST_Generator::EST_Generator(InstanceController * controller) : controller (controller) { }

void EST_Generator::gotoFrame(int frameNr) {
	Instance *instance = controller->getInstance();

    if (!(0 <= frameNr && frameNr <= instance->getMaxFrameNr())) { return; }
    Frame *frame = instance->getFrame(frameNr);

    instance->setGroupPartition(frame->getGroups());
}

QList<Precedence *> EST_Generator::getAdded(int frameNr){
	Instance *instance = controller->getInstance();
    QList<Precedence *> constraints;
    foreach(Precedence * p, instance->getSoftPrecedences()) {
        if (!p->isHard() and p->getFrameNr() == frameNr) constraints.append(p);
    }
    return constraints;
}

