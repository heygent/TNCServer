file(GLOB_RECURSE TNCCORE_SRC "*.c")
add_library(tnccore STATIC "${TNCCORE_SRC}" job.h job.c)
