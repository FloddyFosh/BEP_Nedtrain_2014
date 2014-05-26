#include "chainframe.h"

ChainFrame::ChainFrame(Chain* c, QList<QPoint*>* prev) : Frame(), chain(c), usedProfile(prev), isChain(true) {
    initialize();
}

void ChainFrame::initialize(){
    QList<QPoint*>* bla = new QList<QPoint*>;
    selectedProfile = bla;
    selectedProfile->append(new QPoint(0,0));
    QVector<Activity*>* activities = chain->getActivities();
    foreach(Activity* act, *activities){
        Requirement* req = act->getRequirements().value(chain->resourceId());
        int amount = req->amount(); //moet nog amount van used bij
        selectedProfile->append(new QPoint(act->est(),0));
        selectedProfile->append(new QPoint(act->est(),amount));
        selectedProfile->append(new QPoint(act->eet(),amount));
        selectedProfile->append(new QPoint(act->eet(),0));
    }
}

Chain* ChainFrame::getChain(){
    return chain;
}

QList<QPoint*>* ChainFrame::getSelectedProfile(){
    return selectedProfile;
}

QList<QPoint*>* ChainFrame::getUsedProfile(){
    return usedProfile;
}

void ChainFrame::setUsedProfile(QList<QPoint*>* p){
    usedProfile = p;
}

void ChainFrame::setSelectedProfile(QList<QPoint*>* p){
    selectedProfile = p;
}

void ChainFrame::display(Instance* inst) {
    Frame::display(inst);
}
