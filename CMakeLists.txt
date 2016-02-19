cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -ggdb")

project(TNC)

file(GLOB_RECURSE TNCSERVER_SRC "*.c")
add_executable(TNCServer "${TNCSERVER_SRC}")
target_link_libraries(TNCServer pthread)

file(GLOB_RECURSE TNCLIB_SRC "libtnc/*.c")
add_library(TNCLib STATIC $(TNCLIB_SRC))

file(COPY ${CMAKE_SOURCE_DIR}/pages DESTINATION ${CMAKE_BINARY_DIR}/)
