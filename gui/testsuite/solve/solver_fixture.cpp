#include  <gtest/gtest.h>
#include <QFile>
#include "solve/solver.h"

class SolverTest: public ::testing::Test {
private:
    bool failed;
protected:
    virtual void SetUp() {
        failed = false;
        QString binary = qApp->applicationDirPath()+"/resources/tms";
        QFile f(binary);
        if(!f.exists()) {
            failed = true;
            FAIL() << "The testing environment requires the tms solver to be at /resources/tms.";
        } else {
            s = new Solver("Solver", binary, "-x");
            s2 = new Solver("Solver", binary, "-x");
        }
    }
    virtual void TearDown() {
        if(!failed) {
            delete s;
            delete s2;
        }
    }
    Solver *s, *s2;
};
