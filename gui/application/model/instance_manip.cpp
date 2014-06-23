#include "model/instance.h"

#include "controller/exceptions.h"

#include <cassert>
#include <climits>
#include <algorithm>
#include <set>
#include <stdio.h>

Resource* Instance::addResource(unsigned int i, unsigned int capacity, QString name) {
    int resId = i;
    Resource *existingResource = containsResource(name);

    if (R(resId) && !importing) {
        throw InstanceManipulationException(QString("Resource with id %1 ignored because a resource with that id already exists.").arg(resId));
    }
    else if (existingResource && importing) {
        if (!importResourceIndices.contains(i)) importResourceIndices.insert(i, existingResource->id());
        return existingResource;
    }
    else if (R(resId) && importing) {
        resId = next_resource_index;
        next_resource_index++;
        importResourceIndices.insert(i, resId);
    }

    next_resource_index = (next_resource_index <= i && !importing) ? i+1 : next_resource_index;
    Resource *r = new Resource(this, resId, name, capacity);
    resources[resId] = r;
    emit resourceAdded(r);
    return r;
}

void Instance::addResource(unsigned int capacity, QString name) {
    assert(!R(next_resource_index));

    Resource *r = new Resource(this, next_resource_index, name, capacity);
    resources[next_resource_index] = r;
    next_resource_index++;
    emit resourceAdded(r);
}

void Instance::removeResource(unsigned int i) {
    setUserChanges(true);
    resources.remove(i);
}

Resource* Instance::containsResource(QString name) {
    foreach(Resource * r, resources) {
        if (r->name() == name)
            return r;
    }
    return 0;
}

void Instance::addDummyJob(unsigned int i, unsigned int release_date, unsigned int due_date) {
    int jobId = checkNewJobId(i);
	ResourceDecrease * dec = new ResourceDecrease(release_date+importOffset, due_date+importOffset, 0, jobId);
	decreases[jobId] = dec;
}

ResourceDecrease* Instance::addDummyJob(Resource* r, unsigned int release_date, unsigned int due_date, int cap) {
	assert(!J(next_job_index));

	ResourceDecrease * dec = new ResourceDecrease(release_date+importOffset, due_date+importOffset, cap, next_job_index);
	dec->setResource(r);
	r->addDecrease(dec);
	next_job_index++;
	return dec;
}

int Instance::checkNewJobId(unsigned int i) {
    int jobId = i;
    if (J(jobId) && !importing) {
        throw InstanceManipulationException(QString("Job with id %1 ignored because a job with that id already exists.").arg(jobId));
    }
    else if(J(jobId) && importing) {
        jobId = next_job_index;
        next_job_index++;
        importJobIndices.insert(i, jobId);
    }
    if(next_job_index <= i && !importing) {
        next_job_index = i+1;
    }
    return jobId;
}

Job* Instance::addJob(unsigned int i, unsigned int release_date, unsigned int due_date, QString name) {
    int jobId = checkNewJobId(i);

    Job *job = new Job(this, jobId, name, release_date+importOffset, due_date+importOffset);
    jobs[jobId] = job;

    emit jobAdded(job);
    return job;
}

Job* Instance::addJob(unsigned int release_date, unsigned int due_date, QString name) {
    assert(!J(next_job_index));

    Job *job = new Job(this, next_job_index, name, release_date+importOffset, due_date+importOffset);
    jobs[next_job_index] = job;
    next_job_index++;

    emit jobAdded(job);
    return job;
}

void Instance::removeJob(Job *j) {
    setUserChanges(true);
    QMapIterator<int, Activity*> i(j->getActivities());
    while(i.hasNext()) {
        Activity *a = i.next().value();
        removeActivity(a);
    }
    jobs.remove(j->id());
    delete j;
}

void Instance::addActivity(unsigned int i, unsigned int j, int est, int lst, unsigned int duration, QString name, int st) {
    int jobId = importJobIndices.contains(i) && importing ? importJobIndices.value(i) : i;
    if (!jobs.contains(jobId)){
    	if(!decreases.contains(jobId))
            throw InstanceManipulationException(QString("Activity %1 for job %2 ignored because job %2 does not exist.").arg(j).arg(jobId));
    }
    else{
    	if (A(jobId, j))
			throw InstanceManipulationException(QString("Activity %1 for job %2 ignored because an activity with that id already exists.").arg(j).arg(jobId));
	    bool locked = false;
		if (est == -1) {
		    est = J(jobId)->releaseDate();
		    lst = J(jobId)->dueDate() - duration;
		}
        else locked = true;
		if ( st == -1) st = est;
		Activity * a (new Activity(J(jobId), j, name, duration));
		addActivity(a, est, lst, st, locked);
    }
}

