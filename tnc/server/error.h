#ifndef TNCSERVER_ERROR_H
#define TNCSERVER_ERROR_H

#include "tnc/core/error.h"

/** @file
 * Contiene codici di errore restituiti dalle funzioni relative al server.
 */

/** Codici di errore restituiti dalle funzioni relative al server. */
enum TNCServerError
{
    /** Il percorso indicato come parametro non è valido. */ 
    TNCServerError_invalid_path = TNCError_module_defined,
    /** La connessione non è andata a buon fine. */
    TNCServerError_connection_failed,
    /** La risoluzione DNS tramite la funzione getaddrinfo() non è andata a
     * buon fine. */
    TNCServerError_fn_getaddrinfo_failed,
    /** La funzione socket() ha restituito un errore. */
    TNCServerError_fn_socket_failed,
    /** La funzione setsockopt() ha restituito un errore. */
    TNCServerError_fn_setsockopt_failed,
    /** La funzione bind() ha restituito un errore. */
    TNCServerError_fn_bind_failed
};

#endif //TNCSERVER_ERROR_H
