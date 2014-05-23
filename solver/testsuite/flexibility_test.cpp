#include  <gtest/gtest.h>

#include "../src/clp/flexibility.h"
#include "../src/clp/constraints.h"

class FlexibilityTest : public ::testing::Test {
  protected:
    Constraints cstr;
    // virtual void setUp() {}
    // virtual void TearDown() {}
};

TEST_F(FlexibilityTest, Test_1) {
    cstr.addVariable("A");
    cstr.addConstraint("B", "A", 3);
    cstr.addConstraint("C", "B", 2);
    cstr.addConstraint("C", "A", 4);
    double flex = useClpToSolve(cstr).first;
    EXPECT_EQ(5.0, flex);
}

TEST_F(FlexibilityTest, Test_2) {

}
