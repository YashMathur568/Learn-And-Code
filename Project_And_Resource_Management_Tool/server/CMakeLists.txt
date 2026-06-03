cmake_minimum_required(VERSION 3.16)
project(PrmServer LANGUAGES CXX)

find_package(Drogon CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

find_path(MYSQL_CPPCONN_INCLUDE_DIR
    NAMES cppconn/connection.h
    PATHS
        /usr/include
        /usr/local/include
        /usr/include/mysql-connector-c++
        /usr/local/include/mysql-connector-c++
)

find_library(MYSQL_CPPCONN_LIBRARY
    NAMES mysqlcppconn
    PATHS
        /usr/lib
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
)

if(NOT MYSQL_CPPCONN_INCLUDE_DIR OR NOT MYSQL_CPPCONN_LIBRARY)
    message(FATAL_ERROR
        "MySQL Connector/C++ not found.\n"
        "Install with: sudo apt install libmysqlcppconn-dev"
    )
endif()

include(FetchContent)

FetchContent_Declare(
    jwt-cpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG        v0.7.0
)
set(JWT_BUILD_EXAMPLES   OFF CACHE BOOL "" FORCE)
set(JWT_DISABLE_PICOJSON ON  CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(jwt-cpp)

file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(prm-server ${SERVER_SOURCES})

target_include_directories(prm-server
    PRIVATE
        src
        ${MYSQL_CPPCONN_INCLUDE_DIR}
)

target_link_libraries(prm-server
    PRIVATE
        Drogon::Drogon
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
        ${MYSQL_CPPCONN_LIBRARY}
        jwt-cpp::jwt-cpp
        crypt
)
