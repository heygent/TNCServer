cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -pthread")

project(ProgettoSistemi)

set(SOURCE_FILES send_response.c paths.c main.c tnc_adt/list.c httpdata.c parse_request.c make_response.c make_response.h)
add_executable(ProgettoSistemi ${SOURCE_FILES})
file(COPY ${CMAKE_SOURCE_DIR}/pages DESTINATION ${CMAKE_BINARY_DIR}/)
