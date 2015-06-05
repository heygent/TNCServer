#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "httpheaders.h"
#include "debugmacro.h"

#define PORTA "80"

#define BUFSIZE 8096
#define HEADERSIZE 8096

int inviaMessaggio(int socketConnessione) {

    FILE * fileDaLeggere;
    int codiceErrore;
    int hSize;
    int cSize;
    long lunghezzaFile;
    char buffer[BUFSIZE+1];
    char * header;


    fileDaLeggere = fopen("./pages/default.html","rt");

    CONTROLLAERRORI(fileDaLeggere, "ERRORE Lettura file");

    fseek(fileDaLeggere, 0, SEEK_END);
    lunghezzaFile = ftell(fileDaLeggere);
    fseek(fileDaLeggere, 0, SEEK_SET);

    hSize = strlen(headers[0].stringa);
    cSize = snprintf(NULL, 0, CODA, lunghezzaFile);

    header = calloc(hSize + cSize + 1, sizeof(char));

    strcpy(header, headers[0].stringa);
    sprintf(&header[hSize], CODA, lunghezzaFile);

    send(socketConnessione, header, hSize + cSize, 0);

    while((codiceErrore = fread(buffer, 1, BUFSIZE, fileDaLeggere)) > 0 ) {
        send(socketConnessione,buffer,strlen(buffer),0);
    }

    return codiceErrore;
}
/*
struct headerClient leggiHeader(char headerRicevuto[]) {

	struct headerClient ritorno;
	int i=4;
	char * token;

	token = strtok(headerRicevuto, "\n");
//	if(ISGET(token)) {




}
*/


int main(int argc, char *argv[]) {

    int socketAscolto, socketConnessione;
    char headerClient[HEADERSIZE];
    struct addrinfo parametriIndirizzo, *indirizzoServer;
    struct sockaddr_storage indirizzoClient;
    socklen_t lunghezzaIndirizzoClient;
    int codiceErrore;

    memset(&parametriIndirizzo, 0, sizeof(parametriIndirizzo));
    parametriIndirizzo.ai_family = AF_UNSPEC;
    parametriIndirizzo.ai_socktype = SOCK_STREAM;
    parametriIndirizzo.ai_flags = AI_PASSIVE;

    codiceErrore = getaddrinfo(NULL, PORTA, &parametriIndirizzo, &indirizzoServer);


    CONTROLLAERRORI(codiceErrore, "ERRORE su getaddrinfo()");

    socketAscolto = socket(indirizzoServer->ai_family,
                           indirizzoServer->ai_socktype,
                           indirizzoServer->ai_protocol);

    CONTROLLAERRORI(socketAscolto, "ERRORE su socket()");

    codiceErrore = bind(socketAscolto, indirizzoServer->ai_addr, indirizzoServer->ai_addrlen);

    CONTROLLAERRORI(codiceErrore, "ERRORE su bind()");

    listen(socketAscolto, 5);

    lunghezzaIndirizzoClient = sizeof(indirizzoClient);

    socketConnessione = accept(socketAscolto,
                               (struct sockaddr *) &indirizzoClient,
                               &lunghezzaIndirizzoClient);

    CONTROLLAERRORI(socketConnessione, "ERRORE su accept()");

    codiceErrore = recv(socketConnessione, headerClient, HEADERSIZE - 1, 0);

    CONTROLLAERRORI(codiceErrore, "ERROR reading from socket");

//	leggiHeader(headerClient);

    printf("Client Header: \n\n%s\n", headerClient);


    codiceErrore = inviaMessaggio(socketConnessione);
    CONTROLLAERRORI(codiceErrore, "ERROR writing to socket");

    close(socketConnessione);
}