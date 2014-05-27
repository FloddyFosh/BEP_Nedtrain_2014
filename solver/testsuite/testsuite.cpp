#include  <gtest/gtest.h>

// Include test cases that need to be run
#include "constraints_test.cpp"
#include "flexibility_test.cpp"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

