#ifndef TNC_SERVER_H
#define TNC_SERVER_H

#include <inttypes.h>
#include <pthread.h>
#include <stdatomic.h>
#include "tnc/core/fixedthreadpool.h"

/** @file
 * Contiene funzioni utili all'uso del server.
 */


typedef struct _TNCServer *TNCServer;

/** Crea un nuovo TNCServer.
 *
 * @returns Un puntatore a un TNCServer se le allocazioni vanno a buon fine,
 * NULL altrimenti.
 */

TNCServer TNCServer_new();

/** Avvia il TNCServer passato come parametro, ovvero inizia ad ascoltare e a
 * rispondere alle richieste che riceve.
 *
 * La funzione inizializza i parametri interni del server, e li configura in
 * base agli argomenti passati. Una volta eseguita l'inizializzazione, il
 * server tenta di ottenere un socket di ascolto. Se ci riesce, avvia il
 * threadpool su cui vengono eseguiti i task necessari all'esecuzione del
 * server, e avvia su di questo un gestore di connessioni.
 *
 * @param self Il server da avviare.
 *
 * @param localpath Il percorso locale da servire.
 *
 * @param door La porta su cui il server deve operare.
 *
 * @param max_threads Il numero massimo di thread che il server può usare.
 *
 * @returns 0 se va a buon fine, un codice di errore altrimenti.  I codici di
 * errore relativi al server possono essere consultati nell'header
 * tnc/server/error.h
 *
 * @see error.h
 */

int TNCServer_start(
        TNCServer self,
        const char *localpath,
        uint16_t door,
        size_t max_threads
);

/** Termina il ciclo di ascolto del TNCServer passato come parametro, ed
 * esegue i TNCJob nello stack di cleanup aggiunte tramite
 * TNCServer_cleanup_push().
 *
 * @param self Il server su cui si vuole operare.
 */
void TNCServer_shutdown(TNCServer self);

/** Elimina (dealloca) un TNCServer.
 * Esegue lo shutdown se questa non è stata l'ultima operazione eseguita sul
 * server. */
void TNCServer_destroy(TNCServer self);

/** Restituisce il path che il server è impostato per servire. */
const char *TNCServer_getlocalpath(TNCServer self);

/** Aggiunge un job di cleanup da eseguire allo shutdown del server.
 *
 * @param self Il server su cui operare.
 *
 * @param job Il job che si vuole eseguire allo shutdown del server.
 */
void TNCServer_cleanup_push(TNCServer self, TNCJob job);

/** Rimuove l'ultimo TNCJob di cleanup aggiunto al server. Lo esegue se il
 * parametro `execute` è diverso da 0. */
void TNCServer_cleanup_pop(TNCServer self, int execute);

#endif //TNC_SERVER_H
