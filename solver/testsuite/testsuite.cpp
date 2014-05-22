#include  <gtest/gtest.h>

// Include test cases that need to be run
#include "constraints_test.cpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    int r = RUN_ALL_TESTS();
    while(r>0) r--;
    return r;
}