void Instance::addActivity(Activity * a, int est, int lst, int st, bool locked) {
    int jobId = a->job()->id();
	Group * g (new Group (jobId, est, lst, st));
	g->addActivity(a);
	g->setLocked(locked);
	J(jobId)->addActivity(a);
	createGroup(g);
}

void Instance::addRequirement(unsigned int i, unsigned int j, unsigned int k, unsigned int q) {
    int jobId = importJobIndices.contains(i) && importing ? importJobIndices.value(i) : i;
    int resourceId = importResourceIndices.contains(k) && importing ? importResourceIndices.value(k) : k;

    if(q == 0) // don't accept a requirement of 0 (the solver will fail)
        return;
    if (!jobs.contains(jobId)){
    	if (decreases.contains(jobId)){
			decreases[jobId]->setAmount(q);
			decreases[jobId]->setResource(R(k));
			R(k)->addDecrease(decreases[jobId]);
			decreases.remove(jobId);
			return;
		}
    	else
    		throw InstanceManipulationException(QString("Requirement of a(%1,%2) for resource %3 ignored because job %1 does not exist.").arg(jobId).arg(j).arg(resourceId));
    }
    if (!A(jobId, j))
        throw InstanceManipulationException(QString("Requirement of a(%1,%2) for resource %3 ignored because activity %2 does not exist.").arg(jobId).arg(j).arg(resourceId));
    if (!R(resourceId))
        throw InstanceManipulationException(QString("Requirement of a(%1,%2) for resource %3 ignored because resource %3 does not exist.").arg(jobId).arg(j).arg(resourceId));

    A(jobId, j)->setRequiredAmount(R(resourceId), q);
}

void Instance::addDecreasePrecedence(unsigned int i1, unsigned int i2, unsigned int j2){
	ResourceDecrease* dec = 0;
	foreach(Resource* r, resources){
		if(r->getDecrease(i1)){
			dec = r->getDecrease(i1);
			break;
		}
	}
	if(!dec)
		throw InstanceManipulationException(QString("Decrease precedence with decrease id %1 ignored because no decrease with id %1 exists").arg(i1));
	if (!A(i2, j2))
	    throw InstanceManipulationException(QString("Decrease Precedence %1 before a(%2,%3) ignored because activity %3 does not exist.").arg(i1).arg(i2).arg(j2));
	A(i2, j2)->addDecreasePrecedence(dec);
}

void Instance::removeDecreasePrecedence(ResourceDecrease *dec){
	foreach(Job* j, jobs){
		foreach(Activity* a, j->getActivities())
			a->removeDecrease(dec);
	}
}

void Instance::setFlex(unsigned int i, unsigned int j, unsigned int flex) {
    A(i,j)->setFlex(flex);
}

void Instance::addPrecedence(unsigned int i1, unsigned int j1, unsigned int i2, unsigned int j2, bool hard, int frameNummer) {
    int jobId1 = importJobIndices.contains(i1) && importing ? importJobIndices.value(i1) : i1;
    int jobId2 = importJobIndices.contains(i2) && importing ? importJobIndices.value(i2) : i2;

    if (!J(jobId1)){
    	addDecreasePrecedence(i1, i2, j2);
    	return;
    }
    if (!A(jobId1, j1))
        throw InstanceManipulationException(QString("Precedence a(%1,%2) before a(%3,%4) ignored because activity %2 does not exist.").arg(jobId1).arg(j1).arg(jobId2).arg(j2));
    if (!J(jobId2)) {
            throw InstanceManipulationException(QString("Precedence a(%1,%2) before a(%3,%4) ignored because job %3 does not exist.").arg(jobId1).arg(j1).arg(jobId2).arg(j2));
    }
    if (!A(jobId2, j2))
        throw InstanceManipulationException(QString("Precedence a(%1,%2) before a(%3,%4) ignored because activity %4 does not exist.").arg(jobId1).arg(j1).arg(jobId2).arg(j2));
    addPrecedence(A(jobId1, j1), A(jobId2, j2), hard, frameNummer);

}

