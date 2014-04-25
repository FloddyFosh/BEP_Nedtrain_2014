#include  <gtest/gtest.h>
#include "model/group.h"
#include "group_fixture.cpp"

TEST_F(GroupTest, setEST) {
	EXPECT_EQ(-1, group->getEST());
	group->setEST(1);
    EXPECT_EQ(1, group->getEST());
    EXPECT_EQ(1, a1->est());
    EXPECT_EQ(2, a2->est());
    EXPECT_EQ(4, a3->est());
}

TEST_F(GroupTest, setLST) {
    EXPECT_EQ(-1, group->getLST());
    group->setLST(1);
    EXPECT_EQ(1, group->getLST());
}

TEST_F(GroupTest, addActivity) {
    Activity* activity = new Activity();
    group->addActivity(activity);

    ASSERT_TRUE(group->getActivities().contains(a1));
}

TEST_F(GroupTest, hasGroups) {
    Group *group2 = new Group();
    EXPECT_FALSE(group2->hasActivities());

    Activity *a1 = new Activity();
    group2->addActivity(a1);

    EXPECT_TRUE(group2->hasActivities());
}

TEST_F(GroupTest, removeActivity) {
    group->removeActivity(a1);
    group->removeActivity(a2);
    group->removeActivity(a3);

    ASSERT_FALSE(group->hasActivities());
}

TEST_F(GroupTest, testSize) {
    Group* g = new Group();
    EXPECT_EQ(0, g->size());
    g->addActivity(a1);
    EXPECT_EQ(1, g->size());
    g->addActivity(a1);
    EXPECT_EQ(2, g->size());
}

TEST_F(GroupTest, getDuration) {
    ASSERT_EQ(6, group->getDuration());
}

TEST_F(GroupTest, getEnd) {
    group->setEST(1);

    ASSERT_EQ(7, group->getEnd());
}

TEST_F(GroupTest, getJob) {
    Group *group2 = new Group(1, 0, 2, 0);

    EXPECT_EQ(-1, group->getJob());
    EXPECT_EQ(1, group2->getJob());
}

TEST_F(GroupTest, moveLeft1) {
    group->setEST(0);
    group->moveLeft(a1);

    ASSERT_EQ(0, a1->est());
    ASSERT_EQ(1, a2->est());
    ASSERT_EQ(3, a3->est());
}

TEST_F(GroupTest, moveLeft2) {
    group->setEST(0);
    group->moveLeft(a2);

    ASSERT_EQ(2, a1->est());
    ASSERT_EQ(0, a2->est());
    ASSERT_EQ(3, a3->est());
}

TEST_F(GroupTest, moveLeft3) {
    group->setEST(0);
    group->moveLeft(a3);

    ASSERT_EQ(0, a1->est());
    ASSERT_EQ(4, a2->est());
    ASSERT_EQ(1, a3->est());
}

TEST_F(GroupTest, moveRight1) {
    group->setEST(0);
    group->moveRight(a1);

    ASSERT_EQ(2, a1->est());
    ASSERT_EQ(0, a2->est());
    ASSERT_EQ(3, a3->est());
}

TEST_F(GroupTest, moveRight2) {
    group->setEST(0);
    group->moveRight(a2);

    ASSERT_EQ(0, a1->est());
    ASSERT_EQ(4, a2->est());
    ASSERT_EQ(1, a3->est());
}

TEST_F(GroupTest, moveRight3) {
    group->setEST(0);
    group->moveRight(a3);

    ASSERT_EQ(0, a1->est());
    ASSERT_EQ(1, a2->est());
    ASSERT_EQ(3, a3->est());
}
