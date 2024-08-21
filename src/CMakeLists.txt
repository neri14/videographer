cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        arguments.cpp
        manager.cpp
    PUBLIC
        arguments.h
        manager.h
)

target_sources(vgraph
    PRIVATE
        main.cpp
)

add_subdirectory(telemetry)
add_subdirectory(utils)
add_subdirectory(video)
