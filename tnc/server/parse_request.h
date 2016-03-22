#ifndef TNC_PARSE_REQUEST_H
#define TNC_PARSE_REQUEST_H

#include "httpdata.h"

/** @file
 * @brief Contiene elementi utili al parsing della richiesta HTTP.
 */

/** Esegue il parsing di una richiesta, e inserisce le informazioni ottenute
 * da questa in un HTTPRequestData.
 *
 * La funzione esegue le seguenti operazioni:
 *
 * - Esegue il parsing della request line
 * - Verifica se l'accesso alla risorsa è consentito
 * - A seconda dell'esito di questa operazione, sceglie se rispondere con una
 *   pagina di errore
 * - Raccoglie le informazioni necessarie sulla risposta da inviare
 *   (dimensione, contenuto, ecc.)
 * - Ritorna.
 *
 * I dati raccolti vengono inseriti mano a mano nell'HTTPRequestData il cui
 * puntatore viene passato come argomento.
 *
 * @param request La stringa contenente la richiesta HTTP.
 *
 * @param self Il server su cui si sta operando.
 *
 * @returns Un puntatore a un HTTPRequestData contenente informazioni sulla
 * richiesta.
 *
 * @see HTTPRequestData
 *
 */

void parse_request(TNCServer self, char *request, HTTPRequestData *data);

#endif //TNC_PARSE_REQUEST_H
