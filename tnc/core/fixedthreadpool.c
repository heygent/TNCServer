#include <pthread.h>
#include <stdlib.h>
#include <stdatomic.h>
#include "debugutils.h"
#include "fixedthreadpool.h"
#include "list.h"
#include "error.h"

struct _TNCFixedThreadPool
{

    pthread_t *threads;             /** Un array contenente gli ID dei thread avviati. */
    size_t number_of_threads;       /** Il numero di thread per il threadpool di riferimento. */
    pthread_attr_t thread_attr;     /** Attributi di creazione dei thread */

    TNCList jobqueue;               /** La coda dei job per i worker. */
    pthread_mutex_t jobqueue_mutex; /** mutex per jobqueue */

    atomic_int shutdown;            /** Intero che indica se il threadpool attuale è in fase di shutdown. */

    pthread_cond_t wake_thread;     /** Condizione utile a svegliare i worker, se c'è un nuovo job o se il threadpool
                                        entra in fase di shutdown */

    pthread_mutex_t wake_thread_mutex; /** mutex necessario per usare wake_thread */

};

int TNCFixedThreadPool_enqueue(TNCFixedThreadPool self, TNCJob *job)
{
    int ret;


    TNCJob *job_copy = malloc(sizeof *job_copy);
    *job_copy = *job;

    pthread_mutex_lock(&self->jobqueue_mutex);

    ret = TNCList_push_back(self->jobqueue, job_copy);

    // Se l'operazione va a buon fine, sveglia i worker dormienti
    if(ret == TNCError_good) pthread_cond_signal(&self->wake_thread);

    pthread_mutex_unlock(&self->jobqueue_mutex);

    return ret;
}

int TNCFixedThreadPool_do_next(TNCFixedThreadPool self, TNCJob *job)
{
    int ret;


    pthread_mutex_lock(&self->jobqueue_mutex);

    ret = TNCList_push_front(self->jobqueue, job);

    // Se l'operazione va a buon fine, sveglia i worker dormienti
    if(ret == TNCError_good) pthread_cond_signal(&self->wake_thread);

    pthread_mutex_unlock(&self->jobqueue_mutex);

    return ret;
}

static int _TNCFixedThreadPool_init(TNCFixedThreadPool self, size_t threads)
{

    self->number_of_threads = threads;
    self->shutdown = 0;

    self->jobqueue_mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    self->wake_thread_mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    self->wake_thread = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

    self->jobqueue = NULL;
    self->threads = NULL;

    self->shutdown = ATOMIC_VAR_INIT(0);

    self->jobqueue = TNCList_new();
    if(!self->jobqueue) goto fail;

    self->threads = malloc(sizeof(pthread_t) * self->number_of_threads);
    if(!self->threads) goto fail;

    if(pthread_attr_init(&self->thread_attr) != 0) goto fail;

    pthread_attr_setdetachstate(&self->thread_attr, PTHREAD_CREATE_JOINABLE);
    return 1;

    fail:

    free(self->jobqueue);
    free(self->threads);
    return 0;
}

TNCFixedThreadPool TNCFixedThreadPool_new(size_t threads)
{
    TNCFixedThreadPool self = malloc(sizeof *self);

    if(self)
    {
        self = _TNCFixedThreadPool_init(self, threads) ? self : NULL;
    }

    return self;
}

void TNCFixedThreadPool_destroy(TNCFixedThreadPool self)
{

    free(self->threads);
    pthread_mutex_destroy(&self->wake_thread_mutex);
    pthread_mutex_destroy(&self->jobqueue_mutex);
    pthread_cond_destroy(&self->wake_thread);
    TNCList_destroy_and_free(self->jobqueue, free);
}

static void *_TNCFixedThreadPool_executor(void *threadpoolptr)
{
    TNCFixedThreadPool threadpool = threadpoolptr;

    while(1)
    {
        int shutdown_flags = atomic_load(&threadpool->shutdown);
        if(shutdown_flags & TNCThreadPool_shutdown_do_shutdown &&
           ! (shutdown_flags & TNCThreadPool_shutdown_finish_pending))
           break;

        pthread_mutex_lock(&threadpool->jobqueue_mutex);

        if(!TNCList_empty(threadpool->jobqueue))
        {
            TNCJob *job = TNCList_pop_front(threadpool->jobqueue);
            pthread_mutex_unlock(&threadpool->jobqueue_mutex);

            void *result = job->toexec(job->arg);
            if(job->result_callback) job->result_callback(result);

            free(job);
        }
        else
        {
            pthread_mutex_unlock(&threadpool->jobqueue_mutex);
            shutdown_flags = atomic_load(&threadpool->shutdown);

            if(shutdown_flags & TNCThreadPool_shutdown_do_shutdown &&
               shutdown_flags & TNCThreadPool_shutdown_finish_pending)
               break;

            pthread_mutex_lock(&threadpool->wake_thread_mutex);
            pthread_cond_wait(&threadpool->wake_thread, &threadpool->wake_thread_mutex);
            pthread_mutex_unlock(&threadpool->wake_thread_mutex);
        }
    }

    return NULL;

}

int TNCFixedThreadPool_start(TNCFixedThreadPool self)
{
    int error;

    pthread_mutex_lock(&self->jobqueue_mutex);

    for(size_t i = 0; i < self->number_of_threads; ++i)
    {
        error = pthread_create(self->threads + i, &self->thread_attr, _TNCFixedThreadPool_executor, self);

        if(error)
        {

            atomic_store(&self->shutdown, TNCThreadPool_shutdown_do_shutdown);

            pthread_mutex_lock(&self->wake_thread_mutex);
            pthread_cond_broadcast(&self->wake_thread);
            pthread_mutex_unlock(&self->wake_thread_mutex);

            for(size_t j = 0; j < i; ++j)
            {
                pthread_detach(self->threads[j]);
            }

            pthread_mutex_unlock(&self->jobqueue_mutex);

            return TNCError_thread_start_failed;
        }
    }

    pthread_mutex_unlock(&self->jobqueue_mutex);

    return TNCError_good;

}

void TNCFixedThreadPool_shutdown(TNCFixedThreadPool self, int shutdown_flags)
{
    shutdown_flags |= TNCThreadPool_shutdown_do_shutdown;

    atomic_store(&self->shutdown, shutdown_flags);

    pthread_mutex_lock(&self->wake_thread_mutex);
    pthread_cond_broadcast(&self->wake_thread);
    pthread_mutex_unlock(&self->wake_thread_mutex);

    for(size_t i = 0; i < self->number_of_threads; ++i)
    {
        if(shutdown_flags & TNCThreadPool_shutdown_async)
          pthread_detach(self->threads[i]);
        else
          pthread_join(self->threads[i], NULL);

    }


}
