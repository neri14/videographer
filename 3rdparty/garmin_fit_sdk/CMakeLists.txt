cmake_minimum_required(VERSION 3.20.0)
project(GarminSDK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB garmin_src
     "${PROJECT_SOURCE_DIR}/*.hpp"
     "${PROJECT_SOURCE_DIR}/*.cpp"
)
file(GLOB garmin_plugins_src
     "${PROJECT_SOURCE_DIR}/plugins/*.hpp"
     "${PROJECT_SOURCE_DIR}/plugins/*.cpp"
)

add_library(garmin STATIC ${garmin_src})
target_include_directories(garmin PUBLIC .)
