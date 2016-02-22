#ifndef TNC_HTTPHEADERS_H
#define TNC_HTTPHEADERS_H

#include "httpdate.h"

/** @file 
 * @brief Contiene string literal corrispondenti a header di richiesta e 
 * risposta.
 */

#define HEADERSIZE 8096
#define CRLF "\r\n"

#define HTTP10 "HTTP/1.0"
#define HTTP11 "HTTP/1.1"


/** @name Statusline di risposta
 *
 * Definizioni di stringhe che rappresentano la statusline di una
 * risposta HTTP.
 * 
 * @{
 */

#define RHDR_STATUSLINE_OK                "200 OK"
#define RHDR_STATUSLINE_NOTMODIFIED       "304 Not Modified"
#define RHDR_STATUSLINE_BADREQUEST        "400 Bad Request"
#define RHDR_STATUSLINE_NOTFOUND          "404 Not Found"
#define RHDR_STATUSLINE_NOTIMPLEMENTED    "501 Not Implemented"

/** @} */ // RHDR_STATUSLINE

/** @name Response header
 *
 * String literal formattabili con printf() o strfdate() utili alla creazione
 * di una risposta HTTP.
 *
 * @{
 */

#define RHDR_CONNECTION             "Connection: %s"
#define RHDR_CONTENT_TYPE           "Content-Type: %s; charset=%s"
#define RHDR_CONTENT_LENGTH         "Content-Length: %s"
#define RHDR_LAST_MODIFIED          "Last-Modified: " HTTPDATE 
#define RHDR_DATE                   "Date: " HTTPDATE 

/** @} */ // RHDR

/** Rappresenta i metodi HTTP riconoscibili ricevuti in ingresso. */
enum HTTPMethod
{
    HTTPMethod_unrecognized, HTTPMethod_GET, HTTPMethod_HEAD
};

/** Comando che viene eseguito per ottenere una stringa contenente il mimetype
 * di un file. */
#define COMMAND_MIMETYPE "xdg-mime query filetype %s | tr '\\n' '\\0'"

/** Comando che viene eseguito per ottenere una stringa contenente la codifica 
 * di un file. */
#define COMMAND_ENCODING "file -b --mime-encoding %s | tr '\\n' '\\0'"

#endif //TNC_HTTPHEADERS_H
