file(GLOB_RECURSE TNCSERVER_SRC "*.c")
add_executable(tncserver "${TNCSERVER_SRC}")
set_target_properties(tncserver PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(tncserver tnccore) 

file(COPY "pages" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
