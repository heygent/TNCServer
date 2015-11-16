#include "elaboraRichiesta.h"
#include "inviaMessaggio.h"
#include "debugmacro.h"
#include "httpheaders.h"
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>

void elaboraRichiesta(int socketConnessione)
{
  ssize_t codiceErrore;

  char headerClient[HEADERSIZE]; // Qui viene salvata la richiesta fatta dal browser

  codiceErrore = recv(socketConnessione, headerClient, HEADERSIZE - 1, 0);

  CONTROLLAERRORI(codiceErrore, "ERROR reading from socket");

  printf("Client Header: \n\n%s\n", headerClient);

  codiceErrore = inviaMessaggio(socketConnessione, "./pages/default.html");
  CONTROLLAERRORI(codiceErrore, "ERROR writing to socket");
}
