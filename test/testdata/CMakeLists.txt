cmake_minimum_required(VERSION 3.20.0)

target_compile_definitions(vgraph_test PRIVATE TESTDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(vgraph_test PRIVATE CHECK="abc")
