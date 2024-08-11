cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        args.cpp
    PUBLIC
        args.h
)

add_subdirectory(logging)
