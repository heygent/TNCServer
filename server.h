#ifndef TNC_SERVER_H
#define TNC_SERVER_H

#include <inttypes.h>
#include <pthread.h>
#include <stdatomic.h>
#include "libtnc/fixedthreadpool.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"

enum TNCServer_shutdown
{
    TNCServer_shutdown_now              = TNCThreadPool_shutdown_now,
    TNCServer_shutdown_finish_pending   = TNCThreadPool_shutdown_finish_pending,
};

enum TNCServer_wait
{
    TNCServer_wait_yes = TNCThreadPool_wait_yes,
    TNCServer_wait_no  = TNCThreadPool_wait_no
};

struct _TNCServer
{
    char *local_path;
    uint16_t door;
    void *threadpool;
    size_t max_threads;

    atomic_int shutdown;
};

typedef struct _TNCServer *TNCServer;

TNCServer TNCServer_new(
    const char *localpath, 
    uint16_t door,
    size_t max_threads
);

int TNCServer_start(TNCServer self);

void TNCServer_shutdown(
    TNCServer self,
    enum TNCServer_shutdown shutdown,
    enum TNCServer_wait wait
);

void TNCServer_destroy(TNCServer self);

#pragma clang diagnostic pop

#endif //TNC_SERVER_H
