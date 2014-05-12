#include  <gtest/gtest.h>
#include "model/instance.h"
#include "data/instance_reader.h"
#include "controller/exceptions.h"

TEST(InstanceTest, readFile) {
    Instance *instance = InstanceReader::load(qApp->applicationDirPath()+"/resources/small.instance");
    ASSERT_EQ(1, instance->getResources().size());
    ASSERT_EQ(2, instance->getJobs().size());
}

TEST(InstanceTest, addJob) {
	Instance *instance = new Instance();
	instance->addJob(1, 0, 10, "Project");

	ASSERT_EQ("Project", instance->J(1)->name());
	ASSERT_EQ(0, instance->J(1)->releaseDate());
	ASSERT_EQ(10, instance->J(1)->dueDate());
}

TEST(InstanceTest, removeJob) {
    Instance *instance = new Instance();
    instance->addJob(1, 0, 10, "Project");
    instance->addActivity(1, 1, 0, 0, 1, "A", 0);

    instance->removeJob(instance->J(1));

    EXPECT_FALSE(instance->J(1));
}

TEST(InstanceTest, addResource) {
    Instance *instance = new Instance();

    instance->addResource(1, "R");
    Resource *r = instance->containsResource("R");

    EXPECT_EQ("R", r->name());
    EXPECT_EQ(1, r->capacity());
}

TEST(InstanceTest, containsResource) {
    Instance *instance = new Instance();
    ASSERT_FALSE(instance->containsResource("R"));

    instance->addResource(1, "R");
    Resource *r = instance->containsResource("R");

    ASSERT_TRUE(r != 0);
}

TEST(InstanceTest, addRequirement) {
    Instance *instance = new Instance();
    instance->addResource(1, 1, "R");
    instance->addJob(1, 0, 1, "Job");
    instance->addActivity(1, 1, 0, 0, 1, "A1", 0);
    instance->addRequirement(1, 1, 1, 2);

    int amount = instance->A(1,1)->getRequiredAmount(instance->R(1));
    ASSERT_EQ(2, amount);
}

TEST(InstanceTest, removeResource) {
    Instance *instance = new Instance();
    instance->addResource(1, 2, "R");
    instance->removeResource(1);

    ASSERT_FALSE(instance->containsResource("R"));
}

TEST(InstanceTest, addDuplicateJob) {
    Instance *instance = new Instance();
    instance->addJob(1, 0, 10, "Project");

    ASSERT_THROW(instance->addJob(1, 0, 10, "Project"), InstanceManipulationException);
}

TEST(InstanceTest, testLoadedPrecedencesAreHard) {
    Instance *instance = InstanceReader::load(qApp->applicationDirPath()+"/resources/test.instance");
    bool allHard = true;
    foreach(Precedence *p, instance->getHardPrecedences())
        allHard = allHard && p->isHard();

    ASSERT_TRUE(allHard);
    ASSERT_EQ(39, instance->getHardPrecedences().size());
    ASSERT_EQ(0, instance->getSoftPrecedences().size());
}

TEST(InstanceTest, testLoadSoftAndHardPrecedences) {
    Instance *instance = InstanceReader::load(qApp->applicationDirPath()+"/resources/precedences.instance");
    ASSERT_EQ(1, instance->getHardPrecedences().size());
    ASSERT_EQ(2, instance->getSoftPrecedences().size());
}

TEST(InstanceTest, testAddHardPrecedence) {
    Instance *instance = new Instance();
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();

    instance->addPrecedence(a1, a2, true);

    ASSERT_EQ(1, instance->getHardPrecedences().size());
    ASSERT_EQ(0, instance->getSoftPrecedences().size());
}

TEST(InstanceTest, testAddSoftPrecedence) {
    Instance *instance = new Instance();
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();

    instance->addPrecedence(a1, a2, false);

    ASSERT_EQ(0, instance->getHardPrecedences().size());
    ASSERT_EQ(1, instance->getSoftPrecedences().size());
}

TEST(InstanceTest, testAddPrecedenceToActivities) {
    Instance *instance = new Instance();
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();

    instance->addPrecedence(a1, a2, true);
    QList<Precedence *> precedences = a1->getPrecedences();

    ASSERT_EQ(1, precedences.size());
    ASSERT_EQ(a1, precedences.first()->a1());
    ASSERT_EQ(a2, precedences.first()->a2());
    ASSERT_TRUE(precedences.first()->isHard());
}

TEST(InstanceTest, testPrecedenceExists) {
    Instance *instance = new Instance();
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();
    Activity *a3 = new Activity();
    Precedence *p = new Precedence(a1, a2);
    a1->addPrecedence(p);
    a2->addPrecedence(p);

    ASSERT_TRUE(instance->precedenceExists(a1, a2));
    ASSERT_FALSE(instance->precedenceExists(a2, a1));
    ASSERT_FALSE(instance->precedenceExists(a1, a3));
    ASSERT_FALSE(instance->precedenceExists(a3, a1));
}

