#include  <gtest/gtest.h>

// Include test cases that need to be run
#include "tests/chaining_test.cpp"
#include "tests/constraints_test.cpp"
// #include "tests/flexibility_test.cpp"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

