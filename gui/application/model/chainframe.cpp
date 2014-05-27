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
    QList<QPoint*>* newProfile = addProfiles(selectedProfile,usedProfile);
    selectedProfile = newProfile;
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

QList<QPoint*>* ChainFrame::addProfiles(QList<QPoint*>* pl1, QList<QPoint*>* pl2) {
    if(pl1->empty()) return pl2;
    if(pl2->empty()) return pl1;
    QList<QPoint*>* res = new QList<QPoint*>;

    int i = 0, j = 0;
    while(i < pl1->size() && j < pl2->size()) {
        QPoint* p1 = pl1->at(i);
        QPoint* p2 = pl2->at(j);
        if(p1->x() < p2->x()) {
            res->append(new QPoint(p1->x(), p1->y()+p2->y()));
            i++;
        }
        else if(p1->x() > p2->x()) {
            res->append(new QPoint(p2->x(), p1->y()+p2->y()));
            j++;
        }
        else {
            res->append(new QPoint(p1->x(),p1->y()+p2->y()));
            i++;
            j++;
        }
    }
    while(i < pl1->size()) {
        res->append(new QPoint(pl1->at(i)->x(), pl1->at(i)->y()+pl2->last()->y()));
        i++;
    }
    while(j < pl2->size()) {
        res->append(new QPoint(pl2->at(j)->x(), pl1->last()->y()+pl2->at(j)->y()));
        j++;
    }
    return res;
}

