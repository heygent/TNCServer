//
// Created by root on 02/01/16.
//

#ifndef PROGETTOSISTEMI_THREADPOLL_H
#define PROGETTOSISTEMI_THREADPOLL_H

#include "functiondefs.h"

typedef struct _TNCThreadPool *TNCThreadPool;
typedef struct _TNCJob TNCJob;

struct _TNCJob
{
    TNCFunction toexec;
    void *arg;
};


#endif //PROGETTOSISTEMI_THREADPOLL_H
