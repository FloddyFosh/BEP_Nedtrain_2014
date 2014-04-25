#include  <gtest/gtest.h>
#include "model/activity.h"
#include "model/job.h"
#include "controller/exceptions.h"

TEST(ActivityTest, setJob) {
	Instance *instance = new Instance();
	Job *job = new Job(instance, 1, "Project", 0, 1);

	Activity *activity = new Activity();
	activity->setJob(job);
	ASSERT_EQ(job, activity->job());
}

TEST(ActivityTest, setName) {
	Activity *activity = new Activity();
	activity->setName("Test");
	ASSERT_EQ("Test", activity->name());
}

TEST(ActivityTest, setEmptyName) {
	Activity *activity = new Activity();
	activity->setName("Test");
	activity->setName("");
	ASSERT_EQ("Test", activity->name());
}

TEST(ActivityTest, estimatedEndTime) {
	Activity *activity = new Activity();
	Group* group = new Group();
	group->addActivity(activity);
	group->setEST(5);
	activity->setDuration(10);
	ASSERT_EQ(15, activity->eet());
}

TEST(ActivityTest, NoOverlap1) {
	Activity *activity1 = new Activity();
    Group* group1 = new Group();
    group1->addActivity(activity1);
	group1->setEST(0);
	activity1->setDuration(5);

	Activity *activity2 = new Activity();
    Group* group2 = new Group();
    group2->addActivity(activity2);
	group2->setEST(5);
	activity2->setDuration(10);

	Interval interval = activity1->overlap(activity2);
	ASSERT_EQ(0, interval.start);
	ASSERT_EQ(0, interval.end);
}

TEST(ActivityTest, NoOverlap2) {
	Activity *activity1 = new Activity();
    Group* group1 = new Group();
    group1->addActivity(activity1);
	group1->setEST(0);
	activity1->setDuration(5);

	Activity *activity2 = new Activity();
    Group* group2 = new Group();
    group2->addActivity(activity2);
	group2->setEST(8);
	activity2->setDuration(10);

	Interval interval = activity1->overlap(activity2);
	ASSERT_EQ(0, interval.start);
	ASSERT_EQ(0, interval.end);
}

TEST(ActivityTest, Overlap) {
	Activity *activity1 = new Activity();
    Group* group1 = new Group();
    group1->addActivity(activity1);
	group1->setEST(0);
	activity1->setDuration(5);

	Activity *activity2 = new Activity();
    Group* group2 = new Group();
    group2->addActivity(activity2);
	group2->setEST(3);
	activity2->setDuration(10);

	Interval interval = activity1->overlap(activity2);
	ASSERT_EQ(3, interval.start);
	ASSERT_EQ(5, interval.end);
}

TEST(ActivityTest, Ordering) {
	Activity *activity1 = new Activity();
	Activity *activity2 = new Activity();
    Group* group1 = new Group();
    Group* group2 = new Group();
    group1->addActivity(activity1);
    group2->addActivity(activity2);

	group1->setEST(0);
	activity1->setDuration(6);
	group2->setEST(3);
	activity2->setDuration(5);

	ASSERT_TRUE(*activity1 < *activity2);
	ASSERT_FALSE(*activity2 < *activity1);

	group1->setEST(3);

	ASSERT_FALSE(*activity1 < *activity2);
	ASSERT_TRUE(*activity2 < *activity1);
}

TEST(ActivityTest, GetRequiredAmountTest) {
    Resource *res = new Resource("name");
    Activity *act = new Activity();
    act->setRequiredAmount(res, 1);
    ASSERT_EQ(1, act->getRequiredAmount(res));
}

TEST(ActivityTest, GetUnspecifiedAmountTest) {
    Resource *res = new Resource("name");
    Activity *act = new Activity();

    ASSERT_EQ(0, act->getRequiredAmount(res));
}

TEST(ActivityTest, SetRequiredAmountTest) {
    Resource *res = new Resource("name");
    Activity *act = new Activity();
    act->setRequiredAmount(res, 1);
    ASSERT_EQ(1, act->getRequiredAmount(res));
    act->setRequiredAmount(res, 2);
    ASSERT_EQ(2, act->getRequiredAmount(res));
    ASSERT_EQ(1, act->getRequirements().size());
}

TEST(ActivityTest, setRequiredAmountToZero_a) {
    Resource *res = new Resource("name");
    Activity *act = new Activity();
    act->setRequiredAmount(res, 0);
    ASSERT_EQ(0, act->getRequiredAmount(res));
    ASSERT_EQ(0, act->getRequirements().size());
}

TEST(ActivityTest, setRequiredAmountToZero_b) {
    Resource *res = new Resource("name");
    Activity *act = new Activity();
    act->setRequiredAmount(res, 1);
    act->setRequiredAmount(res, 0);
    ASSERT_EQ(0, act->getRequirements().size());
}

TEST(ActivityTest, addPrecedence) {
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();
    Precedence *p = new Precedence(a1, a2);
    a1->addPrecedence(p);
    a2->addPrecedence(p);

    QList<Precedence *> precedences = a1->getPrecedences();

    ASSERT_EQ(1, precedences.size());
    ASSERT_TRUE(precedences.contains(p));
}

TEST(ActivityTest, addWrongPrecedence) {
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();
    Activity *a3 = new Activity();
    Precedence *p = new Precedence(a1, a2);
    a1->addPrecedence(p);
    a2->addPrecedence(p);

    ASSERT_THROW(a3->addPrecedence(p), InstanceManipulationException);

    QList<Precedence *> precedences = a3->getPrecedences();
    ASSERT_FALSE(precedences.contains(p));
}

TEST(ActivityTest, testIncomingPrecedences) {
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();
    Precedence *p = new Precedence(a1, a2);
    a1->addPrecedence(p);
    a2->addPrecedence(p);

    ASSERT_FALSE(a1->getIncomingPrecedences().contains(p));
    ASSERT_TRUE(a2->getIncomingPrecedences().contains(p));
}

TEST(ActivityTest, testOutgoingPrecedences) {
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();
    Precedence *p = new Precedence(a1, a2);
    a1->addPrecedence(p);
    a2->addPrecedence(p);

    ASSERT_TRUE(a1->getOutgoingPrecedences().contains(p));
    ASSERT_FALSE(a2->getOutgoingPrecedences().contains(p));
}

TEST(ActivityTest, testRemovePrecedence) {
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();
    Precedence *p1 = new Precedence(a1, a2);
    Precedence *p2 = new Precedence(a2, a1);
    a1->addPrecedence(p1);
    a2->addPrecedence(p1);
    a1->addPrecedence(p2);
    a2->addPrecedence(p2);

    a1->removePrecedence(p1);

    ASSERT_FALSE(a1->getPrecedences().contains(p1));
    ASSERT_TRUE(a1->getPrecedences().contains(p2));
}
