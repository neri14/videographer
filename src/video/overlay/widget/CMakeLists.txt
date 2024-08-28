cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        widget.cpp
        text_widget.cpp
        string_widget.cpp
        value_widget.cpp
        timestamp_widget.cpp
        timecode_widget.cpp
        text_align.cpp
        color.cpp
    PUBLIC
        widget.h
        text_widget.h
        string_widget.h
        value_widget.h
        timestamp_widget.h
        timecode_widget.h
        text_align.h
        color.h
)
