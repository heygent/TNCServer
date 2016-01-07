//
// Created by root on 17/12/15.
//


#ifndef TNC_MEMZERO_H
#define TNC_MEMZERO_H

#include <string.h>

#define MEMZERO(PTR) memset((PTR), 0, sizeof (*PTR))

#endif //TNC_MEMZERO_H
