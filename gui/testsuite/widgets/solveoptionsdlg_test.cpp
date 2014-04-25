#include  <gtest/gtest.h>
#include "widgets/dialogs/solvewithoptionsdialog.h"

TEST(SolveOptionsDlg, testEmpty) {
    Solver *s = new Solver("","","");
    Controller *controller = new Controller();
    SolveWithOptionsDialog *dialog = new SolveWithOptionsDialog(s, controller);

    EXPECT_EQ("", dialog->createArguments());
    delete dialog;
    delete controller;
}

TEST(SolveOptionsDlg, testMutex) {
    Solver *s = new Solver("","","-x");
    Controller *controller = new Controller();
    SolveWithOptionsDialog *dialog = new SolveWithOptionsDialog(s, controller);

    EXPECT_EQ("-x", dialog->createArguments());
    delete dialog;
    delete controller;
}

TEST(SolveOptionsDlg, testGrouping1) {
    Solver *s = new Solver("","","-m4");
    Controller *controller = new Controller();
    SolveWithOptionsDialog *dialog = new SolveWithOptionsDialog(s, controller);

    EXPECT_EQ("-m 4", dialog->createArguments());
    delete dialog;
    delete controller;
}

TEST(SolveOptionsDlg, testGrouping2) {
    Solver *s = new Solver("","","-m 4");
    Controller *controller = new Controller();
    SolveWithOptionsDialog *dialog = new SolveWithOptionsDialog(s, controller);

    EXPECT_EQ("-m 4", dialog->createArguments());
    delete dialog;
    delete controller;
}

TEST(SolveOptionsDlg, testMutexAndGrouping) {
    Solver *s = new Solver("","","-m 4 -x");
    Controller *controller = new Controller();
    SolveWithOptionsDialog *dialog = new SolveWithOptionsDialog(s, controller);

    EXPECT_EQ("-x -m 4", dialog->createArguments());
    delete dialog;
    delete controller;
}
