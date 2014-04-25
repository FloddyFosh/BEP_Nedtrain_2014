#ifndef ST_PROPAGATOR_H
#define ST_PROPAGATOR_H

class InstanceController;
class Group;

using namespace std;
#include <map>
#include <vector>
#include <set>

typedef pair<int, Group *> qq_t;

/**
 * TODO comments
 */
class ST_Propagator
{
private:
    set<qq_t, less<qq_t> > se;
    map<Group *, int> st;
public:
    /**
     * Constructor.
     * @param controller the controller of the instance
     * @param group the group which the user tries to drag
     */
    ST_Propagator (InstanceController * controller, Group * group);
    void moveIfPossible(int dt);
    void changeDurationIfPossible(int newDuration);
private:
    bool propagate(int fakeDuration, bool left);
    bool relax(Group *x, Group *y, int fakeDuration, bool left);
    void applyChanges(int newDuration = -1);

    InstanceController * controller;///< the controller of the instance which is managed
    Group * group;
    vector<Group *> allGroups;
    map<Group *, int> savedST;
    map<Group *, vector<Group *> > adj, radj;
};
#endif // ST_PROPAGATOR_H
