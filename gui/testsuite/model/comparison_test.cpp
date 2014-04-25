#include  <gtest/gtest.h>
#include "model/comparison.h"
#include "model/comparison_fixture.cpp"

using ::testing::Values;
using ::testing::Combine;
using ::std::tr1::make_tuple;

TEST_F(ComparisonTest, testInit) {
    ASSERT_NO_THROW(comparison = new Comparison(i1, i2));

    EXPECT_EQ(i1, comparison->getFirstInstance());
    EXPECT_EQ(i2, comparison->getSecondInstance());
}

TEST_P(CompareJobParamTest, testJobsCompatibility) {
    i1->addJob(0, 0, 0, "Job");
    if(id > -1)
        i2->addJob(id, releaseDate, dueDate, name);
    if(expectException)
        ASSERT_THROW(comparison = new Comparison(i1, i2), ComparisonException);
    else
        ASSERT_NO_THROW(comparison = new Comparison(i1, i2));
}

INSTANTIATE_TEST_CASE_P(Parameters, CompareJobParamTest, Values(
                                // expect exception?
    make_tuple(0, 0, 0, "Job", false), // add the same
    make_tuple(-1, -1, -1, "", true),  // don't add any
    make_tuple(0, 0, 0, "Job2", true),
    make_tuple(1, 0, 0, "Job", true),
    make_tuple(0, 1, 0, "Job", true),
    make_tuple(0, 0, 1, "Job", true)
));

TEST_P(CompareActivityParamTest, testActivityCompatibility) {
    i1->addJob(0, 1, 10, "Job");
    i2->addJob(0, 1, 10, "Job");

    i1->addActivity(0, 0, 0, 1, 2, "A", 0);
    if(id > -1)
        i2->addActivity(0, id, est, lst, duration, name , 0);

    if(expectException)
        ASSERT_THROW(comparison = new Comparison(i1, i2), ComparisonException);
    else
        ASSERT_NO_THROW(comparison = new Comparison(i1, i2));
}

INSTANTIATE_TEST_CASE_P(Parameters, CompareActivityParamTest, Values(
                                // expect exception?
    make_tuple(0, 0, 1, 2, "A", false),   // add the same
    make_tuple(-1, -1, -1, -1, "", true), // don't add any
    make_tuple(1, 0, 1, 2, "A", true),
    make_tuple(0, 1, 1, 2, "A", false),
    make_tuple(0, 0, 2, 2, "A", false),
    make_tuple(0, 0, 1, 3, "A", true),
    make_tuple(0, 0, 1, 2, "B", true)
));

TEST_P(CompareResourceParamTest, testResourceCompatibility) {
    i1->addResource(0, 1, "R");
    if(id > -1)
        i2->addResource(id, capacity, name);
    if(expectException)
        ASSERT_THROW(comparison = new Comparison(i1, i2), ComparisonException);
    else
        ASSERT_NO_THROW(comparison = new Comparison(i1, i2));
}

INSTANTIATE_TEST_CASE_P(Parameters, CompareResourceParamTest, Values(
                                // expect exception?
    make_tuple(0, 1, "R", false), // add the same
    make_tuple(-1, -1, "", true), // don't add any
    make_tuple(1, 1, "R", true),
    make_tuple(0, 0, "R", true),
    make_tuple(0, 1, "R2", true)
));

TEST_F(ComparisonTest, testActivityDifferences) {
    i1->addJob(0, 1, 10, "Job");
    i2->addJob(0, 1, 10, "Job");
    i1->addActivity(0, 1, 0, 2, 2, "A", 0); // est = 0
    i2->addActivity(0, 1, 0, 2, 2, "A", 0);
    Group *g1 = new Group(0, 2, 0, 0);
    Group *g2 = new Group(0, 2, 0, 0);
    g1->addActivity(i1->A(0,1));
    g2->addActivity(i2->A(0,1));
    comparison = new Comparison(i1, i2);

    Activity *a = i1->A(0,1);
    EXPECT_FALSE(comparison->hasDifferences(a));

    g1->setST(1);
    EXPECT_TRUE(comparison->hasDifferences(a));
}
