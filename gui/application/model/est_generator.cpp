#include "est_generator.h"

#include "controller/instancecontroller.h"
#include "frame.h"
#include "chain.h"

EST_Generator::EST_Generator(InstanceController * controller) : controller (controller) { }

void EST_Generator::gotoFrame(int frameNr) {
	Instance *instance = controller->getInstance();

    if (!(0 <= frameNr && frameNr <= instance->getMaxFrameNr())) { return; }
    Frame *frame = instance->getFrame(frameNr);
    instance->setGroupPartition(frame->getGroups());
    foreach(int i, instance->getResources().keys()) {
        controller->highlightResource(i, false);
    }
    foreach(int i, instance->getJobs().keys()) {
        controller->highlightJob(i, false);
    }

    if(Chain* c = frame->getChain()){
        controller->focusResource(c->resourceId());
        controller->highlightResource(c->resourceId(), true);
    }
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

