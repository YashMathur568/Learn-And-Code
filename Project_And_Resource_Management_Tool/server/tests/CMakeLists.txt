cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Locate GTest/GMock ─────────────────────────────────────────────────────────
find_package(GTest REQUIRED)

# GMock is shipped with GTest in the MSYS2 package but has no CMake target;
# locate the libraries manually.
find_library(GMOCK_LIB      NAMES gmock      PATHS C:/tools/msys64/mingw64/lib REQUIRED)
find_library(GMOCK_MAIN_LIB NAMES gmock_main PATHS C:/tools/msys64/mingw64/lib REQUIRED)

# ── Sources under test ─────────────────────────────────────────────────────────
set(SERVER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../src")

set(SOURCES_UNDER_TEST
    ${SERVER_SRC}/services/AllocationService.cpp
    ${SERVER_SRC}/services/EmployeeService.cpp
    ${SERVER_SRC}/services/TimesheetService.cpp
    ${SERVER_SRC}/utils/ConfigLoader.cpp
)

# ── Test executable ────────────────────────────────────────────────────────────
add_executable(prm-server-tests
    AllocationServiceTests.cpp
    EmployeeServiceTests.cpp
    TimesheetServiceTests.cpp
    ${SOURCES_UNDER_TEST}
)

# nlohmann_json::nlohmann_json and jwt-cpp::jwt-cpp are available from the
# parent FetchContent calls; link them directly.
target_link_libraries(prm-server-tests PRIVATE
    GTest::GTest
    GTest::Main
    ${GMOCK_LIB}
    ${GMOCK_MAIN_LIB}
    nlohmann_json::nlohmann_json
    jwt-cpp::jwt-cpp
)

target_include_directories(prm-server-tests PRIVATE
    ${SERVER_SRC}
    C:/tools/msys64/mingw64/include   # gmock/gmock.h
)

target_compile_definitions(prm-server-tests PRIVATE JWT_DISABLE_PICOJSON)

# ── Register with CTest ────────────────────────────────────────────────────────
include(GoogleTest)
gtest_add_tests(TARGET prm-server-tests
    SOURCES
        AllocationServiceTests.cpp
        EmployeeServiceTests.cpp
        TimesheetServiceTests.cpp
)
