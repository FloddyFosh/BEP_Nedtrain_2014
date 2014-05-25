#include "chainframe.h"

ChainFrame::ChainFrame(Chain *c) : Frame(), chain(c) {
    initialize();
}

void ChainFrame::initialize(){
    selectedChainProfile.append(new QPoint(0,0));
    foreach(Activity* act, chain){
        Requirement* req = act->getRequirements(chain->_resourceId);
        int amount = req->amount(); //moet nog amount van used bij
        selectedChainProfile.append(new QPoint(act->est(),0));
        selectedChainProfile.append(new QPoint(act->est(),amount));
        selectedChainProfile.append(new QPoint(act->eet(),amount));
        selectedChainProfile.append(new QPoint(act->eet(),0));
    }
}

Chain* ChainFrame::getChain(){
    return chain;
}

QList<QPoint*> ChainFrame::getUsedProfile(){
    return usedProfile;
}

QList<QPoint*> ChainFrame::getSelectedChainProfile(){
    return selectedChainProfile;
}

void ChainFrame::setUsedProfile(QList<QPoint*> p){
    usedProfile = p;
}

/*void ChainFrame::addToSelectedChainProfile(QPoint* p){
    selectedChainProfile.append(p);
}*/

void ChainFrame::display(Instance* inst) {
    Frame::display(inst);

}
