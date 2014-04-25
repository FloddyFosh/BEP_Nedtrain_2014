#include  <gtest/gtest.h>
#include "model/instance.h"

TEST(RequirementTest, testGetActivity) {
    Activity *activity = new Activity();
    Resource *resource = new Resource("R");
    Requirement *requirement = new Requirement(activity, resource, 2);

    ASSERT_EQ(activity, requirement->activity());
}

TEST(RequirementTest, testGetResource) {
    Activity *activity = new Activity();
    Resource *resource = new Resource("R");
    Requirement *requirement = new Requirement(activity, resource, 2);

    ASSERT_EQ(resource, requirement->resource());
}

TEST(RequirementTest, testGetAmount) {
    Activity *activity = new Activity();
    Resource *resource = new Resource("R");
    Requirement *requirement = new Requirement(activity, resource, 2);

    ASSERT_EQ(2, requirement->amount());
}

TEST(RequirementTest, testSetAmount) {
    Activity *activity = new Activity();
    Resource *resource = new Resource("R");
    Requirement *requirement = new Requirement(activity, resource, 2);

    requirement->setAmount(1);
    ASSERT_EQ(1, requirement->amount());
}
