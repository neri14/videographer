cmake_minimum_required(VERSION 3.23.0)
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

# find gstreamer libraries
find_package(PkgConfig REQUIRED)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0)
pkg_search_module(Cairo REQUIRED IMPORTED_TARGET cairo)
pkg_search_module(PangoCairo REQUIRED IMPORTED_TARGET pangocairo)
pkg_search_module(pugixml REQUIRED IMPORTED_TARGET pugixml)

# link libraries to vgraph_lib
target_link_libraries(vgraph_lib PUBLIC
                      PkgConfig::gstreamer
                      PkgConfig::gstreamer-video
                      PkgConfig::Cairo
                      PkgConfig::PangoCairo
                      PkgConfig::pugixml
                      garmin
                      )

# link vgraph_lib to vgraph executable
target_link_libraries(vgraph PRIVATE
                      vgraph_lib
                      )

# compile in version string
execute_process(COMMAND git describe --tags OUTPUT_VARIABLE VERSION_STR)
target_compile_definitions(vgraph_lib PRIVATE VERSION_STR="${VERSION_STR}")

## TESTING

# define test executable
add_executable(vgraph_test)

# find googletest
find_package(GTest REQUIRED)

# add test source files
add_subdirectory(test)

#includes path
target_include_directories(vgraph_test PRIVATE test)

# link libraries to test executable
target_link_libraries(vgraph_test PRIVATE GTest::gmock vgraph_lib garmin)

# setup gtest
include(GoogleTest)
gtest_discover_tests(vgraph_test)