bool Instance::impliedPrecedenceExists(Activity * src, Activity * dst) {
    // depth first search
    QSet<Activity *> reachable;
    QVector<Activity *> queue;
    reachable.insert(src);
    queue.push_back(src);
    while (!queue.empty()) {
        Activity * a1 = queue.back(); queue.pop_back();
        foreach(Precedence * p, a1->getOutgoingPrecedences()) {
            Activity * a2 = p->a2();
            if (reachable.find(a2) == reachable.end()) {
                reachable.insert(a2);
                queue.push_back(a2);
            }
        }
    }
    return reachable.find(dst) != reachable.end();
}

void Instance::addPrecedenceFrame(Activity* a1, Activity* a2, int frameNumber) {
    if(Precedence *existing = precedenceExists(a1,a2)) {
        existing->addFrameNr(frameNumber);
    }
}

void Instance::addPrecedence(Activity * a1, Activity * a2, bool hard, int frameNumber) {
    if(impliedPrecedenceExists(a2, a1))
        throw InstanceManipulationException(tr("The constraint was not added, because it would create a cycle. A chain of constraints in the opposite direction already exists."));
    if (Precedence *existing = precedenceExists(a1, a2)) {
        if(hard) {
            if(!existing->isHard()) {
                // Remove soft precedence, so that a hard precedence is added.
                removePrecedence(existing);
            }
            else throw InstanceManipulationException(tr("Adding of the hard precedence constraint ignored, because it was already present."));
        }
        else {
            if(!existing->isHard() && (existing->isDisabled() || !existing->getFrameNrs().contains(frameNumber))){
                if(existing->isDisabled()){
                    existing->enable();
                }
                if(!existing->getFrameNrs().contains(frameNumber)) {
                    // Soft precedences can be added to multiple frames, because of multiple algorithms.
                    existing->addFrameNr(frameNumber);
                    return;
                }
            }
            else throw InstanceManipulationException(tr("Adding of precedence constraint ignored, because it was already present."));
        }
    }

    Precedence * p (new Precedence(a1, a2, hard));
    if(hard) {
        precedences.push_back(p);
    } else {
        softPrecedences.push_back(p);
        p->addFrameNr(frameNumber);
    }
    a1->addPrecedence(p);
    a2->addPrecedence(p);
}

void Instance::mergeGroup(unsigned int i1, unsigned int j1, unsigned int i2, unsigned int j2) {
    Group * g1 (groupMap[A(i1, j1)]);
    Group * g2 (groupMap[A(i2, j2)]);
    assert(g1 and g2);
    QList<Activity *> acts (g2->getActivities());
    removeGroup(g2);
    foreach(Activity * a, acts) {
        groupMap[a] = g1;
        g1->addActivity(a);
    }
    foreach(Activity * a, acts) a->setChanged();
}

void Instance::createGroup(Group * g) {
    foreach(Activity * a, g->getActivities()) {
        groupMap[a] = g;
    }
    
    J(g->getJob())->addGroup(g);
    groups.append(g);
    foreach(Activity * a, g->getActivities()) {
        a->setGroup(g);
    }
}

void Instance::removeGroup(Group * g) {
    foreach(Activity *a, g->getActivities()) {
        groupMap.remove(a);
    }

    J(g->getJob())->removeGroup(g);
    groups.removeOne(g);

    /*
     * don't deallocate memory here, because groups are removed when
     * going to another frame, and you may want to go back...
     */
}

Group* Instance::activityGroup(Activity *a) {
    return groupMap.value(a);
}

void Instance::removeActivityFromGroup(Activity * a) {
    if(groupMap.contains(a)) {
        Group * g = groupMap.value(a);
        g->removeActivity(a);
        if (g->size() == 0) removeGroup(g);
    }
}

void Instance::addJobMutexes() {
    // for every job...
    foreach(Job * j, jobs) {
        // add dummy resource with capacity 1 and id i
        int i = -1;
        while (R(++i)) {
        } // find free id i
        try {
            addResource(i, 1, j->name());
        } catch(InstanceManipulationException const& e) { e.printWarning(); }

        // add requirement to every activity
        foreach(Activity * a, j->getActivities()) {
            try {
                addRequirement(j->id(), a->id(), i, 1);
            } catch(InstanceManipulationException const& e) { e.printWarning(); }
        }
    }
}

