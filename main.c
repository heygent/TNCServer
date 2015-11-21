#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "debugmacro.h"
#include "processRequest.h"
#include "paths.h"
#include "paths.h"
#include "sendResponse.h"

#define PORTA "80"

static int to_close[10];

void cleanup(int);

int main(int argc, char *argv[]) {

    int listenSocket, socketConnessione;
    struct addrinfo parametriIndirizzo, *indirizzoServer = NULL;
    struct sockaddr_storage indirizzoClient;
    socklen_t lunghezzaIndirizzoClient;
    int codiceErrore;

    bzero(&to_close, 10 * sizeof(int));

    if(argc > 1)
    {
        size_t pathSize = strlen(argv[1]);

        while(argv[1][pathSize] == '/')
        {
            argv[1][pathSize--] = '\0';
        }

        localPathToServe = strdup(argv[1]);

    }
    else
    {
        localPathToServe = strdup(".");
    }

    if(access(localPathToServe, R_OK) != 0)
    {
        printf("Server: path %s not accessible.", localPathToServe);
        exit(0);
    }

    // Inizializzo il socket
    bzero(&parametriIndirizzo, sizeof(parametriIndirizzo));

    parametriIndirizzo.ai_family = AF_UNSPEC;
    parametriIndirizzo.ai_socktype = SOCK_STREAM;
    parametriIndirizzo.ai_flags = AI_PASSIVE;

    // Ottiene informazioni sull'indirizzo del server
    codiceErrore = getaddrinfo(NULL, PORTA, &parametriIndirizzo, &indirizzoServer);

    CONTROLLAERRORI(codiceErrore, "ERRORE su getaddrinfo()");

    // Crea il socket di ascolto
    listenSocket = socket(indirizzoServer->ai_family,
                          indirizzoServer->ai_socktype,
                          indirizzoServer->ai_protocol);

    to_close[0] = listenSocket;

    CONTROLLAERRORI(listenSocket, "ERRORE su socket()");

    int optval = 1;
    codiceErrore = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    CONTROLLAERRORI(codiceErrore, "ERRORE su setsockopt()");

    // Lega il socket di ascolto alle informazioni sull'indirizzo messe prima
    codiceErrore = bind(listenSocket, indirizzoServer->ai_addr, indirizzoServer->ai_addrlen);

    CONTROLLAERRORI(codiceErrore, "ERRORE su bind()");

    puts("Server in ascolto, vai su http://localhost per inviare una richiesta\n");
    listen(listenSocket, 5);

    signal(SIGINT, cleanup);
    // Ignora broken pipe
    signal(SIGPIPE, SIG_IGN);

    while(true)
    {
        lunghezzaIndirizzoClient = sizeof(indirizzoClient);

        socketConnessione = accept(listenSocket,
                                   (struct sockaddr *) &indirizzoClient,
                                   &lunghezzaIndirizzoClient);

        to_close[1] = socketConnessione;

        CONTROLLAERRORI(socketConnessione, "ERRORE su accept()");

        /*
        if(!fork()) {

            close(listenSocket);
            // */
            requestInfo *info;

            info = processRequest(socketConnessione);
            sendResponse(socketConnessione, info);
            close(socketConnessione);

            freeRequestInfo(info);
         /*
            exit(0);
        }

        close(listenSocket);
        // */
    }

}

void cleanup(int ignore) {
    for(int i = 0; i < 10 && to_close[i] != 0; ++i)
    {
        close(to_close[i]);
    }
}