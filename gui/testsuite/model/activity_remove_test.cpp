#include  <gtest/gtest.h>

#include "model/instance.h"
#include "model/activity.h"
#include "model/job.h"
#include "model/requirement.h"

TEST(ActivityRemoveTest, removeFromJob) {
    Instance *instance = new Instance();
    Job *job = instance->addJob(1, 0, 1, "Project");
    Activity *activity1 = new Activity(job, 1, "A1", 5);
    Activity *activity2 = new Activity(job, 2, "A2", 5);
    job->addActivity(activity1);
    job->addActivity(activity2);

    instance->removeActivity(activity1);

    ASSERT_TRUE(job->getActivities().keys(activity1).isEmpty());
    ASSERT_FALSE(job->getActivities().keys(activity2).isEmpty());
}

TEST(ActivityRemoveTest, removeFromResources) {
    Instance *instance = new Instance();
    Job *job = instance->addJob(1, 0, 1, "Project");
    Resource *resource = new Resource("Resource");

    Activity *activity1 = new Activity(job, 1, "A1", 0);
    activity1->setRequiredAmount(resource, 1);
    job->addActivity(activity1);

    Activity *activity2 = new Activity(job, 2, "A2", 0);
    activity2->setRequiredAmount(resource, 1);
    job->addActivity(activity2);

    instance->removeActivity(activity1);

    ASSERT_EQ(1, resource->getRequirements().size());
    Requirement *r = resource->getRequirements().first();
    ASSERT_EQ(activity2, r->activity());
}

TEST(ActivityRemoveTest, removePrecedences) {
    Instance *instance = new Instance();
    Job *job = instance->addJob(1, 0, 1, "Project");
    Activity *activity1 = new Activity(job, 1, "A1", 5);
    Activity *activity2 = new Activity(job, 2, "A2", 5);
    job->addActivity(activity1);
    job->addActivity(activity2);

    // Add precedence: activity1 -> activity2
    instance->addPrecedence(job->id(), activity1->id(), job->id(), activity2->id(), true);

    ASSERT_TRUE(!instance->getHardPrecedences().isEmpty());
    instance->removeActivity(activity1);
    ASSERT_TRUE(instance->getHardPrecedences().isEmpty());
}

TEST(ActivityRemoveTest, removeBackwardsPrecedences) {
    Instance *instance = new Instance();
    Job *job = instance->addJob(1, 0, 1, "Project");
    Activity *activity1 = new Activity(job, 1, "A1", 5);
    Activity *activity2 = new Activity(job, 2, "A2", 5);
    job->addActivity(activity1);
    job->addActivity(activity2);

    // Add precedence: activity2 -> activity1
    instance->addPrecedence(job->id(), activity2->id(), job->id(), activity1->id(), true);

    ASSERT_TRUE(!instance->getHardPrecedences().isEmpty());
    instance->removeActivity(activity1);
    ASSERT_TRUE(instance->getHardPrecedences().isEmpty());
}

TEST(ActivityRemoveTest, fixImplicitPrecedence) {
    Instance *instance = new Instance();
    Job *job = instance->addJob(1, 0, 1, "Project");
    Activity *activity1 = new Activity(job, 1, "A1", 5);
    Activity *activity2 = new Activity(job, 2, "A2", 5);
    Activity *activity3 = new Activity(job, 3, "A3", 5);
    job->addActivity(activity1);
    job->addActivity(activity2);
    job->addActivity(activity3);

    // Add precedences:
    //   activity1 -> activity2
    //   activity2 -> activity3
    instance->addPrecedence(job->id(), activity1->id(), job->id(), activity2->id(), true);
    instance->addPrecedence(job->id(), activity2->id(), job->id(), activity3->id(), true);

    instance->removeActivity(activity2);

    ASSERT_EQ(1, instance->getHardPrecedences().size());
    // Expect one precedence: activity1 -> activity3
    Precedence *implicit = instance->getHardPrecedences().first();
    ASSERT_EQ(activity1, implicit->a1());
    ASSERT_EQ(activity3, implicit->a2());
}

TEST(ActivityRemoveTest, fixImplicitChain) {
    Instance *instance = new Instance();
    Job *job = instance->addJob(1, 0, 1, "Project");
    Activity *activity1 = new Activity(job, 1, "A1", 5);
    Activity *activity2 = new Activity(job, 2, "A2", 5);
    Activity *activity3 = new Activity(job, 3, "A3", 5);
    Activity *activity4 = new Activity(job, 4, "A4", 5);
    Activity *activity5 = new Activity(job, 5, "A5", 5);
    job->addActivity(activity1);
    job->addActivity(activity2);
    job->addActivity(activity3);
    job->addActivity(activity4);
    job->addActivity(activity5);

    instance->addPrecedence(job->id(), activity1->id(), job->id(), activity2->id(), true);
    instance->addPrecedence(job->id(), activity2->id(), job->id(), activity3->id(), true);
    instance->addPrecedence(job->id(), activity3->id(), job->id(), activity4->id(), true);
    instance->addPrecedence(job->id(), activity4->id(), job->id(), activity5->id(), true);

    instance->removeActivity(activity2);
    instance->removeActivity(activity3);
    instance->removeActivity(activity4);

    ASSERT_EQ(1, instance->getHardPrecedences().size());
    // Expect one precedence: activity1 -> activity5
    Precedence *implicit = instance->getHardPrecedences().first();
    ASSERT_EQ(activity1, implicit->a1());
    ASSERT_EQ(activity5, implicit->a2());
}
