#ifndef RESOURCECALCULATOR_H_
#define RESOURCECALCULATOR_H_

#include <QList>
#include <QMap>
#include <QPoint>
#include <QPolygon>
#include <QtGlobal>

class Activity;
class Chain;
class Job;
class Resource;

class ResourceCalculator {

private:
	Resource* _resource;
	QList<QPoint> demandProfile, resourceProfile, jobProfile, dots;
	int maxEet, maxDemand;
	Job* shadedJob;

	/**
	 * Calculates a list of points that represents the usage of the resource
	 */
	void calcDemandProfile();

	/**
	 * Calculates a list of points that represents the capacity of the resource
	 */
	void calcResourceProfile();

	/**
	 * Calculates a list of points that indicate the usage of the resource by a particular job
	 */
	void calcJobProfile();

public:
	ResourceCalculator(Resource *);
	void calculate(); ///< calculates the demand profile, the resource profile and the job profile.
	void setJob(Job*); ///< sets the job to be considered for calculating the job profile.

	QMap<int, int> demandProfileData(); ///< returns a map with collected demand profile data.
	QMap<int, int> resourceProfileData(); ///< returns a map with collected resource profile data.
	QMap<int, int> jobProfileData(); ///< returns a map with collected job profile data.

	QList<QPoint> getDemandProfile() const; ///< @return The demand profile which is calculated in calcDemandProfile().
	QList<QPoint> getResourceProfile() const; ///< @return The resource profile which is calculated in calcResourceProfile().
	QList<QPoint> getJobProfile() const;  ///< @return The job profile which is calculated in calcJobProfile().
	QPolygon getDemandPolygon(); ///< @return A polygon for the demand profile. The demand profile calculation should have been done.
	QPolygon getResourcePolygon(int width); ///< @return A polygon for the resource profile, starting at point (0,0). The resource profile calculation should have been done.
	QPolygon getResourcePolyline(int width); ///< @return A polygon for the resource profile starting at the height of the resource capacity. The resource profile calculation should have been done.
	QPolygon getJobPolygon(); ///< @return A polygon for the job profile. The job profile calculation should have been done.
	QPolygon getExceedPolygon(int width); ///< @return A polygon for the regions where the demand profile exceeds the resource profile
    QPolygon getChainPolygon(QList<QPoint*>* points);
    QVector<QPolygon> calculateChainMatrixPolygons(Chain* ch);
    Activity* selectedActivity(QPoint selected);

	QRect selectedResourceRegion(QPoint p); ///< @return A horizontal consecutive region enclosing point p.
	QPoint getResourceUsageAt(int t); ///< @return The resource usage at time t.
    QPoint getResourceDemandAt(int t); ///< @return The resource demand at time t.
	int getMaximumEet(); ///< @return the earliest end time of the last activity
	int getMaximumDemand() const; ///< @return The maximum resource usage
	int getRowCount(); ///< @return The number of rows of this resource (used to paint the resource widget).
};

#endif /* RESOURCECALCULATOR_H_ */