void Instance::clearSoftPrecedences() {
    setUserChanges(true);
    foreach(Precedence *p, softPrecedences){
         p->disable();
    }

    foreach(Job* j, jobs){
    	foreach(Activity* a, j->getActivities())
    		a->clearDecreases();
    }
    
    QList<Group *> copyOfGroups (groups);
    foreach(Group * g, copyOfGroups) {
        QList<Activity *> acts = g->getActivities();
        int j = g->getJob(), est = g->getEST(), lst = g->getLST(), lct = lst + g->getDuration(), st = g->getST();
        removeGroup(g);
        if (g->isLocked()) {
            Group * g2 (new Group (j, est, lst, st));
            foreach(Activity * a, acts) g2->addActivity(a);
            g2->setLocked(true);
            createGroup(g2);
        }
        else {
            foreach(Activity * a, acts) {
                Group * g2 (new Group (j, est, lct - a->duration(), st));
                g2->addActivity(a);
                createGroup(g2);
            }
        }
    }
}

bool Instance::hasSoftPrecedences() {
    foreach(Precedence *p, softPrecedences){
        if(!p->isDisabled()) return true;
    }
    return false;
}

void Instance::emitChanged() {
    emit instanceChanged();
}

void Instance::removePrecedence(Precedence * p) {
    Activity * a1 (p->a1()), * a2 (p->a2());
    if(p->isHard())
        precedences.erase( std::find(precedences.begin(), precedences.end(), p));
    else
        softPrecedences.erase( std::find(softPrecedences.begin(), softPrecedences.end(), p));
    a1->removePrecedence(p);
    a2->removePrecedence(p);
    delete p;
}

void Instance::removePrecedence(Activity *a1, Activity *a2) {
    if(Precedence *p = precedenceExists(a1, a2))
        removePrecedence(p);
}

void Instance::removePrecedences(Activity *a) {
    QVector<Activity *> pred, succ;
    foreach(Precedence * p,  a->getIncomingPrecedences()) {
        if(p->isHard())
            pred.push_back(p->a1());
        removePrecedence(p);
    }
    foreach(Precedence * p, a->getOutgoingPrecedences()) {
        if(p->isHard())
            succ.push_back(p->a2());
        removePrecedence(p);
    }

    foreach(Activity * a1, pred) {
        foreach(Activity * a2, succ)  {
            if (!precedenceExists(a1, a2)) {
                try {
                    addPrecedence(a1, a2, true);
                } catch(InstanceManipulationException const& e) { e.printWarning(); }
            }
        }
    }
}

void Instance::removeActivity(Activity *a) {
    setUserChanges(true);
    removePrecedences(a);
    a->removeRequiredAmounts();
    a->job()->removeActivity(a);
    removeActivityFromGroup(a);

    delete a;
}

Precedence* Instance::precedenceExists(Activity * a1, Activity * a2) {
    QListIterator<Precedence *> precedences(a1->getOutgoingPrecedences());
    while(precedences.hasNext()) {
        Precedence *p = precedences.next();
        if(p->a2() == a2)
            return p;
    }
    return 0;
}

void Instance::updateStartTime(){
    int start = INT_MAX;

    if(jobs.size() > 0){
        foreach(Job *j, jobs) start = qMin(start, j->releaseDate());
    }else{
        start = 0;
    }

    startTime = QTime(0,0,0).addSecs(start*60);
    timelineStartTime = startTime.addSecs((startTime.minute() % 15)*-60);
    timeOffset = timelineStartTime.secsTo(startTime)/60 - start;
}

QTime Instance::getStartTime() {
    return startTime;
}

QTime Instance::getTimeLineStartTime() {
    return timelineStartTime;
}

int Instance::getTimeOffset() {
	return hoursOnTimeline() ? timeOffset : 0;
}

bool Instance::hoursOnTimeline() {
    return useHoursOnTimeline;
}

void Instance::setHoursOnTimeline(bool b) {
    useHoursOnTimeline = b;
}

void Instance::emitSaved() {
    emit fileNameChanged(fileName);
    emit isModified(false);
}

void Instance::setImportFlag(bool b) {
    importing = b;
}

void Instance::setImportOffset(int o) {
    importOffset = o;
}

void Instance::clearImportMaps() {
    importJobIndices.clear();
    importResourceIndices.clear();
}

void Instance::setGroupPartition(QVector<Group *> new_groups) {
    QList<Group *> all_groups (groups);
    foreach(Group * g, all_groups) removeGroup(g);
    foreach(Group * g, new_groups) createGroup(g);
}
