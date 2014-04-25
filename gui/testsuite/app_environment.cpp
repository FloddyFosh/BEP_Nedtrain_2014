#include  <gtest/gtest.h>
#include <QApplication>
#include <QSettings>

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
  }
  virtual void TearDown() {
      QSettings settings;
      settings.clear();
      delete app;
  }
};
