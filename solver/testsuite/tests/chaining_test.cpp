#include  <gtest/gtest.h>

#include "../../src/chaining.cpp"
#include "../../src/tmsp.h"
#include "../../src/exceptions.h"

using namespace std;

class ChainingTest : public ::testing::Test {
  protected:
    ostringstream buffer;
    std::streambuf *sbuf;

    virtual void SetUp() {
        tmsp = new tmsp_t;
        //Redirect stdout to buffer
        sbuf = cout.rdbuf();
        cout.rdbuf(buffer.rdbuf());

        string eng,cl,t0,t1,a00,a01,a10;
        eng = "engineers", cl = "cleaners";
        add_resource(0,3,&eng[0]);
        add_resource(1,1,&cl[0]);

        t0 = "t0", a00 = "act0,0", a01 = "act0,1";
        add_train(0,5,22,&t0[0]);
        add_activity(0,0,10,&a00[0],5,12);
        add_requirement(0,0,0,2);
        add_requirement(0,0,1,1);
        add_activity(0,1,3,&a01[0],6,8);
        add_requirement(0,1,0,1);

        t1 = "t1", a10 = "act1,0";
        add_train(1,0,35,&t1[0]);
        add_activity(1,0,12,&a10[0],0,23);
        add_requirement(1,0,1,1);
    }
    virtual void TearDown() {
        //Redirect stdout back to itself
        std::cout.rdbuf(sbuf);
        delete tmsp;
    }
};

TEST_F(ChainingTest, InitializeActivitiesTest) {
    initializeActivities();
    EXPECT_EQ(3,getActivities().size());
}

TEST_F(ChainingTest, SortActivitiesTest) {
    sort(activities.begin(), activities.end(), compareEST);
    EXPECT_LE(activities[0]->est,activities[1]->est);
    EXPECT_LE(activities[1]->est,activities[2]->est);
}

TEST_F(ChainingTest, InitializeChainsTest) {
    initializeChains();
    EXPECT_EQ(4,chains.size());
}

TEST_F(ChainingTest, SelectFirstChainTest) {
    initializeActivities();
    initializeChains();
    activity* act1 = A(0,0);
    chainId id1 = selectFirstChain(act1->i,act1->j,0);
    EXPECT_EQ(0,id1.resource);
    EXPECT_EQ(0,id1.unit);

    activity* act2 = A(0,1);
    chainId id2 = selectFirstChain(act2->i,act2->j,1);
    EXPECT_EQ(1,id2.resource);
    EXPECT_EQ(0,id2.unit);
}

TEST_F(ChainingTest, SelectFirstChainExceptionTest) {
    initializeActivities();
    initializeChains();
    chainId newId = {1,0};
    chain newChain = {};
    chains[newId] = newChain;

    activity* a1 = A(0,0);
    chainId id1 = selectFirstChain(a1->i,a1->j,1);
    chains[id1].activities.push_back(a1);
    activity* a2 = A(1,0);
    EXPECT_THROW(selectFirstChain(a2->i,a2->j,1),NoChainFoundException);
}

TEST_F(ChainingTest, SelectRandomChainTest) {
    initializeActivities();
    initializeChains();
    activity* act1 = A(0,0);
    chainId id1 = selectRandomChain(act1->i,act1->j,0);
    EXPECT_EQ(0,id1.resource);
    EXPECT_GE(2,id1.unit);
    EXPECT_LE(0,id1.unit);

    activity* act2 = A(0,1);
    chainId id2 = selectRandomChain(act2->i,act2->j,1);
    EXPECT_EQ(1,id2.resource);
    EXPECT_EQ(0,id2.unit);
}

TEST_F(ChainingTest, SelectRandomChainExceptionTest) {
    initializeActivities();
    initializeChains();
    chainId newId = {1,0};
    chain newChain = {};
    chains[newId] = newChain;

    activity* a1 = A(0,0);
    chainId id1 = selectRandomChain(a1->i,a1->j,1);
    chains[id1].activities.push_back(a1);
    activity* a2 = A(1,0);
    EXPECT_THROW(selectRandomChain(a2->i,a2->j,1),NoChainFoundException);
}

TEST_F(ChainingTest, PushToChainTest) {
    initializeActivities();
    initializeChains();
    stjn_construct();
    chainId id1 = {0,0};
    chainId id2 = {1,0};
    pushToChain(A(0,0),&id1);
    pushToChain(A(0,1),&id1);
    EXPECT_EQ(2,chains[id1].activities.size());
    precedence* prec = P(0);
    EXPECT_EQ(0,prec->i1);
    EXPECT_EQ(0,prec->i2);
    EXPECT_EQ(0,prec->j1);
    EXPECT_EQ(1,prec->j2);
}

TEST_F(ChainingTest, PushToBestChainTest) {
    initializeActivities();
    initializeChains();
    pushToBestChains(0,0,0);
    int count = 0;
    map<chainId, chain>::iterator it;
    for(it = chains.begin(); it != chains.end(); ++it) {
        chain c = it->second;
        count += (int) c.activities.size();
    }
    EXPECT_EQ(2,count);
}

TEST_F(ChainingTest, PrintChainTest) {
    initializeActivities();
    initializeChains();
    chainId id = {0,0};
    chains[id].activities.push_back(A(0,0));
    chains[id].activities.push_back(A(0,1));
    print_chain(0,0);
    EXPECT_EQ("CHAIN: 0 0 2 0 0 0 1 -1\n",buffer.str());
}

TEST_F(ChainingTest, AddFrameTest) {
    initializeActivities();
    initializeChains();
    add_frame();
    string exp = "STATE: 0 5 12 1 0 0 0 6 8 1 0 1 1 0 23 1 1 0 -1\n";
    EXPECT_EQ(exp,buffer.str());
}
