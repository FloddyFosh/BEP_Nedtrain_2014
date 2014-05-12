#include <gtest/gtest.h>
#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QFile>

/**
 * AppEnvironment sets up the testing environment,
 * so that relative file paths can be used.
 * It also makes sure that settings are cleared.
 */
class AppEnvironment: public ::testing::Environment {
    QApplication *app;
    int argc;
    char **argv;
 public:
  virtual ~AppEnvironment() {}
  virtual void setParams(int argc, char **argv) {
      this->argc = argc;
      this->argv = argv;
  }
  virtual void SetUp() {
      app = new QApplication(argc, argv);
      app->setOrganizationName("TU Delft");
      app->setApplicationName("Task scheduler Tester");

      QDir().mkdir(qApp->applicationDirPath()+"/resources");
      QFile::copy(":/resources/resources/j1201_1.instance",qApp->applicationDirPath()+"/resources/j1201_1.instance");
      QFile::copy(":/resources/resources/precedences.instance",qApp->applicationDirPath()+"/resources/precedences.instance");
      QFile::copy(":/resources/resources/small.instance",qApp->applicationDirPath()+"/resources/small.instance");
      QFile::copy(":/resources/resources/test.instance",qApp->applicationDirPath()+"/resources/test.instance");
      QFile::copy(":/resources/resources/test.precedence.adding",qApp->applicationDirPath()+"/resources/test.precedence.adding");
  }
  virtual void TearDown() {
      QSettings settings;
      settings.clear();
      delete app;
  }
};
