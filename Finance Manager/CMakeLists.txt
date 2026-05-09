cmake_minimum_required(VERSION 3.16)
project(FinanceManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SQLite3 REQUIRED)

include_directories(inc interfaces)

file(GLOB SOURCES "src/*.cpp")

add_executable(finance_manager main.cpp ${SOURCES})

target_link_libraries(finance_manager SQLite3::SQLite3)
