cmake_minimum_required(VERSION 3.16)
project(PrmClient LANGUAGES CXX)

find_package(CURL REQUIRED)

file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(prm-client ${CLIENT_SOURCES})

target_include_directories(prm-client
    PRIVATE
        src
)

target_link_libraries(prm-client
    PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
)
