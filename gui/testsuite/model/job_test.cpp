#include  <gtest/gtest.h>
#include "model/activity.h"
#include "model/job.h"

TEST(JobTest, AddActivityTest) {
    Job *j = new Job();

    ASSERT_EQ(0, j->findAvailableActivityId());

    Activity *a1 = new Activity(j, 0, "a1", 1);
    j->addActivity(a1);
    ASSERT_EQ(1, j->findAvailableActivityId());

    Activity *a2 = new Activity(j, 1, "a2", 1);
    j->addActivity(a2);
    ASSERT_EQ(2, j->findAvailableActivityId());
}

TEST(JobTest, NameTest) {
    Job *j = new Job();
    j->setName("name");
    ASSERT_EQ("name", j->name());
}

TEST(JobTest, ReleaseDateTest) {
    Job *j = new Job();
    j->setReleaseDate(55);
    ASSERT_EQ(55, j->releaseDate());
}

TEST(JobTest, DueDateTest) {
    Job *j = new Job();
    j->setDueDate(55);
    ASSERT_EQ(55, j->dueDate());
}
