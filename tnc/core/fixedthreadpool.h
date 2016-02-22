#ifndef TNC_FIXEDTHREADPOOL_H
#define TNC_FIXEDTHREADPOOL_H

#include <stdlib.h>
#include "functiontypes.h"

struct TNCJob
{
    TNCFunction toexec;
    void *arg;
    TNCConsumer result_callback;
};

typedef struct TNCJob TNCJob;

/**
 * @file
 * @brief Contiene l'interfaccia per i threadpool.
 */

/** Un puntatore opaco a una struttura threadpool */
typedef struct _TNCFixedThreadPool *TNCFixedThreadPool;


enum TNCFixedThreadPool_shutdown_flags
{
    TNCThreadPool_shutdown_now = 1,
    TNCThreadPool_shutdown_finish_pending
};

enum TNCFixedThreadPool_wait
{
    TNCThreadPool_wait_yes = 1, TNCThreadPool_wait_no
};

/** Crea un threadpool.
 *
 * @param threads Il numero di worker nel pool.
 *
 * @return Un TNCFixedThreadPool se l'operazione va a buon fine, NULL
 * altrimenti.
 */
TNCFixedThreadPool TNCFixedThreadPool_new(size_t threads);

/** Avvia un threadpool.
 *
 * @param self Il threadpool da avviare.
 *
 * @return Un codice di errore. I codici di errore si possono trovare
 * nell'header error.h.
 *
 */
int TNCFixedThreadPool_start(TNCFixedThreadPool self);

/** Termina i worker di un threadpool.
 *
 * @param self Il threadpool su cui si vuole operare.
 *
 * @param mode Parametro che indica la modalità di spegnimento.  Se settato su
 * TNCThreadPool_shutdown_now, lo spegnimento non attenderà il termine dei job
 * ancora nella coda.  Se settato su TNCThreadPool_shutdown_finish_pending lo
 * spegnimento avverrà al termine dei job ancora rimasti nella coda.
 *
 * @param wait Parametro che indica se la chiamata a shutdown blocca
 * l'esecuzione.  Se settato a TNCThreadPool_wait_yes, la chiamata a shutdown
 * blocca l'esecuzione fino al termine di tutti i thread. Se settato a
 * TNCThreadPool_wait_no, la chiamata a shutdown restituisce prima del termine
 * dell'esecuzione dei thread rimanenti.
 */
void TNCFixedThreadPool_shutdown(TNCFixedThreadPool self,
                            enum TNCFixedThreadPool_shutdown_flags mode,
                            enum TNCFixedThreadPool_wait wait);


/** Accoda un nuovo job alla coda. */
int TNCFixedThreadPool_enqueue(TNCFixedThreadPool self, TNCJob *job);

/** Mette un nuovo job di fronte coda. */
int TNCFixedThreadPool_do_next(TNCFixedThreadPool self, TNCJob *job);

/** Distrugge (dealloca) un TNCFixedThreadPool. */
void TNCFixedThreadPool_destroy(TNCFixedThreadPool self);

#endif //TNC_FIXEDTHREADPOOL_H
