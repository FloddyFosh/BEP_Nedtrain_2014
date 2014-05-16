#include  <gtest/gtest.h>
#include <QFile>
#include "solve/solver.h"

class ChainingTest: public ::testing::Test {
private:
    bool failed;
protected:
    virtual void SetUp() {
        failed = false;
        filepath = qApp->applicationDirPath()+"/resources/solver";
        QFile f(filepath);

        if(!f.exists()) {
            //Might be windows
            filepath += ".exe";
        }

        QFile fw(filepath);

        if(!fw.exists()) {
            failed = true;
            FAIL() << "The testing environment requires the solver to be at /resources/.";
        } else {
            s = new Solver("Solver", filepath, "-x");
            s2 = new Solver("Solver", filepath, "-x");
        }
    }
    virtual void TearDown() {
        if(!failed) {
            delete s;
            delete s2;
        }
    }
    Solver *s, *s2;
    QString filepath;
};
