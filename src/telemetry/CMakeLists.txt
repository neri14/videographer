cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        fit_parser.cpp
        telemetry.cpp
    PUBLIC
        datapoint.h
        enum_field.h
        value.h
        parser.h
        fit_parser.h
        telemetry.h
        
)
