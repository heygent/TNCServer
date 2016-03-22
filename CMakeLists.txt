cmake_minimum_required(VERSION 2.8.0)
project(TNC C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -ggdb -pthread -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("tnc/")
