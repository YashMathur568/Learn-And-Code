cmake_minimum_required(VERSION 3.14)
project(DivisorPairs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_library(divisor_pairs_lib
    src/DivisorCounter.cpp
    src/DivisorPairCounter.cpp
)
target_include_directories(divisor_pairs_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
)

add_executable(divisor_pairs main.cpp)
target_link_libraries(divisor_pairs divisor_pairs_lib)

enable_testing()

add_executable(divisor_pairs_tests
    tests/DivisorCounterTest.cpp
    tests/DivisorPairCounterTest.cpp
)
target_include_directories(divisor_pairs_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
)
target_link_libraries(divisor_pairs_tests divisor_pairs_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(divisor_pairs_tests)
