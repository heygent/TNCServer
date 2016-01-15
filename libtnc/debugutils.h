//
// Created by jeff on 10/01/16.
//

#ifndef PROGETTOSISTEMI_DEBUGUTILS_H
#define PROGETTOSISTEMI_DEBUGUTILS_H

#include <stdio.h>

#ifdef TNC_DEBUG
#define TNC_dbgprint(...) do { fprintf(stderr, __VA_ARGS__); } while(0)
#else
#define TNC_dbgprint(...)
#endif

#endif //PROGETTOSISTEMI_DEBUGUTILS_H
