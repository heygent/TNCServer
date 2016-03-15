#ifndef TNC_JOB_H
#define TNC_JOB_H

#include "functiontypes.h"

struct TNCJob
{
    TNCFunction toexec;
    void *arg;
    TNCConsumer result_callback;
};

typedef struct TNCJob TNCJob;

void *TNCJob_free_wrapper(void *arg);
void *TNCJob_close_wrapper(void *freeable_fd);

void TNCJob_run(TNCJob *job);
TNCJob *TNCJob_new_free(void *arg);
TNCJob *TNCJob_new_close(int fd);

#endif //TNC_JOB_H
