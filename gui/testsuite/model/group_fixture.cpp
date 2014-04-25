#include  <gtest/gtest.h>
#include "model/group.h"

class GroupTest: public ::testing::Test {
protected:
    virtual void SetUp() {
        group = new Group();

        a1 = new Activity();
        a1->setDuration(1);
        group->addActivity(a1);

        a2 = new Activity();
        a2->setDuration(2);
        group->addActivity(a2);

        a3 = new Activity();
        a3->setDuration(3);
        group->addActivity(a3);
    }
    virtual void TearDown() {
        delete group;
        delete a1;
        delete a2;
        delete a3;
    }
    Group *group;
    Activity *a1, *a2, *a3;
};
