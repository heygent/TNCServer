#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>
#include "server.h"

jmp_buf signal_jmp;

void sighandler(int ignore)
{
    longjmp(signal_jmp, 0);
}

int main(int argc, char **argv)
{
    int signal_received;
    TNCServer srv;

    struct sigaction handler;
    sigset_t signals_to_block;


    if(argc < 2)
    {
      printf("Usage: %s DIR\n", argv[0]);
      pthread_exit(EXIT_SUCCESS);
    }

    handler.sa_handler = sighandler;

    sigemptyset(&signals_to_block);
    sigaddset(&signals_to_block, SIGINT);
    sigaddset(&signals_to_block, SIGTSTP);
    sigaddset(&signals_to_block, SIGTERM);

    pthread_sigmask(SIG_BLOCK, &signals_to_block, NULL);

    sigaction(SIGINT, &handler, NULL);
    sigaction(SIGTSTP, &handler, NULL);
    sigaction(SIGTERM, &handler, NULL);

    srv = TNCServer_new(argv[1], 8080, 8);

    signal_received = setjmp(signal_jmp);

    pthread_sigmask(SIG_UNBLOCK, &signals_to_block, NULL);

    if(signal_received)
    {
        TNCServer_shutdown(
            srv,
            TNCServer_shutdown_finish_pending,
            TNCServer_wait_yes
        );
        TNCServer_destroy(srv);

        pthread_exit(EXIT_SUCCESS);
    }

    TNCServer_start(srv);
    pause();
}
