cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_test
    PRIVATE
        argument_parser_test.cpp
)

add_subdirectory(logging)
