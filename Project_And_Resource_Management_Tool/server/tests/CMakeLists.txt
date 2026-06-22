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
    ${SERVER_SRC}/services/src/AllocationService.cpp
    ${SERVER_SRC}/services/src/AuthService.cpp
    ${SERVER_SRC}/services/src/ProjectService.cpp
    ${SERVER_SRC}/services/src/ResourceService.cpp
    ${SERVER_SRC}/services/src/TimesheetService.cpp
    ${SERVER_SRC}/services/src/UserService.cpp
    ${SERVER_SRC}/security/src/JwtUtil.cpp
    ${SERVER_SRC}/security/src/PasswordUtil.cpp
    ${SERVER_SRC}/utils/src/ConfigLoader.cpp
)

# ── Test executable ────────────────────────────────────────────────────────────
add_executable(prm-server-tests
    AllocationServiceTests.cpp
    AuthServiceTests.cpp
    ProjectServiceTests.cpp
    ResourceServiceTests.cpp
    TimesheetServiceTests.cpp
    UserServiceTests.cpp
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
    OpenSSL::SSL
    OpenSSL::Crypto
)

target_include_directories(prm-server-tests PRIVATE
    ${SERVER_SRC}
    ${SERVER_SRC}/ai/inc
    ${SERVER_SRC}/dto
    ${SERVER_SRC}/email/inc
    ${SERVER_SRC}/models
    ${SERVER_SRC}/repositories/inc
    ${SERVER_SRC}/security/inc
    ${SERVER_SRC}/services/inc
    ${SERVER_SRC}/utils/inc
    C:/tools/msys64/mingw64/include   # gmock/gmock.h
)

target_compile_definitions(prm-server-tests PRIVATE JWT_DISABLE_PICOJSON)

# ── Coverage instrumentation ───────────────────────────────────────────────────
target_compile_options(prm-server-tests PRIVATE --coverage -O0)
target_link_options(prm-server-tests PRIVATE --coverage)

# ── Register with CTest ────────────────────────────────────────────────────────
include(GoogleTest)
gtest_add_tests(TARGET prm-server-tests
    SOURCES
        AllocationServiceTests.cpp
        AuthServiceTests.cpp
        ProjectServiceTests.cpp
        ResourceServiceTests.cpp
        TimesheetServiceTests.cpp
        UserServiceTests.cpp
)
