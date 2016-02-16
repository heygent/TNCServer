//
// Created by jeff on 10/01/16.
//

#ifndef TNC_JOB_H
#define TNC_JOB_H

#include "typedefs.h"

struct TNCJob
{
    TNCFunction toexec;
    void *arg;
    TNCConsumer result_callback;
};

typedef struct TNCJob TNCJob;

#endif //TNC_JOB_H
