#include "processRequest.h"
#include "sendResponse.h"
#include "debugmacro.h"
#include "paths.h"
#include <sys/socket.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

extern char *localPathToServe;

requestInfo *processRequest(int socketConnessione)
{
    requestInfo *ret;

    FILE *fileToServe = NULL;
    char *pathToServe = NULL;

    char requestHeader[HEADERSIZE]; // Qui viene salvata la richiesta fatta dal browser
    char *requestHeaderLine, *currentToken;
    char *responseHeaderLine;
    char *saveptrLine = NULL, *saveptrSpace = NULL;
    headerLine *currentHdr;


    unsigned char methodCode;
    bool notFound = false;
    ssize_t codiceErrore;

    ret = malloc(sizeof(requestInfo));
    struct stat *fileinfo = malloc(sizeof(struct stat));

    // Ricevi richiesta

    codiceErrore = recv(socketConnessione, requestHeader, HEADERSIZE - 1, 0);

    CONTROLLAERRORI(codiceErrore, "ERROR reading from socket");
    printf("Client Header: \n\n%s\n", requestHeader);

    // Tokenizza

    requestHeaderLine = strtok_r(requestHeader, "\n", &saveptrLine);
    currentToken = strtok_r(requestHeaderLine, " ", &saveptrSpace);

    // Leggi il metodo HTTP

    if      (strcasecmp(currentToken, GETSTR) == 0)  methodCode = M_GET;
    else if (strcasecmp(currentToken, HEADSTR) == 0) methodCode = M_HEAD;
    else if (strcasecmp(currentToken, POSTSTR) == 0) methodCode = M_POST;
    else                                             methodCode = M_BAD;

    ret->methodCode = methodCode;

    // Leggi il percorso richiesto

    currentToken = strtok_r(NULL, " ", &saveptrSpace);
    ret->requestedPath = strdup(currentToken);

    // Ottieni informazioni sul file richiesto

    if(strcmp(currentToken, PATH_ROOT) == 0)
    {
        pathToServe = getIndexPath();
    }
    else
    {
        pathToServe = malloc(strlen(localPathToServe) + strlen(currentToken) + 1);
        strcpy(pathToServe, localPathToServe);
        strcat(pathToServe, currentToken);
    }

    fileToServe = fopen(pathToServe, "rb");

    if(NULL == fileToServe)
    {

        pathToServe = PATH_DEFAULT_404;
        fileToServe = fopen(pathToServe, "rb");
        notFound = true;

    }

    ret->servedPath = pathToServe;
    ret->toSend = fileToServe;
    fstat(fileno(fileToServe), fileinfo);
    ret->fileinfo = fileinfo;

    // Make headers

    if(notFound)
    {
        responseHeaderLine = strdup(RHDR_MAIN_NOTFOUND);
    }
    else
    {
        switch(methodCode)
        {
            case M_GET:
            case M_HEAD:
                responseHeaderLine = strdup(RHDR_MAIN_OK);
                break;

            case M_POST:
                responseHeaderLine = strdup(RHDR_MAIN_NOTIMPLEMENTED);
                break;

            default:
                responseHeaderLine = strdup(RHDR_MAIN_BADREQUEST);
                break;

        }
    }

    currentHdr = calloc(1, sizeof(headerLine));
    currentHdr->string = responseHeaderLine;
    ret->firstHeaderLine = currentHdr;

    currentHdr = makeContentHeaders(currentHdr, ret);
    currentHdr = makeDateHeader(currentHdr);

    currentHdr->nextLine = NULL;

    return ret;

}

