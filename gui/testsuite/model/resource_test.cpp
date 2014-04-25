#include  <gtest/gtest.h>
#include "model/resource.h"
#include "model/activity.h"
#include "model/instance.h"

TEST(ResourceTest, NameTest) {
    Resource *r = new Resource("res");
    ASSERT_EQ("res", r->name());
    r->setName("name");
    ASSERT_EQ("name", r->name());
}

TEST(ResourceTest, CapacityTest) {
    Resource *r = new Resource("res");
    r->setCapacity(5);
    ASSERT_EQ(5, r->capacity());
}

TEST(ResourceTest, AddActivityTest) {
    Resource *r = new Resource("res");
    Activity *a = new Activity();
    Requirement *req = new Requirement(a, r, 5);
    r->addActivity(req);
    ASSERT_EQ(1, r->getRequirements().size());
    ASSERT_EQ(5, r->getRequirements().value(0)->amount());
}