TEST(InstanceTest, testImpliedPrecedenceExists) {
    Instance *instance = new Instance();
    try {
        instance->addJob(1, 0, 1, "Job");
        instance->addActivity(1, 1, 0, 0, 1, "A1", 0);
        instance->addActivity(1, 2, 0, 0, 1, "A2", 0);
        instance->addActivity(1, 3, 0, 0, 1, "A3", 0);
        instance->addActivity(1, 4, 0, 0, 1, "A4", 0);
        instance->addPrecedence(1, 1, 1, 2, true); // 1 --> 2
        instance->addPrecedence(1, 2, 1, 3, true); // 2 --> 3
    } catch(InstanceManipulationException const& e) { e.printWarning(); FAIL(); }

    ASSERT_TRUE(instance->impliedPrecedenceExists(instance->A(1,1), instance->A(1,2)));
    ASSERT_TRUE(instance->impliedPrecedenceExists(instance->A(1,1), instance->A(1,3)));
    ASSERT_FALSE(instance->impliedPrecedenceExists(instance->A(1,1), instance->A(1,4)));
    ASSERT_FALSE(instance->impliedPrecedenceExists(instance->A(1,2), instance->A(1,1)));
}

TEST(InstanceTest, testIgnoreDuplicatePrecedence) {
    Instance *instance = new Instance();
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();

    instance->addPrecedence(a1, a2, false);
    ASSERT_THROW(instance->addPrecedence(a1, a2, false), InstanceManipulationException);

    ASSERT_EQ(0, instance->getHardPrecedences().size());
    ASSERT_EQ(1, instance->getSoftPrecedences().size());

    instance->clearSoftPrecedences();
    instance->addPrecedence(a1, a2, true);

    ASSERT_THROW(instance->addPrecedence(a1, a2, true), InstanceManipulationException);

    ASSERT_EQ(1, instance->getHardPrecedences().size());
    ASSERT_EQ(0, instance->getSoftPrecedences().size());
}

TEST(InstanceTest, testIgnoreSoftPrecedenceIfHardExists) {
    Instance *instance = new Instance();
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();

    instance->addPrecedence(a1, a2, true);
    ASSERT_THROW(instance->addPrecedence(a1, a2, false), InstanceManipulationException);

    ASSERT_EQ(1, instance->getHardPrecedences().size());
    ASSERT_EQ(0, instance->getSoftPrecedences().size());
}

TEST(InstanceTest, testTransformSoftPrecedenceToHard) {
    Instance *instance = new Instance();
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();

    instance->addPrecedence(a1, a2, false);
    instance->addPrecedence(a1, a2, true);

    ASSERT_EQ(1, instance->getHardPrecedences().size());
    ASSERT_EQ(0, instance->getSoftPrecedences().size());

    Precedence *p = instance->precedenceExists(a1, a2);
    ASSERT_TRUE(p->isHard());
}

TEST(InstanceTest, testAddPrecedenceCycle) {
    Instance *instance = new Instance();
    try {
        instance->addJob(1, 0, 1, "Job");
        instance->addActivity(1, 1, 0, 0, 1, "A1", 0);
        instance->addActivity(1, 2, 0, 0, 1, "A2", 0);
        instance->addActivity(1, 3, 0, 0, 1, "A3", 0);
        instance->addActivity(1, 4, 0, 0, 1, "A4", 0);
        instance->addPrecedence(1, 1, 1, 2, true); // 1 --> 2
        instance->addPrecedence(1, 2, 1, 3, true); // 2 --> 3
    } catch(InstanceManipulationException const& e) { e.printWarning(); FAIL(); }

    ASSERT_THROW(instance->addPrecedence(instance->A(1,3), instance->A(1,1), true), InstanceManipulationException);
    ASSERT_THROW(instance->addPrecedence(instance->A(1,2), instance->A(1,1), true), InstanceManipulationException);
}

TEST(InstanceTest, testStringRepresentation) {
    Instance *instance = new Instance();
    instance->addJob(1, 0, 5, "Job");
    instance->addActivity(1, 1, 0, 0, 2, "Act1", 0);
    instance->addActivity(1, 2, 0, 0, 3, "Act2", 0);
    instance->addActivity(1, 3, 0, 0, 4, "Act3", 0);
    instance->addResource(3, 2, "Res");
    instance->addRequirement(1, 1, 3, 1);
    instance->addPrecedence(1, 1, 1, 2, true);
    instance->addPrecedence(1, 2, 1, 3, false);

    QString representation = instance->toString();

    EXPECT_TRUE(representation.contains("J 1 0 5 \"Job\""));
    EXPECT_TRUE(representation.contains("A 1 1 2 \"Act1\""));
    EXPECT_TRUE(representation.contains("A 1 2 3 \"Act2\""));
    EXPECT_TRUE(representation.contains("A 1 3 4 \"Act3\""));
    EXPECT_TRUE(representation.contains("R 3 2 \"Res\""));
    EXPECT_TRUE(representation.contains("Q 1 1 3 1"));
    EXPECT_TRUE(representation.contains("P 1 1 1 2"));
    EXPECT_TRUE(representation.contains("S 1 2 1 3"));
}
