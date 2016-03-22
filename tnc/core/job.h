#ifndef TNC_JOB_H
#define TNC_JOB_H

#include "functiontypes.h"

/** Struttura contenente un puntatore a funzione, un parametro da passarle, e
 * una callback da chiamare con parametro il risultato di questa(verrà
 * eseguita se diversa da NULL.
 */
struct TNCJob
{
    /** Puntatore alla funzione da chiamare.
     * @see TNCFunction */
    TNCFunction toexec;

    /** Parametro da passare a `toexec`. */
    void *arg;

    /** Puntatore a una funzione di callback, che verrà eseguita se questo è
     * diverso da NULL, e a cui verrà passata il risultato di `toexec`. */
    TNCConsumer result_callback;
};

typedef struct TNCJob TNCJob;

/** Esegue un TNCJob.
 *
 * @param job Il job da eseguire. */
void TNCJob_run(const TNCJob *job);

/** Crea un nuovo TNCJob (allocato nell'heap) che esegue `free` sul puntatore
 * passato come parametro.
 *
 * @param arg L'argomento da passare a `free` durante l'esecuzione del
 * TNCJob.
 *
 * @returns Un TNCJob allocato nell'heap. */
TNCJob *TNCJob_new_free(void *arg);

/** Crea un nuovo TNCJob (allocato nell'heap) che esegue `close` sul file
 * descriptor passato come parametro.
 *
 * @param fd L'argomento da passare a `close` durante l'esecuzione del
 * TNCJob.
 *
 * @returns Un TNCJob allocato nell'heap. */
TNCJob *TNCJob_new_close(int fd);

#endif //TNC_JOB_H
