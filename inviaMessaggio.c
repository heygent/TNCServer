#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "inviaMessaggio.h"
#include "debugmacro.h"
#include "httpheaders.h"

#define BUFSIZE 8096

bool inviaMessaggio(int socketConnessione, const char *path)

{

    FILE * fileDaLeggere;
    size_t dimensioneHeader; // Dimensione della prima parte dell'header da inviare
    size_t cSize; // Dimensione della seconda parte dell'header da inviare
    long lunghezzaFile;
    char buffer[BUFSIZE+1];
    char * header;
    bool ret;

    fileDaLeggere = fopen(path, "rt");

    CONTROLLAERRORI(fileDaLeggere, "ERRORE Lettura file");

    // Con fseek si indica dove leggere nel file. Viene impostata alla fine del file
    fseek(fileDaLeggere, 0, SEEK_END);

    // ftell restituisce l'attuale posizione di lettura nel file.
    // Visto che ora la posizione è alla fine, ftell restituisce la dimensione del file.
    lunghezzaFile = ftell(fileDaLeggere);

    // Reimposta la posizione all'inizio
    fseek(fileDaLeggere, 0, SEEK_SET);

    // Calcola la dimensione della testa dell'header da inviare
    dimensioneHeader = strlen(HEADERS[OK].stringa);

    // Calcola la dimensione della coda dell'header.
    // snprintf(char &buffer, int maxlen, char str, char ... attributes)
    // stampa una stringa formattata in printf in un buffer.
    // La funzione restituisce il numero di caratteri che non entrano nel buffer
    // Come parametri fornisco un puntatore nullo, lunghezza 0, e la stringa da stampare con i parametri.
    // In questo modo la funzione restituisce la lunghezza della stringa formattata.

    cSize = (size_t) snprintf(NULL, 0, CODA, lunghezzaFile);

    // Alloco memoria della dimensione totale dell'header più un carattere di fine stringa
    header = calloc(dimensioneHeader + cSize + 1, sizeof(char));

    // Copio gli header necessari nel buffer
    strcpy(header, HEADERS[OK].stringa);
    sprintf(&header[dimensioneHeader], CODA, lunghezzaFile);

    // Mando l'header attraverso il socket
    send(socketConnessione, header, dimensioneHeader + cSize, 0);

    free(header);

    // Mando il file html, in pacchetti da 1 byte
    while(fread(buffer, 1, BUFSIZE, fileDaLeggere)) {
        send(socketConnessione, buffer, strlen(buffer), 0);
    }

    ret = ferror(fileDaLeggere) != 0;

    fclose(fileDaLeggere);

    return ret;
}
