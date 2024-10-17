cmake_minimum_required(VERSION 3.20)
project(match_simu VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
add_executable(server server.cpp)
add_executable(client client.cpp)

find_package(libhv CONFIG REQUIRED)
target_link_libraries(server PRIVATE hv_static)
target_link_libraries(client PRIVATE hv_static)

find_package(flatbuffers CONFIG REQUIRED)
target_link_libraries(server PRIVATE flatbuffers::flatbuffers)
target_link_libraries(client PRIVATE flatbuffers::flatbuffers)

option(BUILD_TESTS "Build test programs" ON)

if(BUILD_TESTS)
    add_executable(simu_matcher_test simu_matcher_test.cpp)
endif()