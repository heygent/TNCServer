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
#include "elaboraRichiesta.h"

#define PORTA "80"

static volatile bool continua = true;

void SIGINTHandler(int ignore) {
    continua = false;
    puts("\n");
}


/*****
 * Funzione di prova, prende in input un socket
 * e manda gli header del server e una pagina di default al client.
 *
 *
 */


int main(int argc, char *argv[]) {

    int socketAscolto, socketConnessione;
    struct addrinfo parametriIndirizzo, *indirizzoServer = NULL;
    struct sockaddr_storage indirizzoClient;
    socklen_t lunghezzaIndirizzoClient;
    int codiceErrore;

    // Inizializzo il socket
    memset(&parametriIndirizzo, 0, sizeof(parametriIndirizzo));
    parametriIndirizzo.ai_family = AF_UNSPEC;
    parametriIndirizzo.ai_socktype = SOCK_STREAM;
    parametriIndirizzo.ai_flags = AI_PASSIVE;

    // Ottiene informazioni sull'indirizzo del server
    codiceErrore = getaddrinfo(NULL, PORTA, &parametriIndirizzo, &indirizzoServer);


    CONTROLLAERRORI(codiceErrore, "ERRORE su getaddrinfo()");

    // Crea il socket di ascolto
    socketAscolto = socket(indirizzoServer->ai_family,
                           indirizzoServer->ai_socktype,
                           indirizzoServer->ai_protocol);

    CONTROLLAERRORI(socketAscolto, "ERRORE su socket()");

    // Lega il socket di ascolto alle informazioni sull'indirizzo messe prima
    codiceErrore = bind(socketAscolto, indirizzoServer->ai_addr, indirizzoServer->ai_addrlen);

    CONTROLLAERRORI(codiceErrore, "ERRORE su bind()");

    puts("Server in ascolto, vai su http://localhost per inviare una richiesta\n");
    listen(socketAscolto, 5);

    signal(SIGINT, SIGINTHandler);

    while(true)
    {
        lunghezzaIndirizzoClient = sizeof(indirizzoClient);

        socketConnessione = accept(socketAscolto,
                                   (struct sockaddr *) &indirizzoClient,
                                   &lunghezzaIndirizzoClient);

        if(!continua) break;

        CONTROLLAERRORI(socketConnessione, "ERRORE su accept()");

        elaboraRichiesta(socketConnessione);

    }


    close(socketAscolto);
    close(socketConnessione);

    return 0;
}
