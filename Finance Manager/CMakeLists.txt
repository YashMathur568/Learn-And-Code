cmake_minimum_required(VERSION 3.16)
project(FinanceManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    include(FetchContent)
    FetchContent_Declare(
        sqlite3
        URL https://www.sqlite.org/2024/sqlite-amalgamation-3450300.zip
        SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/sqlite3-src
    )
    FetchContent_MakeAvailable(sqlite3)
    
    set(SQLITE3_INCLUDE_DIR ${CMAKE_BINARY_DIR}/_deps/sqlite3-src)
    add_library(sqlite3 STATIC ${CMAKE_BINARY_DIR}/_deps/sqlite3-src/sqlite3.c)
    target_include_directories(sqlite3 PUBLIC ${SQLITE3_INCLUDE_DIR})
    set(SQLite3_LIBRARIES sqlite3)
else()
    find_package(SQLite3 REQUIRED)
endif()

include_directories(inc interfaces)

file(GLOB SOURCES "src/*.cpp")

add_executable(finance_manager main.cpp ${SOURCES})

target_link_libraries(finance_manager ${SQLite3_LIBRARIES})
