cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        generator.cpp
    PUBLIC
        generator.h
)
