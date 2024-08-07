cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        fit_parser.cpp
    PUBLIC
        datapoint.h
        enum_field.h
        fit_parser.h
        parser.h
        value.h
)
