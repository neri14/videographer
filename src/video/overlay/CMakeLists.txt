cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        text_align.cpp
        overlay.cpp
        widget.cpp
        string_widget.cpp
    PUBLIC
        color.h
        text_align.h
        overlay.h
        widget.h
        string_widget.h
)
