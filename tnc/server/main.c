#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "server.h"


int main(int argc, char **argv)
{
  TNCServer srv;

  if(argc < 2)
  {
    printf("Usage: %s DIR\n", argv[0]);
    pthread_exit(EXIT_SUCCESS);
  }

  srv = TNCServer_new(argv[1], 8080, 8);

  TNCServer_start(srv);

  puts("Press enter to exit");
  getchar();

  TNCServer_shutdown(srv, TNCServer_shutdown_finish_pending);
  TNCServer_destroy(srv);

  pthread_exit(EXIT_SUCCESS);

}
