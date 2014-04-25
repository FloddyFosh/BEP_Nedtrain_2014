#include  <gtest/gtest.h>
#include "model/comparison.h"

using ::testing::Test;
using ::testing::WithParamInterface;
using ::std::tr1::tuple;
using ::std::tr1::get;

class ComparisonTest: public Test {
protected:
    virtual void SetUp() {
        i1 = new Instance();
        i2 = new Instance();
        comparison = 0;
    }
    virtual void TearDown() {
        if(comparison)
            delete comparison;
        delete i1;
        delete i2;
    }
    Comparison *comparison;
    Instance *i1, *i2;
};

class CompareJobParamTest: public ComparisonTest, public WithParamInterface< tuple<int, int, int, const char*, bool> > {
protected:
    virtual void SetUp() {
        ComparisonTest::SetUp();
        id = get<0>(GetParam());
        releaseDate = get<1>(GetParam());
        dueDate = get<2>(GetParam());
        name = get<3>(GetParam());
        expectException = get<4>(GetParam());
    }
    int id, releaseDate, dueDate;
    QString name;
    bool expectException;
};

class CompareActivityParamTest: public ComparisonTest, public WithParamInterface< tuple<int, int, int, int, const char*, bool> > {
protected:
    virtual void SetUp() {
        ComparisonTest::SetUp();
        id = get<0>(GetParam());
        est = get<1>(GetParam());
        lst = get<2>(GetParam());
        duration = get<3>(GetParam());
        name = get<4>(GetParam());
        expectException = get<5>(GetParam());
    }
    int id, est, lst, duration;
    QString name;
    bool expectException;
};

class CompareResourceParamTest: public ComparisonTest, public WithParamInterface< tuple<int, int, const char*, bool> > {
protected:
    virtual void SetUp() {
        ComparisonTest::SetUp();
        id = get<0>(GetParam());
        capacity = get<1>(GetParam());
        name = get<2>(GetParam());
        expectException = get<3>(GetParam());
    }
    int id, capacity;
    QString name;
    bool expectException;
};
