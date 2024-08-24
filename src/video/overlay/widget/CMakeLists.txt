cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        widget.cpp
        string_widget.cpp
        text_align.cpp
    PUBLIC
        widget.h
        string_widget.h
        text_align.h
        color.h
)
