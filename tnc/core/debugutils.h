#ifndef TNC_DEBUGUTILS_H
#define TNC_DEBUGUTILS_H

#include <stdio.h>

#ifdef TNC_DEBUG
#define TNC_dbgprint(...) do { fprintf(stderr, __VA_ARGS__); } while(0)
#else
#define TNC_dbgprint(...)
#endif

#endif //TNC_DEBUGUTILS_H
