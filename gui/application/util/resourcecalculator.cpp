#include "resourcecalculator.h"

ResourceCalculator::ResourceCalculator(Resource* r) : _resource(r) {
	maxEet=0;
	maxDemand=0;
	shadedJob=0;
}

void ResourceCalculator::setJob(Job* j) {
	shadedJob = j;
}

void ResourceCalculator::calculate() {
	calcDemandProfile();
	calcResourceProfile();
	calcJobProfile();
}

bool sortbyXCoordinate(QPoint &p1, QPoint &p2) {
    return p1.x() < p2.x();
}

QList<QPoint> ResourceCalculator::getDemandProfile() const {
	return demandProfile;
}

QList<QPoint> ResourceCalculator::getResourceProfile() const {
	return resourceProfile;
}

QList<QPoint> ResourceCalculator::getJobProfile() const {
	return jobProfile;
}

QPolygon ResourceCalculator::getDemandPolygon() {
	QPolygon polygon;
	if (!demandProfile.isEmpty()) {
		for (int i = 0; i < demandProfile.size(); ++i) {
			polygon << QPoint(demandProfile[i].x(), i == 0 ? 0 : demandProfile[i - 1].y())
					<< demandProfile[i];
		}
		// finish
		polygon << polygon.first();
	}
	return polygon;
}
QPolygon ResourceCalculator::getResourcePolygon(int width) {
	QPolygon polygon;

	//add start point
	polygon << QPoint(0,getRowCount());

	for (int i = 0; i < resourceProfile.size(); ++i) {
		polygon << QPoint(resourceProfile[i].x(), i == 0 ? _resource->capacity() : resourceProfile[i - 1].y())
				<< resourceProfile[i];
	}
	// draw last part
	polygon << QPoint(width, _resource->capacity());

	//finish
	polygon << QPoint(width, getRowCount()) << polygon.first();
	return polygon;
}

QPolygon ResourceCalculator::getResourcePolyline(int width) {
	QPolygon polyline;
	for (int i = 0; i < resourceProfile.size(); ++i) {
		polyline << QPoint(resourceProfile[i].x(), i == 0 ? _resource->capacity() : resourceProfile[i - 1].y())
				<< resourceProfile[i];
	}
	// draw last part
	polyline << QPoint(width, _resource->capacity());
	return polyline;
}

QPolygon ResourceCalculator::getJobPolygon() {
	QPolygon polygon;
	if(!jobProfile.isEmpty()){
		for (int i = 0; i < jobProfile.size(); ++i) {
			polygon << QPoint(jobProfile[i].x(), i == 0 ? 0 : jobProfile[i - 1].y())
					<< jobProfile[i];
		}
		// finish
		polygon << polygon.first();
	}
	return polygon;
}

QPolygon ResourceCalculator::getExceedPolygon(int width) {
	QPolygon demandPolygon = getDemandPolygon();

	QPolygon resourcePolygon = getResourcePolygon(width);
	QPolygon exceedPolygon = demandPolygon.intersected(resourcePolygon);
	return exceedPolygon;
}

int ResourceCalculator::getRowCount() {
	return max(maxDemand, _resource->capacity()) + 2;
}

QPoint ResourceCalculator::getResourceUsageAt(int time) {
	int usage = 0;
	for(int i = 0; i < resourceProfile.size(); ++i) {
		if(resourceProfile[i].x()>time)
			break;
		else
			usage = resourceProfile[i].y();
	}
	return QPoint(time,usage);
}

QPoint ResourceCalculator::getResourceDemandAt(int time) {
    int usage = 0;
    for(int i = 0; i < demandProfile.size(); ++i) {
        if(demandProfile[i].x()>time)
            break;
        else
            usage = demandProfile[i].y();
    }
    return QPoint(time,usage);
}

QMap<int, int> ResourceCalculator::demandProfileData() {
    QMap<int, int> demandChanges;
    foreach(Requirement * q, _resource->getRequirements()) {
        if (q->amount() != 0) {
            demandChanges[q->activity()->st()] += q->amount();
            demandChanges[q->activity()->eet()+q->activity()->st()-q->activity()->est()] -= q->amount();
        }
    }
    return demandChanges;
}

