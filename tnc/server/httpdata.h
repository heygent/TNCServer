#ifndef TNC_HTTPDATA_H
#define TNC_HTTPDATA_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "httpheaders.h"
#include "tnc/core/list.h"

/** @file
 * Contiene strutture dati utili al salvataggio dei dati delle richieste e
 * delle risposte HTTP.
 */

typedef struct HTTPRequestData HTTPRequestData;

/** Contiene informazioni su di una richiesta HTTP.
 * Una volta elaborate, le informazioni su di una richiesta HTTP ricevuta dal
 * server vengono raccolte in un'istanza di questa struttura.
 * Una struttura HTTPRequestData è restituita da parse_request() dopo
 * l'elaborazione a partire da una stringa, e può essere passata come parametro a
 * make_response() per elaborare la risposta del server.
 *
 */
struct HTTPRequestData
{
    /** Il file da inviare, che può essere il file richiesto, nel caso in cui
     * l'elaborazione della richiesta sia andata a buon fine, o una pagina di
     * errore altrimenti. */ 
    FILE                *file_to_serve;

    /** Un puntatore al risultato di stat() eseguito sul file richiesto. */
    struct stat         *file_to_serve_stat;

    /** Il percorso come indicato nella richiesta. */
    char                *remote_path;

    /** Il percorso locale di file_to_serve. */
    char                *path_to_serve;

    /** Lo status code, come da inserire nella risposta, a cui ha dato origine
     * l'elaborazione della richiesta. */
    int                 status_code;

    /** @name http_version
     * La versione di HTTP, come indicata nella richiesta.
     * @{
     */
    int                 http_version_major;
    int                 http_version_minor;
    /** @} */

    /** Un valore di HTTPMethod rappresentante il metodo della richiesta. */
    enum HTTPMethod     method;

    /** Bitmask rappresentante diverse informazioni sulla richiesta.
     * @see HTTPRequestData_flags 
     */
    uint8_t             flags;
};

typedef struct HTTPRequestHeader HTTPRequestHeader;

/** I valori di questa enumerazione vengono usati per creare una bitmask che
 * rappresenta informazioni e istruzioni di operazione per la richiesta HTTP a
 * cui si riferiscono. */
enum HTTPRequestData_flags
{
    /** Se questa opzione è settata, l'elaborazione della richiesta deve
     * terminare. */
    HTTPRequestData_flags_stop_parsing      = 1 << 0,
    /** Se questa opzione è settata, la connessione non deve terminare dopo
     * l'invio della risposta. */
    HTTPRequestData_flags_keep_alive        = 1 << 1,
    /** Se questa opzione è settata, devono essere inviati solo gli header
     * della risposta. */
    HTTPRequestData_flags_dont_send_payload = 1 << 2,
    /** Se questa opzione è settata, al posto del file richiesto il server deve
     * inviare una pagina di errore. */
    HTTPRequestData_flags_get_error_page    = 1 << 3
};

/** Rappresenta un header di richiesta HTTP.*/
struct HTTPRequestHeader
{
    /** Il nome dell'header. */
    char *field_name;
    /** Il valore dell'header. */
    char *field_content;
};

typedef struct HTTPResponseData HTTPResponseData;

struct HTTPResponseData
{
    const HTTPRequestData    *request_data;
    TNCList headers;
};

void HTTPRequestData_init(HTTPRequestData *data);
HTTPRequestData *HTTPRequestData_new();
void HTTPRequestData_destroy(HTTPRequestData *data);
void HTTPRequestData_destroy_members(HTTPRequestData *data);

void HTTPResponseData_init(HTTPResponseData *data, const HTTPRequestData *rd);
HTTPResponseData *HTTPResponseData_new(const HTTPRequestData *rd);
void HTTPResponseData_destroy_members(HTTPResponseData *data);
void HTTPResponseData_destroy(HTTPResponseData *data);

#endif //TNC_HTTPDATA_H
