cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")

project(ProgettoSistemi)


set(SOURCE_FILES send_response.c paths.c main.c tnc_adt/list.c tnc_adt/threadpool.c httpdata.c parse_request.c make_response.c)
add_executable(ProgettoSistemi ${SOURCE_FILES})
target_link_libraries(ProgettoSistemi pthread)

file(COPY ${CMAKE_SOURCE_DIR}/pages DESTINATION ${CMAKE_BINARY_DIR}/)