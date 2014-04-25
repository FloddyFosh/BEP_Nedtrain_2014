#include  <gtest/gtest.h>
#include "ui_fixture.cpp"
#include "controller/exceptions.h"
#include <QTest>

TEST_F(UITest, NoInstance) {
    ASSERT_THROW(controller->getCurrentInstance(), NoInstanceException);
}

TEST_F(UITest, LoadInstance) {
    mainWindow->invokeTestMethod("loadFile", qApp->applicationDirPath()+"/resources/test.instance");
    Instance *instance = 0;
    ASSERT_NO_THROW({
        instance = controller->getCurrentInstance();
    });
    EXPECT_EQ(5, instance->getJobs().size());
    EXPECT_EQ(5, instance->getResources().size());
}

TEST_F(UITest, GetAllInstances) {
    mainWindow->invokeTestMethod("loadFile", qApp->applicationDirPath()+"/resources/test.instance");
    mainWindow->invokeTestMethod("loadFile", qApp->applicationDirPath()+"/resources/precedences.instance");
    QList<Instance*> instances = controller->getAllInstances();
    EXPECT_EQ(qApp->applicationDirPath()+"/resources/test.instance", instances[0]->getFileName());
    EXPECT_EQ(qApp->applicationDirPath()+"/resources/precedences.instance", instances[1]->getFileName());
}

TEST_F(UITest, CloseInstance) {
    mainWindow->invokeTestMethod("loadFile", qApp->applicationDirPath()+"/resources/test.instance");
    mainWindow->invokeTestMethod("closeSubWindow");
    ASSERT_THROW(controller->getCurrentInstance(), NoInstanceException);
}

TEST_F(UITest, NewInstance) {
    ASSERT_THROW(controller->getCurrentInstance(), NoInstanceException);
    mainWindow->invokeTestMethod("newInstance");
    ASSERT_NO_THROW(controller->getCurrentInstance());
}

TEST_F(UIInstanceTest, EnablePrecedenceEditing) {
    EXPECT_FALSE(instanceWidget->isPrecedenceEditing());
    EXPECT_FALSE(instanceWidget->hasAddedFirstPrecedencePoint());

    controller->getCurrentInstanceWidget()->enablePrecedenceAdding();

    EXPECT_TRUE(instanceWidget->isPrecedenceEditing());
    EXPECT_FALSE(instanceWidget->hasAddedFirstPrecedencePoint());
}

TEST_F(UIInstanceTest, EnablePrecedenceEditing_FirstSelect) {
    // given
    controller->getCurrentInstanceWidget()->enablePrecedenceAdding();
    // when
    QTest::mouseClick(a1, Qt::LeftButton);
    // then
    EXPECT_TRUE(instanceWidget->isPrecedenceEditing());
    EXPECT_TRUE(instanceWidget->hasAddedFirstPrecedencePoint());
}

TEST_F(UIInstanceTest, EnablePrecedenceEditing_SecondSelect) {
    // given
    controller->getCurrentInstanceWidget()->enablePrecedenceAdding();
    // when
    QTest::mouseClick(a1, Qt::LeftButton);
    QTest::mouseClick(a2, Qt::LeftButton);
    // then
    EXPECT_FALSE(instanceWidget->isPrecedenceEditing());
    EXPECT_FALSE(instanceWidget->hasAddedFirstPrecedencePoint());
}

TEST_F(UIInstanceTest, EnablePrecedenceEditing_AddPrecedence) {
    // given
    controller->getCurrentInstanceWidget()->enablePrecedenceAdding();
    // when
    QTest::mouseClick(a1, Qt::LeftButton);
    QTest::mouseClick(a2, Qt::LeftButton);
    // then
    EXPECT_TRUE(instance->precedenceExists(instance->A(0,0), instance->A(0,1)));
    EXPECT_FALSE(instance->precedenceExists(instance->A(0,1), instance->A(0,0)));
}
