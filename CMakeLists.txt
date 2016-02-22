cmake_minimum_required(VERSION 3.0.2)
project(TNC C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -ggdb -pthread")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("tnc/")
