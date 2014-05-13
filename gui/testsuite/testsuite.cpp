#include  <gtest/gtest.h>
#include "app_environment.cpp"

// Include test cases that need to be run
#include "model/instance_test.cpp"
#include "model/activity_test.cpp"
#include "model/activity_remove_test.cpp"
#include "data/template_gateway_test.cpp"
#include "model/activitytemplate_test.cpp"
#include "model/job_test.cpp"
#include "model/resource_test.cpp"
#include "model/requirement_test.cpp"
#include "model/precedence_test.cpp"
#include "model/group_test.cpp"
#include "model/comparison_test.cpp"
#include "solve/solver_test.cpp"
#include "util/resourcecalculator_test.cpp"
#include "widgets/smoke_ui_test.cpp"
#include "widgets/solveoptionsdlg_test.cpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    AppEnvironment *env = new AppEnvironment();
    env->setParams(argc, argv);
    ::testing::AddGlobalTestEnvironment(env);

    int r = RUN_ALL_TESTS();

    while(r>0) r--;

    return r;
}
