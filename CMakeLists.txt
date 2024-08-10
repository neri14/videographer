cmake_minimum_required(VERSION 3.20.0)
project(VideoGrapher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# define main static library and executable
add_library(vgraph_lib STATIC)
add_executable(vgraph)

# load source files
add_subdirectory(src)

# includes path
target_include_directories(vgraph_lib PUBLIC src)

# add garmin sdk library directory
add_subdirectory(3rdparty/garmin_fit_sdk)

# link libraries to executable
target_link_libraries(vgraph PRIVATE vgraph_lib garmin)


## TESTING

# define test executable
add_executable(vgraph_test)

# add google test library directory
add_subdirectory(3rdparty/googletest)

# add test source files
add_subdirectory(test)

#includes path
target_include_directories(vgraph_test PRIVATE test)

# link libraries to test executable
target_link_libraries(vgraph_test PRIVATE GTest::gmock vgraph_lib garmin)

# setup gtest
include(GoogleTest)
gtest_discover_tests(vgraph_test)
