cmake_minimum_required(VERSION 3.20.0)
project(VideoGrapher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# static library
add_library(vgraph_lib STATIC)
set_property(TARGET vgraph_lib PROPERTY CXX_STANDARD 23)
target_include_directories(vgraph_lib PUBLIC src)

# executable
add_executable(vgraph)
set_property(TARGET vgraph PROPERTY CXX_STANDARD 23)
target_link_libraries(vgraph PRIVATE vgraph_lib)

# source subdirectory
add_subdirectory(src)

# UNIT TESTING
add_subdirectory(3rdparty/googletest)

add_executable(vgraph_test)
target_link_libraries(vgraph_test PRIVATE GTest::gmock_main vgraph_lib)

# tests subdirectory
add_subdirectory(test)

# gtest setup
include(GoogleTest)
gtest_discover_tests(vgraph_test)
