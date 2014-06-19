#include  <gtest/gtest.h>

#include "../../src/flexibility.h"
#include "../../src/constraints.h"

class FlexibilityTest : public ::testing::Test {
  protected:
    Constraints cstr;
    // virtual void SetUp() {}
    // virtual void TearDown() {}
};

TEST_F(FlexibilityTest, Test_1) {
    cstr.setUpperLimit("a", 42);
    cstr.setLowerLimit("a", 0);
    cstr.setLocked("a", false);

    useClpToSolve(&cstr);

    EXPECT_EQ(42, getFlexibility());
}

TEST_F(FlexibilityTest, Test_2) {
    cstr.addConstraint("D", "A", -10);
    cstr.addConstraint("D", "C", -10);
    cstr.addConstraint("B", "A", -5);
    cstr.addConstraint("B", "C", -5);
    cstr.addConstraint("D", "C", -10);

    cstr.setUpperLimit("A", 15);
    cstr.setLowerLimit("A", 0);
    cstr.setUpperLimit("B", 15);
    cstr.setLowerLimit("B", 0);
    cstr.setUpperLimit("C", 19);
    cstr.setLowerLimit("C", 0);
    cstr.setUpperLimit("D", 10);
    cstr.setLowerLimit("D", 0);
    cstr.setUpperLimit("E", 10);
    cstr.setLowerLimit("E", 0);

    cstr.setLocked("A", false);
    cstr.setLocked("B", false);
    cstr.setLocked("C", false);
    cstr.setLocked("D", false);
    cstr.setLocked("E", false);

    useClpToSolve(&cstr);

    EXPECT_EQ(29, getFlexibility());
}

TEST_F(FlexibilityTest, Test_3) {
    cstr.addConstraint("A", "B", -2);
    cstr.addConstraint("A", "B", -2);
    cstr.addConstraint("F", "G", -2);    

    cstr.setUpperLimit("A", 108);
    cstr.setLowerLimit("A", 100);
    cstr.setUpperLimit("B", 108);
    cstr.setLowerLimit("B", 100);
    cstr.setUpperLimit("F", 108);
    cstr.setLowerLimit("F", 100);
    cstr.setUpperLimit("G", 108);
    cstr.setLowerLimit("G", 100);

    cstr.setLocked("A", false);
    cstr.setLocked("B", false);    
    cstr.setLocked("F", false);
    cstr.setLocked("G", false);

    useClpToSolve(&cstr);

    EXPECT_EQ(12, getFlexibility());
}
