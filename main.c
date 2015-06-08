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

/*****
 * Funzione di prova, prende in input un socket
 * e manda gli header del server e una pagina di default al client.
 *
 *
 */

int inviaMessaggio(int socketConnessione) {

    FILE * fileDaLeggere;
    int codiceErrore;
    int hSize; // Dimensione della prima parte dell'header da inviare
    int cSize; // Dimensione della seconda parte dell'header da inviare
    long lunghezzaFile;
    char buffer[BUFSIZE+1];
    char * header;


    fileDaLeggere = fopen("./pages/default.html","rt");

    CONTROLLAERRORI(fileDaLeggere, "ERRORE Lettura file");

    // Con fseek si indica dove leggere nel file. Viene impostata alla fine del file
    fseek(fileDaLeggere, 0, SEEK_END);

    // ftell restituisce l'attuale posizione di lettura nel file.
    // Visto che ora la posizione è alla fine, ftell restituisce la dimensione del file.
    lunghezzaFile = ftell(fileDaLeggere);

    // Reimposta la posizione all'inizio
    fseek(fileDaLeggere, 0, SEEK_SET);

    // Calcola la dimensione della testa dell'header da inviare
    hSize = strlen(headers[OK].stringa);

    // Calcola la dimensione della coda dell'header.
    // snprintf(char &buffer, int maxlen, char str, char ... attributes)
    // stampa una stringa formattata in printf in un buffer.
    // La funzione restituisce il numero di caratteri che non entrano nel buffer
    // Come parametri fornisco un puntatore nullo, lunghezza 0, e la stringa da stampare con i parametri.
    // In questo modo la funzione restituisce la lunghezza della stringa formattata.

    cSize = snprintf(NULL, 0, CODA, lunghezzaFile);

    // Alloco memoria della dimensione totale dell'header più un carattere di fine stringa
    header = calloc(hSize + cSize + 1, sizeof(char));

    // Copio gli header necessari nel buffer
    strcpy(header, headers[OK].stringa);
    sprintf(&header[hSize], CODA, lunghezzaFile);

    // Mando l'header attraverso il socket
    send(socketConnessione, header, hSize + cSize, 0);

    // Mando il file html, in pacchetti da 1 byte
    while((codiceErrore = fread(buffer, 1, BUFSIZE, fileDaLeggere)) > 0 ) {
        send(socketConnessione,buffer,strlen(buffer),0);
    }

    return codiceErrore;
}


int main(int argc, char *argv[]) {

    int socketAscolto, socketConnessione;
    char headerClient[HEADERSIZE]; // Qui viene salvata la richiesta fatta dal browser
    struct addrinfo parametriIndirizzo, *indirizzoServer;
    struct sockaddr_storage indirizzoClient;
    socklen_t lunghezzaIndirizzoClient;
    int codiceErrore;

    // Inizializzo il socket
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

    printf("Server in ascolto, vai su http://localhost per inviare una richiesta\n");

    listen(socketAscolto, 5);

    lunghezzaIndirizzoClient = sizeof(indirizzoClient);

    socketConnessione = accept(socketAscolto,
                               (struct sockaddr *) &indirizzoClient,
                               &lunghezzaIndirizzoClient);

    CONTROLLAERRORI(socketConnessione, "ERRORE su accept()");

    codiceErrore = recv(socketConnessione, headerClient, HEADERSIZE - 1, 0);

    CONTROLLAERRORI(codiceErrore, "ERROR reading from socket");

    printf("Client Header: \n\n%s\n", headerClient);

    codiceErrore = inviaMessaggio(socketConnessione);
    CONTROLLAERRORI(codiceErrore, "ERROR writing to socket");

    close(socketConnessione);

    return 0;
}