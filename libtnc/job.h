//
// Created by jeff on 10/01/16.
//

#ifndef PROGETTOSISTEMI_JOB_H
#define PROGETTOSISTEMI_JOB_H

#include "typedefs.h"

struct TNCJob
{
    TNCFunction toexec;
    void *arg;
    TNCConsumer result_callback;
};

typedef struct TNCJob TNCJob;

#endif //PROGETTOSISTEMI_JOB_H
