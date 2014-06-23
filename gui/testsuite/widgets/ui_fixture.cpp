#include  <gtest/gtest.h>

#include "controller/controller.h"
#include "controller/instancecontroller.h"
#include "widgets/mainwindow.h"

#include <QMdiSubWindow>

class UITest: public ::testing::Test {
protected:
    virtual void SetUp() {
        controller = new Controller();
        controller->showMainWindow();
        mainWindow = controller->getMainWindow();
    }
    virtual void TearDown() {
        if(QMdiSubWindow *tab = mainWindow->getCurrentInstanceMdiWidget()->currentSubWindow())
            tab->setWindowModified(false);
        mainWindow->close();
    }
    Controller *controller;
    MainWindow *mainWindow;
};

class UIInstanceTest: public UITest {
protected:
    virtual void SetUp() {
        UITest::SetUp();
        mainWindow->invokeTestMethod("newInstance");
        mainWindow->invokeTestMethod("loadFile", qApp->applicationDirPath()+"/resources/test.precedence.adding");

        instance = controller->getCurrentInstance();
        instanceWidget = controller->getCurrentInstanceWidget();
        a1 = controller->getCurrentInstanceController()->getActivityWidgets().at(0);
        a2 = controller->getCurrentInstanceController()->getActivityWidgets().at(1);
    }
    Instance *instance;
    InstanceWidget *instanceWidget;
    ActivityWidget *a1, *a2;
};
