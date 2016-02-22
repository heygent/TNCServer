#ifndef TNC_PARSE_REQUEST_H
#define TNC_PARSE_REQUEST_H

#include "httpdata.h"

/** @file
 * @brief Contiene elementi utili al parsing della richiesta HTTP.
 */

/** Data una richiesta e il server che la riceve, restituisce un puntatore a un
 * HTTPRequestData contenente informazioni su di questa.
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

HTTPRequestData *parse_request(TNCServer self, char *request);

#endif //TNC_PARSE_REQUEST_H
