cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -ggdb")

project(ProgettoSistemi)


set(SOURCE_FILES send_response.c paths.c server.c libtnc/list.c libtnc/fixedthreadpool.c httpdata.c parse_request.c make_response.c main.c httpdate.c httpdate.h httpheaders.h)
add_executable(ProgettoSistemi ${SOURCE_FILES})
target_link_libraries(ProgettoSistemi pthread)

file(COPY ${CMAKE_SOURCE_DIR}/pages DESTINATION ${CMAKE_BINARY_DIR}/)