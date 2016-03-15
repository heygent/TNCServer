#ifndef TNC_SERVER_H
#define TNC_SERVER_H

#include <inttypes.h>
#include <pthread.h>
#include <stdatomic.h>
#include "tnc/core/fixedthreadpool.h"

/** @file
 * Contiene funzioni utili all'uso del server.
 */

enum TNCServer_shutdown_flags
{
    TNCServer_shutdown_async            = TNCThreadPool_shutdown_async,
    TNCServer_shutdown_finish_pending   = TNCThreadPool_shutdown_finish_pending,
};

typedef struct _TNCServer *TNCServer;

/** Crea un nuovo TNCServer.
 *
 * @param localpath Il percorso locale da servire.
 *
 * @param door La porta su cui il server deve operare.
 *
 * @param max_threads Il numero massimo di thread che il server può usare.
 *
 * @returns Un puntatore opaco a un TNCServer.
 */

TNCServer TNCServer_new(
    const char *localpath,
    uint16_t door,
    size_t max_threads
);

/** Avvia il TNCServer passato come parametro, ovvero inizia ad ascoltare e a
 * rispondere alle richieste che riceve.
 *
 * @param self Il server da avviare.
 *
 * @returns 0 se va a buon fine, un codice di errore altrimenti.  I codici di
 * errore relativi al server possono essere consultati nell'header
 * tnc/server/error.h
 *
 * @see error.h
 */

int TNCServer_start(TNCServer self);

/** Termina il TNCServer passato come parametro.
 *
 * @param self Il server su cui si vuole operare.
 *
 * @param shutdown Se settato su TNCServer_shutdown_finish_pending, viene
 * attesa l'elaborazione delle richieste già ricevute dal server prima di
 * terminarlo. Se settato su TNCServer_shutdown_now, il server esegue
 * l'elaborazione delle sole richieste su cui ha già iniziato ad operare prima
 * di terminare.
 *
 * @param wait Se settato su TNCServer_wait_yes, la funzione attende la
 * terminazione del server prima di terminare. Se settato su TNCServer_wait_no,
 * la chiamata a TNCServer_shutdown termina immediatamente.
 *
 */
void TNCServer_shutdown(TNCServer self, int shutdown_flags);

/** Elimina (dealloca) un TNCServer. */
void TNCServer_destroy(TNCServer self);

/** Restituisce il path che il server è impostato per servire. */
const char *TNCServer_getlocalpath(TNCServer self);

void TNCServer_cleanup_push(TNCServer self, TNCJob job);

void TNCServer_cleanup_pop(TNCServer self, int execute);

#endif //TNC_SERVER_H
