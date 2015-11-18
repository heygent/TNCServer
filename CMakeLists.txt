cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")

project(ProgettoSistemi)

set(SOURCE_FILES elaboraRichiesta.c inviaMessaggio.c httpheaders.c main.c )
add_executable(ProgettoSistemi ${SOURCE_FILES})
file(COPY ${CMAKE_SOURCE_DIR}/pages DESTINATION ${CMAKE_BINARY_DIR}/)
