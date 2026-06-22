cmake_minimum_required(VERSION 3.16)
project(PrmServer LANGUAGES CXX)

# Allow pointing to a custom MySQL Connector/C++ installation:
#   cmake -DMYSQL_CONNECTOR_ROOT=C:/tools/mysql-connector-cpp ...
if(NOT MYSQL_CONNECTOR_ROOT AND DEFINED ENV{MYSQL_CONNECTOR_ROOT})
    set(MYSQL_CONNECTOR_ROOT "$ENV{MYSQL_CONNECTOR_ROOT}")
endif()

# Add MSYS2 MINGW64 and common Windows paths to CMake search paths
if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH
        "C:/tools/msys64/mingw64"
        "${MYSQL_CONNECTOR_ROOT}"
    )
endif()

find_package(Drogon CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

# Shim headers redirect cppconn/ API to MariaDB Connector/C++
find_path(MYSQL_CPPCONN_INCLUDE_DIR
    NAMES cppconn/connection.h
    PATHS
        C:/tools/mysql-shim/include
        /c/tools/mysql-shim/include
        ${MYSQL_CONNECTOR_ROOT}/include
        /usr/include
        /usr/local/include
        /usr/include/mysql-connector-c++
        /usr/local/include/mysql-connector-c++
)

# Static MariaDB Connector/C++ (built from source with MinGW)
find_library(MYSQL_CPPCONN_LIBRARY
    NAMES mariadbcpp mysqlcppconn mysqlcppconn8
    PATHS
        C:/tools/msys64/mingw64/lib/mariadb
        /mingw64/lib/mariadb
        ${MYSQL_CONNECTOR_ROOT}/lib64
        ${MYSQL_CONNECTOR_ROOT}/lib
        /usr/lib
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
)

# MariaDB C connector (needed by the C++ connector static lib)
find_library(MARIADB_C_LIBRARY
    NAMES mariadbclient mariadb
    PATHS
        C:/tools/msys64/mingw64/lib
        /mingw64/lib
        /usr/lib
        /usr/local/lib
)

if(NOT MYSQL_CPPCONN_INCLUDE_DIR OR NOT MYSQL_CPPCONN_LIBRARY)
    message(FATAL_ERROR
        "MySQL/MariaDB Connector/C++ not found.\n"
        "Linux:   sudo apt install libmysqlcppconn-dev\n"
        "Windows: Build mariadb-connector-cpp from source and install to mingw64\n"
        "         (see MYSQL_CPPCONN_INCLUDE_DIR and MYSQL_CPPCONN_LIBRARY hints above)"
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

find_package(CURL REQUIRED)

file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(prm-server ${SERVER_SOURCES})

target_include_directories(prm-server
    PRIVATE
        src
        src/ai/inc
        src/controllers/inc
        src/dto
        src/email/inc
        src/models
        src/repositories/inc
        src/security/inc
        src/services/inc
        src/utils/inc
        ${MYSQL_CPPCONN_INCLUDE_DIR}
        C:/tools/msys64/mingw64/include
)

target_link_libraries(prm-server
    PRIVATE
        Drogon::Drogon
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
        ${MYSQL_CPPCONN_LIBRARY}
        ${MARIADB_C_LIBRARY}
        jwt-cpp::jwt-cpp
        CURL::libcurl
        bcrypt
        zstd
)

# Copy config.json to the build/server directory automatically
file(COPY "${CMAKE_SOURCE_DIR}/config.json" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# Create logs directory for Drogon
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")

# ── Unit tests ─────────────────────────────────────────────────────────────────
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
