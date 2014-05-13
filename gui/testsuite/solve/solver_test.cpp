#include <gtest/gtest.h>
#include <QApplication>
#include <QProcess>
#include <QSignalSpy>
#include "solve/solver.h"
#include "solver_fixture.cpp"
#include "controller/exceptions.h"
#include "data/instance_reader.h"

Q_DECLARE_METATYPE(QProcess::ExitStatus)

TEST_F(SolverTest, testGetSetName) {
    ASSERT_EQ("Solver", s->getName());

    s->setName("test");
    ASSERT_EQ("test", s->getName());
}

TEST_F(SolverTest, testGetBinary) {
    ASSERT_EQ(filepath, s->getBinary());

    s->setBinary("solver");
    ASSERT_EQ("solver", s->getBinary());
}

TEST_F(SolverTest, testGetArguments) {
    ASSERT_EQ("-x", s->getArguments());

    s->setArguments("-m");
    ASSERT_EQ("-m", s->getArguments());
}

TEST_F(SolverTest, testSaveAndLoad) {
    s->save();
    Solver *s2 = Solver::load("Solver");
    ASSERT_EQ(filepath, s2->getBinary());
    ASSERT_EQ("-x", s2->getArguments());
}

TEST_F(SolverTest, testFailingLoad) {
    ASSERT_THROW(Solver::load("UnexistingSolver"), SolverNotFoundException);
}

TEST_F(SolverTest, testFinished) {
    qRegisterMetaType<QProcess::ExitStatus>("ExitStatus");
    QProcess* solveProcess = s->getProcess();
    QSignalSpy spyReading(solveProcess, SIGNAL(readyRead()));
    QSignalSpy spyFinished(solveProcess, SIGNAL(finished(int, QProcess::ExitStatus)));
    QSignalSpy spySolver(s, SIGNAL(finished(int, QProcess::ExitStatus)));

    EXPECT_NO_THROW({
        s->start(InstanceReader::load(qApp->applicationDirPath()+"/resources/test.instance"));
        ASSERT_TRUE(solveProcess->waitForFinished(10000));
    });

    ASSERT_GE(spyReading.count(), 2); // a lot of lines should have been processed
    ASSERT_EQ(1, spyFinished.count());
    ASSERT_EQ(1, spySolver.count());
    ASSERT_TRUE(s->isSolved());
}

TEST_F(SolverTest, testSecondSolveWithoutClearing) {
    QProcess* solveProcess = s->getProcess();
    QProcess* solveProcess2 = s2->getProcess();

    Instance* instance = InstanceReader::load(qApp->applicationDirPath()+"/resources/test.instance");
    ASSERT_FALSE(instance->hasSoftPrecedences());

    s->start(instance);
    ASSERT_TRUE(solveProcess->waitForFinished(10000));
    ASSERT_TRUE(s->isSolved());
    int precedences = instance->getSoftPrecedences().size();

    ASSERT_GT(precedences, 30);

    s2->start(instance);
    ASSERT_TRUE(solveProcess2->waitForFinished(10000));
    ASSERT_EQ(precedences, instance->getSoftPrecedences().size());
    ASSERT_TRUE(s2->isSolved());
}

TEST_F(SolverTest, testSecondSolveWithClearing) {
    QProcess* solveProcess = s->getProcess();
    QProcess* solveProcess2 = s2->getProcess();

    Instance* instance = InstanceReader::load(qApp->applicationDirPath()+"/resources/test.instance");
    s->start(instance);
    ASSERT_TRUE(solveProcess->waitForFinished(10000));
    ASSERT_TRUE(s->isSolved());

    ASSERT_TRUE(instance->hasSoftPrecedences());
    int precedences = instance->getHardPrecedences().size();

    instance->clearSoftPrecedences();
    ASSERT_FALSE(instance->hasSoftPrecedences());

    s2->start(instance);
    ASSERT_TRUE(solveProcess2->waitForFinished(10000));
    ASSERT_EQ(precedences, instance->getHardPrecedences().size());
    ASSERT_TRUE(s2->isSolved());
}
