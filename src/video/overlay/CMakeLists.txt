cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        overlay.cpp
    PUBLIC
        overlay.h
)

add_subdirectory(widget)
