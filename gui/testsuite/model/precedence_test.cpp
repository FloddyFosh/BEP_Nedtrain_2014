#include  <gtest/gtest.h>
#include "model/activity.h"
#include "model/job.h"

TEST(PrecedenceTest, testHard) {
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();
    Precedence *p1 = new Precedence(a1, a2);
    Precedence *p2 = new Precedence(a1, a2, true);
    ASSERT_TRUE(p1->isHard());
    ASSERT_TRUE(p2->isHard());
}

TEST(PrecedenceTest, testSoft) {
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();
    Precedence *p = new Precedence(a1, a2, false);
    ASSERT_FALSE(p->isHard());
}

TEST(PrecedenceTest, testIsIncoming) {
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();
    Precedence *p = new Precedence(a1, a2, false);
    ASSERT_FALSE(p->isIncoming(a1));
    ASSERT_TRUE(p->isIncoming(a2));
}

TEST(PrecedenceTest, testIsOutgoing) {
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();
    Precedence *p = new Precedence(a1, a2, false);
    ASSERT_TRUE(p->isOutgoing(a1));
    ASSERT_FALSE(p->isOutgoing(a2));
}

TEST(PrecedenceTest, testEquality) {
    Activity *a1 = new Activity();
    Activity *a2 = new Activity();
    Activity *a3 = new Activity();
    Precedence *p1 = new Precedence(a1, a2);
    Precedence *p2 = new Precedence(a1, a2);
    Precedence *p3 = new Precedence(a1, a3);
    Precedence *p4 = new Precedence(a3, a2);
    Precedence *p5 = new Precedence(a1, a2, false);

    ASSERT_TRUE(*p1 == *p1);
    ASSERT_TRUE(*p1 == *p2);
    ASSERT_FALSE(*p1 == *p3);
    ASSERT_FALSE(*p1 == *p4);
    ASSERT_FALSE(*p1 == *p5);
}
