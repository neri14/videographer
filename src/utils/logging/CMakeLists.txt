cmake_minimum_required(VERSION 3.20.0)

target_sources(vgraph_lib
    PRIVATE
        logger.cpp
        backend.cpp
        stream_sink.cpp
    PUBLIC
        log_level.h
        sink.h
        logger.h
        backend.h
        stream_sink.h
)
