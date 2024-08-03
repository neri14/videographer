cmake_minimum_required(VERSION 3.20.0)
project(VideoGrapher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# static library
add_library(vgraph_lib STATIC)
target_include_directories(vgraph_lib PUBLIC src)

# executable
add_executable(vgraph)

add_subdirectory(3rdparty/garmin_fit_sdk)
target_link_libraries(vgraph PRIVATE vgraph_lib garmin)

# source subdirectory
add_subdirectory(src)

# UNIT TESTING
add_executable(vgraph_test)

add_subdirectory(3rdparty/googletest)

target_link_libraries(vgraph_test PRIVATE GTest::gmock_main vgraph_lib)

# tests subdirectory
add_subdirectory(test)

# gtest setup
include(GoogleTest)
gtest_discover_tests(vgraph_test)
