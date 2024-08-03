cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        file.cpp
        fit_file.cpp
        track.cpp
    PUBLIC
        file.h
        fit_file.h
        track.h
)
