#include <gtest/gtest.h>

#include "../../src/tmsp.h"

class TMSPTest : public ::testing::Test {
    protected:

    void addTestAct(int i, int j) {
        // add train
        char* train1 = (char *) "a";
        add_train(i, 0, 10, train1);

        // add activity
        char* name = (char *) "b";
        add_activity(i, j, 1, name);
    }

    virtual void SetUp() {
        tmsp = new tmsp_t;
    }

    virtual void TearDown() {
        delete tmsp;
    }    
};

TEST_F(TMSPTest, AddTrein) {
    char* train1 = (char *) "train1";
    add_train(1, 0, 10, train1);
    
    EXPECT_EQ(0, Tr(1)->release_date);
    EXPECT_EQ(10, Tr(1)->due_date);
    EXPECT_EQ(string("train1"), string(Tr(1)->name));
}

TEST_F(TMSPTest, AddActivity) {
    // add train
    char* train1 = (char *) "train1";
    add_train(1, 0, 10, train1);

    // add activity
    char* name = (char *) "act1";
    add_activity(1, 2, 3, name);
    activity* a = A(1, 2);

    EXPECT_EQ(1, a->i);
    EXPECT_EQ(2, a->j);
    EXPECT_EQ(3, a->duration);
    EXPECT_EQ(string("act1"), string(a->name));
}

TEST_F(TMSPTest, AddResource) {
    ADD_FAILURE(); // TODO
}

TEST_F(TMSPTest, AddRequirement) {
    ADD_FAILURE(); // TODO
}

TEST_F(TMSPTest, AddPrecedence_1) {
    add_precedence(2, 3, 5, 6, true);
    EXPECT_EQ(2, P(0)->i1);
    EXPECT_EQ(3, P(0)->j1);
    EXPECT_EQ(5, P(0)->i2);
    EXPECT_EQ(6, P(0)->j2);
    EXPECT_EQ(true, P(0)->isHard);
}

TEST_F(TMSPTest, AddPrecedence_2) {
    add_precedence(2, 3, 5, 6, false);
    add_precedence(5, 6, 4, 3, false);
    EXPECT_EQ(5, P(1)->i1);
    EXPECT_EQ(6, P(1)->j1);
    EXPECT_EQ(4, P(1)->i2);
    EXPECT_EQ(3, P(1)->j2);
    EXPECT_EQ(false, P(1)->isHard);
}

TEST_F(TMSPTest, AddFlex_1) {
    addTestAct(1, 2);
    
    add_flex(1, 2, 42);
    EXPECT_EQ(42, A(1,2)->flex); 
}

TEST_F(TMSPTest, AddFlex_2) {
    addTestAct(1, 3);
    
    add_flex(1, 3, 42);
    add_flex(1, 3, 35);
    EXPECT_EQ(35, A(1,3)->flex); 
}

TEST_F(TMSPTest, AddGroupChild) {
    addTestAct(0, 1);
    addTestAct(2, 3);

    add_groupchild(0, 1, 2, 3);
    activity* a = A(0, 1)->groupchilds[0];
    EXPECT_EQ(2, a->i);
    EXPECT_EQ(3, a->j);
}

TEST_F(TMSPTest, AddTrainMutexes) {
    // add_train_mutexes();
    ADD_FAILURE(); // TODO
}

TEST_F(TMSPTest, UpdateStartTimes) {
    addTestAct(0, 1);
    A(0, 1)->duration = 5;
    A(0, 1)->est = 0;
    A(0, 1)->lst = 5;

    addTestAct(2, 3);
    A(2, 3)->duration = 3;
    A(2, 3)->est = 0;
    A(2, 3)->lst = 7;
    
    update_starttimes(0, 1, 2, 3);
    EXPECT_EQ(0 , A(0,1)->est);
    EXPECT_EQ(2 , A(0,1)->lst);
    EXPECT_EQ(5 , A(0,1)->duration);
    EXPECT_EQ(5 , A(2,3)->est);
    EXPECT_EQ(7 , A(2,3)->lst);
    EXPECT_EQ(3 , A(2,3)->duration);
}

