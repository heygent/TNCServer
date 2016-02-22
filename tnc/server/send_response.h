#ifndef TNC_SEND_RESPONSE_H
#define TNC_SEND_RESPONSE_H

#include <stdbool.h>
#include "httpdata.h"

/** @file
 * Contiene definizioni e funzioni utili all'invio di risposte HTTP.
 */

/** Invia una risposta tramite un socket.
 *
 * La funzione invia tramite connection_socket una risposta HTTP, composta dagli
 * header e dal file contenuti in data. Per comporre il parametro data, usare
 * make_response().
 *
 * @param connection_socket Il socket attraverso cui inviare la risposta.
 *
 * @param data I dati sulla risposta da inviare.
 *
 * @return L'esito dell'operazione, true se è andata a buon fine, false
 * altrimenti.*
 *
 */
bool send_response(int connection_socket, HTTPResponseData *data);

#endif // TNC_SEND_RESPONSE_H
