cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        generator.cpp
        pipeline.cpp
        overlay_drawer.cpp
    PUBLIC
        generator.h
        pipeline.h
        overlay_drawer.h
)
