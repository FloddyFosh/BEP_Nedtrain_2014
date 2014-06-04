#include  <gtest/gtest.h>

#include "../../src/chaining.h"
#include "../../src/tmsp.h"

class ChainingTest : public ::testing::Test {
  protected:
    std::stringstream buffer;
    std::streambuf *sbuf;

    virtual void SetUp() {
        //Redirect stdout to buffer
        *sbuf = std::cout.rdbuf();
        std::cout.rdbuf(buffer.rdbuf());

        add_resource(0,3,"engineers");
        add_resource(1,1,"cleaners");

        add_train(0,5,25,"t0");
        add_activity(0,0,10,"act0,0",5,12);
        add_requirement(0,0,0,2);
        add_requirement(0,0,1,1);
        add_activity(0,1,3,"act0,1",6,8);
        add_requirement(0,1,1,1);

        add_train(2,0,30,"t1");
        add_activity(2,0,12,"act2,0",0,23);
    }
    virtual void TearDown() {
        //Redirect stdout back to itself
        std::cout.rdbuf(sbuf);
    }
};

TEST_F(ChainingTest, InitializeActivitiesTest) {
    initializeActivities();
    EXPECT_EQ(activities.size(),3);
}

TEST_F(ChainingTest, SortActivitiesTest){
    sort(activities.begin(), activities.end(), compareEST);
    EXPECT_LE(activities[0].est,activities[1]);
    EXPECT_LE(activities[1].est,activities[2]);
}

TEST_F(ChainingTest, InitializeChainsTest) {
    initializeChains();
    EXPECT_EQ(chains.size(), 4);
}

TEST_F(ChainingTest, SelectChainTest){
    initializeChains();
    activity* act = A(0,0);
    pair<int,int> chainId = selectChain(act->i,act->j,0);
    chains[chainId] = act;
    EXPECT_EQ(chainId.first,0);
    EXPECT_EQ(chainId.second,0);
}

TEST_F(ChainingTest, SelectChainExceptionTest){
    initializeChains();
    selectChain();
}

TEST_F(ChainingTest, PushToChainTest){

}

TEST_F(ChainingTest, PrintChainTest){

}

TEST_F(ChainingTest, AddFrameTest){

}
