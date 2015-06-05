//
// Created by bendetta on 05/06/15.
//

#ifndef PROGETTOSISTEMI_HTTPHEADERS_H
#define PROGETTOSISTEMI_HTTPHEADERS_H

#define OK 0
#define UNAUTHORIZED 1
#define BADREQUEST 2
#define FORBIDDEN 3
#define NOTFOUND 4
#define METHODNOTALLOWED 5

#define SIZEOFHEADERS 6

#define CODA "Connection: close\nContent-Type: text/html\nContent-Length: %ld\n\n"

#define ISGET(str) strncmp(str, "GET ", 4) == 0 || strncmp(str, "get ", 4) == 0
#define ISHEAD(str) strncmp(str, "HEAD ", 5) == 0 || strncmp(str, "head ", 5) == 0
#define ISPOST(str) strncmp(str, "POST ", 5) == 0 || strncmp(str, "post ", 5) == 0

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

struct headerClient {
    int codice;
    char metodo[5];
    char percorso[];
};

#endif //PROGETTOSISTEMI_HTTPHEADERS_H
