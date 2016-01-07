#include <pthread.h>
#include <stdlib.h>
#include "threadpool.h"
#include "list.h"

struct _TNCThreadPool
{
    size_t threads_max;
    size_t threads_min;
    size_t threads_actual;

    TNCList jobqueue;
    TNCList resultqueue;

    pthread_mutex_t threads_max_mutex;
    pthread_mutex_t threads_actual_mutex;
    pthread_mutex_t jobqueue_mutex;
    pthread_mutex_t resultqueue_mutex;

    pthread_cond_t jobqueue_not_empty;

};

int TNCThreadPool_enqueue(TNCThreadPool self, TNCJob *job)
{
    int ret;

    TNCJob *job_copy = malloc(sizeof *job_copy);
    *job_copy = *job;

    pthread_mutex_lock(&self->jobqueue_mutex);

    ret = TNCList_push_back(self->jobqueue, job_copy);

    pthread_cond_signal(&self->jobqueue_not_empty);

    pthread_mutex_unlock(&self->jobqueue_mutex);

    return ret;
}

int TNCThreadPool_do_next(TNCThreadPool self, TNCJob *job)
{
    int ret;

    pthread_mutex_lock(&self->jobqueue_mutex);

    ret = TNCList_push_front(self->jobqueue, job);

    pthread_cond_signal(&self->jobqueue_not_empty);

    pthread_mutex_unlock(&self->jobqueue_mutex);

    return ret;
}

inline static void _TNCThreadPool_init(TNCThreadPool self, size_t max_threads, size_t min_threads)
{
    self->threads_max = max_threads;
    self->threads_min = min_threads;
    self->threads_actual = 0;
    self->jobqueue_mutex = PTHREAD_MUTEX_INITIALIZER;
    self->jobqueue_not_empty = PTHREAD_COND_INITIALIZER;
}

TNCThreadPool TNCThreadPool_new(size_t min_threads, size_t max_threads)
{
    TNCThreadPool self = malloc(sizeof *self);

    if(self)
    {
        _TNCThreadPool_init(self, max_threads, min_threads);
    }

    return self;
}

void *_TNCThreadPool_executor(void *pool)
{
    TNCThreadPool self = pool;

    while(1)
    {
        int too_many_threads;

        pthread_mutex_lock(&self->threads_max_mutex);
        pthread_mutex_lock(&self->threads_actual_mutex);

        too_many_threads = self->threads_actual > self->threads_min;

        pthread_mutex_unlock(&self->threads_actual_mutex);
        pthread_mutex_unlock(&self->threads_max_mutex);

        if(too_many_threads) break;


        pthread_mutex_lock(&self->jobqueue_mutex);

        while (TNCList_empty(self->jobqueue))
        {
            pthread_cond_wait(&self->jobqueue_not_empty, &self->jobqueue_mutex);
        }

        TNCJob *job = TNCList_pop_front(self->jobqueue);

        pthread_mutex_unlock(&self->jobqueue_mutex);

        void *result = job->toexec(job->arg);

    }

    --self->threads_actual;
}