#include  <gtest/gtest.h>

#include "constraints.cpp"

TEST(Constraints, Size_Test) {
    Constraints cstr;
    ASSERT_EQ(0, cstr.size());

    cstr.addConstraint("var1","var2",10);
    ASSERT_EQ(1, cstr.size());

    cstr.addConstraint("var1","var2",10);
    ASSERT_EQ(2, cstr.size());

    cstr.addConstraint("var1","var2",10);
    ASSERT_EQ(3, cstr.size());

    for(int i = 0; i < 10; i++) {
        cstr.addConstraint("var" + i,"var2",i);
        ASSERT_EQ(4 + i, cstr.size());
    }
}

TEST(Constraints, AddConstraint_Test) {
    Constraints cstr;
    cstr.addConstraint("a", "b", 42);
    ASSERT_EQ(0, cstr[0].t1);
    ASSERT_EQ(1, cstr[0].t2);
    ASSERT_EQ(42, cstr[0].c);

    cstr.addConstraint("c", "d", 33);
    ASSERT_EQ(2, cstr[1].t1);
    ASSERT_EQ(3, cstr[1].t2);
    ASSERT_EQ(33, cstr[1].c);

    cstr.addConstraint("b", "d", 21);
    ASSERT_EQ(1, cstr[2].t1);
    ASSERT_EQ(3, cstr[2].t2);
    ASSERT_EQ(21, cstr[2].c);
}

TEST(Constraints, GetAmountOfVariables_Test) {
    Constraints cstr;
    ASSERT_EQ(0, cstr.getAmountOfVariables());
    
    cstr.addConstraint("a", "b", 1);
    ASSERT_EQ(2, cstr.getAmountOfVariables());
    
    for(int i = 0 ; i < 3; i++) {
        cstr.addConstraint("a", "b", i);
        ASSERT_EQ(2, cstr.getAmountOfVariables());
    }

    cstr.addConstraint("a1", "a1", 11);
    ASSERT_EQ(3, cstr.getAmountOfVariables());
    
    cstr.addConstraint("d" , "e", 11);
    ASSERT_EQ(5, cstr.getAmountOfVariables());
}

TEST(Constraints, GetVariableName_Test) {
    Constraints cstr;
    cstr.addConstraint("aaa", "bbb", 1);
    ASSERT_EQ(cstr.getVariableName(1), "bbb"); 
    cstr.addConstraint("eee", "aaa", 2);
    ASSERT_EQ(cstr.getVariableName(2), "eee");
    ASSERT_EQ(cstr.getVariableName(0), "aaa");  
    cstr.addConstraint("a", "b", 9);
    ASSERT_EQ(cstr.getVariableName(3), "a"); 
    ASSERT_EQ(cstr.getVariableName(4), "b"); 
}

TEST(Constraints, AddVariable_Test) {
    Constraints cstr;
    
}
