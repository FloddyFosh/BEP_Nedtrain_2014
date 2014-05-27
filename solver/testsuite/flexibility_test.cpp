#include  <gtest/gtest.h>

#include "../src/clp/flexibility.h"
#include "../src/clp/constraints.h"

class FlexibilityTest : public ::testing::Test {
  protected:
    Constraints cstr;
    virtual void SetUp() {
       cstr.addVariable("A");     
    }
    // virtual void TearDown() {}
};

TEST_F(FlexibilityTest, Test_1) {
    cstr.addConstraint("B", "A", 3);
    cstr.addConstraint("C", "B", 2);
    cstr.addConstraint("C", "A", 4);
    pair<double, map<string, double> > output = useClpToSolve(cstr);
    double flex = output.first;
    map<string, double> vars = output.second;
    EXPECT_EQ(5.0, flex);
}

TEST_F(FlexibilityTest, Test_2) {
    cstr.addConstraint("B", "A", 10);
    cstr.addConstraint("B", "C", 10);
    cstr.addConstraint("C", "D", 10);
    cstr.addConstraint("D", "C", 10);
    double flex = useClpToSolve(cstr).first;
    EXPECT_EQ(30.0, flex);
}
