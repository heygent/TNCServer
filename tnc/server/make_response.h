#ifndef TNC_MAKE_RESPONSE_H
#define TNC_MAKE_RESPONSE_H

#include "httpdata.h"

/** @file
 * @brief Contiene funzioni utili a generare una risposta HTTP.
 */

/** A partire da una richiesta di cui è stato eseguito il parsing, genera gli
 * header di risposta per questa.
 *
 * La funzione stampa su stringhe corrispondenti agli header di risposta le
 * informazioni ottenute nel parsing della richiesta, e le inserisce
 * nell'ordine in cui devono essere inviate in una TNCList. La lista generata
 * viene inserita nella struttura HTTPResponseData il cui puntatore è stato
 * passato come argomento.
 *
 * @param request_data Un HTTPResponseData in cui verranno inseriti gli
 * header di risposta.
 *
 * @returns Un HTTPResponseData contenente gli header di risposta.
 *
 * @see HTTPResponseData
 *
 */
void make_response(HTTPResponseData *response_data);

#endif //TNC_MAKE_RESPONSE_H