void ResourceCalculator::calcDemandProfile() {
	demandProfile.clear();
	QMap<int, int> demandChanges = demandProfileData();

	// create QPoints for demandProfile
	demandProfile.clear();
	for (QMap<int, int>::const_iterator it = demandChanges.begin(); it != demandChanges.end(); ++it) {
		demandProfile.append(QPoint(it.key(), it.value()));
	}

	// sort demandProfile by x coordinate
	qSort(demandProfile.begin(), demandProfile.end(), sortbyXCoordinate);

	// convert QPoint y values from deltas to absolute values (and update max. demand)
	maxDemand = demandProfile.size() > 0 ? demandProfile[0].y() : 0;
	for (int i = 1; i < demandProfile.size(); ++i) {
		demandProfile[i].setY(demandProfile[i - 1].y() + demandProfile[i].y());
		maxDemand = max(maxDemand, demandProfile[i].y());
	}

	this->demandProfile = demandProfile;
}

QMap<int, int> ResourceCalculator::resourceProfileData() {
    QMap<int, int> resourceChanges;
    foreach(ResourceDecrease * d, _resource->getDecreases()) {
        if (d->getAmount() != 0) {
            resourceChanges[d->getFrom()] -= d->getAmount();
            resourceChanges[d->getTill()] += d->getAmount();
        }
    }
    return resourceChanges;
}

void ResourceCalculator::calcResourceProfile() {
	QList<QPoint> resourceProfile;
	QMap<int, int> resourceChanges = resourceProfileData();

	// create QPoints for resourceProfile
	resourceProfile.clear();
	for (QMap<int, int>::const_iterator it = resourceChanges.begin(); it != resourceChanges.end(); ++it) {
		resourceProfile.append(QPoint(it.key(), it.value()));
	}
	resourceProfile.append(QPoint(0, _resource->capacity()));

	// sort resourceProfile by x coordinate
	qSort(resourceProfile.begin(), resourceProfile.end(), sortbyXCoordinate);

	// convert QPoint y values from deltas to absolute values
	for (int i = 1; i < resourceProfile.size(); ++i)
		resourceProfile[i].setY(resourceProfile[i - 1].y() + resourceProfile[i].y());

	this->resourceProfile = resourceProfile;
}

QMap<int, int> ResourceCalculator::jobProfileData() {
    QMap<int, int> jobChanges;
    foreach(Activity* act, shadedJob->getActivities()) {
        foreach(Requirement * q, act->getRequirements()) {
            if (q->resource() == _resource && q->amount() != 0) {
                jobChanges[act->st()] += q->amount();
                jobChanges[act->eet()+act->st()-act->est()] -= q->amount();
            }
        }
    }
    return jobChanges;
}

void ResourceCalculator::calcJobProfile() {
	QList<QPoint> jobProfile;

	if(shadedJob){
		// collect data for shade profile
		QMap<int, int> jobChanges = jobProfileData();

		// create QPoints for jobProfile
		jobProfile.clear();
		for (QMap<int, int>::const_iterator it = jobChanges.begin(); it != jobChanges.end(); ++it) {
			jobProfile.append(QPoint(it.key(), it.value()));
		}
		// sort job Profile by x coordinate
		qSort(jobProfile.begin(), jobProfile.end(), sortbyXCoordinate);

		// convert QPoint y values from deltas to absolute values
		for (int i = 1; i < jobProfile.size(); ++i) {
			jobProfile[i].setY(jobProfile[i - 1].y() + jobProfile[i].y());
		}
	}
	this->jobProfile = jobProfile;
}

QRect ResourceCalculator::selectedResourceRegion(QPoint selected) {
	int start = -1;
	int end   = -1;
	for(int i=0; i<demandProfile.size(); i++){
		if(demandProfile[i].x()<=selected.x() && demandProfile[i].y()<selected.y())
			start = -1;
		else if(demandProfile[i].x()<=selected.x() && start==-1 && demandProfile[i].y()>=selected.y())
			start=demandProfile[i].x();
		else if(demandProfile[i].x()>=selected.x() && demandProfile[i].y()<selected.y()) {
			end=demandProfile[i].x();
			break;
		}
	}

	QRect selectedRect;
	if(start!=-1 && end !=-1)
		selectedRect = QRect(QPoint(start, selected.y()), QPoint(end-1,selected.y()-1));
	else
		selectedRect = QRect(-1,-1,-1,-1);

	return selectedRect;
}

int ResourceCalculator::getMaximumEet() {
	return maxEet = demandProfile.isEmpty() ? 0 : demandProfile.last().x();
}

int ResourceCalculator::getMaximumDemand() const {
	return maxDemand;
}
