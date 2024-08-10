cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_test
    PRIVATE
        main.cpp
)

add_subdirectory(telemetry)
add_subdirectory(testdata)
add_subdirectory(testutils)
add_subdirectory(utils)
