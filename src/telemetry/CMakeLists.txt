cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        field.cpp
        parser.cpp
        fit_parser.cpp
        telemetry.cpp
    PUBLIC
        datapoint.h
        field.h
        parser.h
        fit_parser.h
        telemetry.h
)
