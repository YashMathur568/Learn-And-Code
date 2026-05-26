cmake_minimum_required(VERSION 3.16)
project(FinanceManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

if(WIN32)
    FetchContent_Declare(
        sqlite3
        URL https://www.sqlite.org/2024/sqlite-amalgamation-3450300.zip
        SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/sqlite3-src
    )
    FetchContent_MakeAvailable(sqlite3)

    set(SQLITE3_INCLUDE_DIR ${CMAKE_BINARY_DIR}/_deps/sqlite3-src)
    add_library(sqlite3_lib STATIC ${CMAKE_BINARY_DIR}/_deps/sqlite3-src/sqlite3.c)
    target_include_directories(sqlite3_lib PUBLIC ${SQLITE3_INCLUDE_DIR})
    set(SQLite3_LIBRARIES sqlite3_lib)
else()
    find_package(SQLite3 REQUIRED)
endif()

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(inc interfaces)

file(GLOB SOURCES "src/*.cpp")

add_executable(finance_manager main.cpp ${SOURCES})
target_link_libraries(finance_manager ${SQLite3_LIBRARIES})

enable_testing()

set(INTERACTOR_SOURCES
    src/RegisterInteractor.cpp
    src/LoginInteractor.cpp
    src/LogoutInteractor.cpp
    src/ValidateTokenInteractor.cpp
    src/AddExpenseInteractor.cpp
    src/GetExpensesInteractor.cpp
    src/FilterExpensesByCategoryInteractor.cpp
    src/FilterExpensesByDateInteractor.cpp
    src/DeleteExpenseInteractor.cpp
    src/AddIncomeInteractor.cpp
    src/GetIncomesInteractor.cpp
    src/SetBudgetInteractor.cpp
    src/TrackBudgetInteractor.cpp
    src/GetSummaryInteractor.cpp
    src/SQLiteConnection.cpp
    src/SQLiteUserRepository.cpp
    src/SQLiteSessionRepository.cpp
    src/SQLiteExpenseRepository.cpp
    src/SQLiteIncomeRepository.cpp
    src/SQLiteBudgetRepository.cpp
)

file(GLOB TEST_SOURCES "tests/*.cpp")

add_executable(finance_tests ${TEST_SOURCES} ${INTERACTOR_SOURCES})
target_include_directories(finance_tests PRIVATE inc interfaces tests/mocks)
target_link_libraries(finance_tests GTest::gtest_main ${SQLite3_LIBRARIES})

include(GoogleTest)
gtest_discover_tests(finance_tests)
