cmake_minimum_required(VERSION 2.6)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Include some directories for the Clp and CoinUtils libs
include_directories(../../coin-Clp/include/coin)
include_directories(../../coin-Clp/CoinUtils/src)
link_directories(../../coin-Clp/lib)

# Add the source files are going to be tested to this list:
add_executable(testsuite testsuite.cpp 
    ../src/exceptions.cpp
    ../src/constraints.cpp
    ../src/flexibility.cpp
    ../src/tmsp.cpp
    ../src/exceptions.cpp
    ../src/debug.cpp)

# Link testsuite with GTest, pthread, Clp en CoinUtils
target_link_libraries(testsuite ${GTEST_LIBRARIES} pthread Clp CoinUtils z)
