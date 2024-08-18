cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        args.cpp
        argument_parser.cpp
    PUBLIC
        args.h
        argument_parser.h
)

add_subdirectory(logging)
