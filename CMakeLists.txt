cmake_minimum_required(VERSION 3.20)
project(match_simu VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
add_executable(match_simu main.cpp)

find_package(libhv CONFIG REQUIRED)
target_link_libraries(match_simu PRIVATE hv_static)

find_package(flatbuffers CONFIG REQUIRED)
target_link_libraries(match_simu PRIVATE flatbuffers::flatbuffers)