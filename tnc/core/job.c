#include <stdlib.h>
#include <unistd.h>
#include "job.h"

void TNCJob_run(const TNCJob *job)
{
    void *result = job->toexec(job->arg);
    if(job->result_callback)
        job->result_callback(result);
}

void *TNCJob_free_wrapper(void *arg)
{
    free(arg);
    return NULL;
}

void *TNCJob_close_wrapper(void *freeable_fd)
{
    close(*(int *) freeable_fd);
    free(freeable_fd);
    return NULL;
}

TNCJob *TNCJob_new_free(void *arg)
{
    TNCJob *ret = malloc(sizeof *ret);

    ret->toexec = TNCJob_free_wrapper;
    ret->arg = arg;
    ret->result_callback = NULL;

    return ret;
}

TNCJob *TNCJob_new_close(int fd)
{
    int *freeable_fd = malloc(sizeof fd);
    TNCJob *ret = malloc(sizeof *ret);
    *freeable_fd = fd;

    ret->toexec = TNCJob_close_wrapper;
    ret->arg = freeable_fd;
    ret->result_callback = NULL;

    return ret;
}
