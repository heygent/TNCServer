#ifndef TNC_MAKE_RESPONSE_H
#define TNC_MAKE_RESPONSE_H

#include "httpdata.h"

/** @file
 * @brief Contiene funzioni utili a generare una risposta HTTP.
 */

/** Data una richiesta, restituisce un HTTPResponseData contenente gli elementi
 * necessari a generare una risposta.
 *
 * @param request_data L'HTTPRequestData per cui si vuole generare una risposta.
 *
 * @returns Un HTTPResponseData contenente gli header e i file descriptor utili
 * all'invio della risposta.
 *
 * @see HTTPResponseData
 *
 */
HTTPResponseData *make_response(const HTTPRequestData *request_data);

#endif //TNC_MAKE_RESPONSE_H
