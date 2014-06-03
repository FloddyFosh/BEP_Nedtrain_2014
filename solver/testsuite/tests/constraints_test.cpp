#include  <gtest/gtest.h>

#include "../../src/constraints.h"

class ConstraintsTest : public ::testing::Test {
  protected:
    Constraints cstr;
    // virtual void SetUp() {}
    // virtual void TearDown() {}
}; 

TEST_F(ConstraintsTest, Size) {
    EXPECT_EQ(0, cstr.size());

    cstr.addConstraint("var1","var2",10);
    EXPECT_EQ(1, cstr.size());

    cstr.addConstraint("var1","var2",10);
    EXPECT_EQ(2, cstr.size());

    cstr.addConstraint("var1","var2",10);
    EXPECT_EQ(3, cstr.size());

    for(int i = 0; i < 10; i++) {
        cstr.addConstraint("var" + i,"var2",i);
        EXPECT_EQ(4 + i, cstr.size());
    }
}

TEST_F(ConstraintsTest, AddConstraint) {
    cstr.addConstraint("a", "b", 42);
    EXPECT_EQ(0, cstr[0].t1);
    EXPECT_EQ(1, cstr[0].t2);
    EXPECT_EQ(42, cstr[0].c);

    cstr.addConstraint("c", "d", 33);
    EXPECT_EQ(2, cstr[1].t1);
    EXPECT_EQ(3, cstr[1].t2);
    EXPECT_EQ(33, cstr[1].c);

    cstr.addConstraint("b", "d", 21);
    EXPECT_EQ(1, cstr[2].t1);
    EXPECT_EQ(3, cstr[2].t2);
    EXPECT_EQ(21, cstr[2].c);
}

TEST_F(ConstraintsTest, GetAmountOfVariables) {
    EXPECT_EQ(0, cstr.getAmountOfVariables());
    
    cstr.addConstraint("a", "b", 1);
    EXPECT_EQ(2, cstr.getAmountOfVariables());
    
    for(int i = 0 ; i < 3; i++) {
        cstr.addConstraint("a", "b", i);
        EXPECT_EQ(2, cstr.getAmountOfVariables());
    }

    cstr.addConstraint("a1", "a1", 11);
    EXPECT_EQ(3, cstr.getAmountOfVariables());
    
    cstr.addConstraint("d" , "e", 11);
    EXPECT_EQ(5, cstr.getAmountOfVariables());
}

TEST_F(ConstraintsTest, GetVariableName) {
    cstr.addConstraint("aaa", "bbb", 1);
    EXPECT_EQ("bbb", cstr.getVariableName(1)); 
    cstr.addConstraint("eee", "aaa", 2);
    EXPECT_EQ("eee", cstr.getVariableName(2));
    EXPECT_EQ("aaa", cstr.getVariableName(0));  
    cstr.addConstraint("a", "b", 9);
    EXPECT_EQ("a", cstr.getVariableName(3)); 
    EXPECT_EQ("b", cstr.getVariableName(4)); 
}

TEST_F(ConstraintsTest, AddVariable) {
    EXPECT_EQ(0, cstr.addVariable("a"));    
    EXPECT_EQ("a", cstr.getVariableName(0));    
    EXPECT_EQ(1, cstr.addVariable("b"));
    EXPECT_EQ(0, cstr.addVariable("a"));
    EXPECT_EQ(1, cstr.addVariable("b"));
    EXPECT_EQ(2, cstr.getAmountOfVariables());

    cstr.addConstraint("aaa", "bbb", 1);
    EXPECT_EQ(4, cstr.getAmountOfVariables());
    EXPECT_EQ(2, cstr.addVariable("aaa"));
    EXPECT_EQ(3, cstr.addVariable("bbb"));
    EXPECT_EQ("aaa", cstr.getVariableName(2));
    EXPECT_EQ(4, cstr.getAmountOfVariables());
    
}
