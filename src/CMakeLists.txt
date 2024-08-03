cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph
    PRIVATE
        main.cpp
)
target_sources(vgraph_lib
    PRIVATE
        greeter.cpp
    PUBLIC
        greeter.h
)
