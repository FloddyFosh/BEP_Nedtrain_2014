#include "chainframe.h"

ChainFrame::ChainFrame(Chain* c, QList<QPoint*>* prev) : Frame(), chain(c), usedProfile(prev), isChain(true) {
    initialize();
}

void ChainFrame::initialize(){
    selectedProfile = new QList<QPoint*>;
    selectedProfile->append(new QPoint(0,0));
    QVector<Activity*>* activities = chain->getActivities();
    foreach(Activity* act, *activities){
        selectedProfile->append(new QPoint(act->est(),0));
        selectedProfile->append(new QPoint(act->est(),1));
        selectedProfile->append(new QPoint(act->eet(),1));
        selectedProfile->append(new QPoint(act->eet(),0));
    }
    QList<QPoint*>* newPoints = new QList<QPoint*>;
    int i=0;
    foreach(QPoint* np, *selectedProfile){
        if(np->y() == 0) continue;
        if(i >= usedProfile->size()) return;
        QPoint* op = usedProfile->at(i);
        while(op->x() < np->x()){
            newPoints->append(new QPoint(op->x(),op->y()+1));
            if(i == usedProfile->size()-1) break;
            op = usedProfile->at(++i);
        }
        if(op->x() == np->x()){
            newPoints->append(new QPoint(op->x(),op->y()+1));
            i++;
        }
        else if(op->x() > np->x()){
            newPoints->append(new QPoint(np->x(),usedProfile->at(i-1)->y()+1));
            newPoints->append(new QPoint(np->x(),usedProfile->at(i-1)->y()));
        }
    }
    selectedProfile->append(*newPoints);
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
