cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        argument_parser.cpp
        geo.cpp
    PUBLIC
        argument_parser.h
        geo.h
)

add_subdirectory(logging)
