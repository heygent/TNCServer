#ifndef TNC_DEBUGMACRO_H
#define TNC_DEBUGMACRO_H
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define TNC_RUNTIME_ERROR(...) do { fprintf(stderr, __VA_ARGS__); abort(); } while(0)
#define TNC_RUNTIME_ASSERT(COND, ...) do { if(!(COND)) TNC_RUNTIME_ERROR(__VA_ARGS__); } while(0)
#define TNC_RUNTIME_ERROR_ERRNO(MSG) TNC_RUNTIME_ERROR("%s [%i]: %s", MSG, errno, strerror(errno))

#endif //TNC_DEBUGMACRO_H
