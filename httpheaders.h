#ifndef PROGETTOSISTEMI_HTTPHEADERS_H
#define PROGETTOSISTEMI_HTTPHEADERS_H

#include <stdbool.h>

#define HEADERSIZE 8096

// https://tools.ietf.org/html/rfc7231#section-7.1.1.1
#define HTTPDATE "%a, %d %b %Y %T GMT"

#define HTTP10 "HTTP/1.0 "
#define HTTP11 "HTTP/1.1 "


// Response Header
#define RHDR_MAIN_OK                "200 OK\n"
#define RHDR_MAIN_BADREQUEST        "400 Bad Request\n"
#define RHDR_MAIN_NOTFOUND          "404 Not Found\n"
#define RHDR_MAIN_NOTIMPLEMENTED    "501 Not Implemented\n"


#define RHDR_CONNECTION             "Connection: %s\n"
#define RHDR_CONTENT_TYPE           "Content-Type: %s; charset=%s\n"
#define RHDR_CONTENT_LENGTH         "Content-Length: %s\n"
#define RHDR_LAST_MODIFIED          "Last-Modified: " HTTPDATE "\n"
#define RHDR_DATE                   "Date: " HTTPDATE "\n"

#define GETSTR "GET"
#define HEADSTR "HEAD"
#define POSTSTR "POST"

#define M_GET 0
#define M_HEAD 1
#define M_POST 2
#define M_BAD 255

#define COMMAND_MIMETYPE "mimetype -b %s | tr '\\n' '\\0'"
#define COMMAND_ENCODING "file -b --mime-encoding %s | tr '\\n' '\\0'"

typedef struct headerLine headerLine;
struct headerLine
{
    char *string;
    headerLine *nextLine;
};

typedef struct requestInfo requestInfo;
struct requestInfo
{
    FILE *toSend;
    headerLine *firstHeaderLine;
    struct stat *fileinfo;
    char *requestedPath;
    char *servedPath;
    unsigned char methodCode;
    bool closeConnection;
};

headerLine *makeContentHeaders(headerLine *, requestInfo *info);
headerLine *makeDateHeader(headerLine *toChain);

void freeRequestInfo(requestInfo *tofree);


#endif //PROGETTOSISTEMI_HTTPHEADERS_H