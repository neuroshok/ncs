cmake_minimum_required(VERSION 3.14)
project(ncs)

set(CMAKE_CXX_STANDARD 20)

set(NCS_ROOT ${CMAKE_SOURCE_DIR})

file(GLOB_RECURSE NCS_SOURCES
    "include/ncs/*.hpp"
    "source/ncs/*.cpp"
)

add_library(lib_ncs INTERFACE)
target_include_directories(lib_ncs INTERFACE include)

add_library(lib_color INTERFACE)
target_include_directories(lib_color INTERFACE third_party/color-console/include)
target_link_libraries(lib_ncs INTERFACE lib_color)

add_executable(ncs source/main.cpp ${NCS_SOURCES})
target_link_libraries(ncs lib_ncs)