cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph
    PRIVATE
        main.cpp
        arguments.cpp
        manager.cpp
    PUBLIC
        arguments.h
        manager.h
)

add_subdirectory(telemetry)
add_subdirectory(utils)
