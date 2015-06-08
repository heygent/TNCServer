#ifndef PROGETTOSISTEMI_HTTPHEADERS_H
#define PROGETTOSISTEMI_HTTPHEADERS_H

#define OK 0
#define UNAUTHORIZED 1
#define BADREQUEST 2
#define FORBIDDEN 3
#define NOTFOUND 4
#define METHODNOTALLOWED 5

#define SIZEOFHEADERS 6

//
#define CODA "Connection: close\nContent-Type: text/html\nContent-Length: %ld\n\n"

#define ISGET(str) strncmp(str, "GET ", 4) == 0 || strncmp(str, "get ", 4) == 0
#define ISHEAD(str) strncmp(str, "HEAD ", 5) == 0 || strncmp(str, "head ", 5) == 0
#define ISPOST(str) strncmp(str, "POST ", 5) == 0 || strncmp(str, "post ", 5) == 0

/***
 * Il browser manda una richiesta al server.
 * Il server risponde sempre con un header (del testo contenente informazioni),
 * e, a seconda dei casi, con un contenuto (es. pagina html).
 * Se il server può soddisfare la richiesta del browser, il server risponde inviando
 * un header di cui la prima riga è "HTTP/1.1 200 OK".
 * Un esempio di risposta completa del server è:
 *
 * HTTP/1.1 200 OK
 * Connection: close
 * Content-type: text/html
 * Content-length: 176
 *
 * <html>
 *     <head>
 *         ...
 *
 * L'header è separato dal contenuto da una riga vuota.
 * I parametri di quest'header sono:
 * - Connection, che stabilisce se la connessione deve continuare o deve essere chiusa
 * - Content-type, che indica il tipo di contenuto inviato dal server
 * - Content-length, che deve contenere la lunghezza del contenuto
 *
 * L'array headers[], qua sotto, contiene gli header con i rispettivi codici di errore.
 * Se il server non può soddisfare la richiesta, la prima riga dell'header indica il codice di errore.
 * Ad esempio, se il file richiesto dal client non esiste, il server manda un header con codice 404,
 * o se la richiesta del client non è leggibile, il server manda un errore 402.
 *
 * CODA, per ora, contiene i parametri comuni a tutte le risposte che può dare il server, mentre l'array
 * headers[] contiene la testa, cioè prima riga e i parametri contestuali al codice di errore.
 *
 */

struct contenitoreHeader {
    int codice;
    char stringa[];
} headers[] = {
        { 200, "HTTP/1.1 200 OK\n" },
        { 401, "HTTP/1.1 401 Unauthorized\n" },
        { 402, "HTTP/1.1 402 Bad Request\n" },
        { 403, "HTTP/1.1 403 Forbidden\n" },
        { 404, "HTTP/1.1 404 Not Found\n" },
        { 405, "HTTP/1.1 405 Method Not Allowed\nAllowed: GET, HEAD\n" }
};
/*
struct headerClient {
    int codice;
    char metodo[5];
    char percorso[];
};
*/
#endif //PROGETTOSISTEMI_HTTPHEADERS_H
