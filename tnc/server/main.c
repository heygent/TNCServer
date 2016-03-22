#include <stdlib.h>
#include <stdio.h>
#include <tnc/core/error.h>
#include <bits/signum.h>
#include <signal.h>
#include "server.h"
#include "error.h"

int main(int argc, char **argv)
{
    TNCServer srv;
    uint16_t door;
    int error;

    sigset_t sigpipe;

    sigemptyset(&sigpipe);
    sigaddset(&sigpipe, SIGPIPE);

    pthread_sigmask(SIG_BLOCK, &sigpipe, NULL);

    switch(argc)
    {

    case 2:
        door = 8080;
        break;

    case 3:
        error = sscanf(argv[2], "%" SCNu16, &door);

        if(error != 0 && error != EOF)
            break;

    default:
        printf("Usage: %s DIRECTORY [DOOR]\n", argv[0]);
        return EXIT_FAILURE;
    }

    srv = TNCServer_new();

    if(!srv)
        error = TNCError_failed_alloc;
    else
        error = TNCServer_start(srv, argv[1], door, 8);

    if(error != TNCError_good)
    {
        const char *exit_msg;

        switch(error)
        {
        case TNCError_failed_alloc:
            exit_msg = "Could not allocate enough memory.";
            srv = NULL;
            break;
        case TNCServerError_invalid_path :
            exit_msg = "The path asked to serve is invalid";
            break;
        case TNCServerError_fn_getaddrinfo_failed:
            exit_msg = "Error on address resolution.";
            break;
        case TNCServerError_fn_bind_failed:
            exit_msg = "Error on socket binding.";
            break;
        default:
            exit_msg = "The server encountered a problem. Aborting.";
        }
        TNCServer_destroy(srv);

        fputs(exit_msg, stderr);
        return EXIT_FAILURE;
    }


    fprintf(
        stderr,
        "This program is serving on http://localhost:%" PRIu16
        ", press enter to exit.\n",
        door
    );

    getchar();

    fputs("Shutting down, please wait...", stderr);

    TNCServer_shutdown(srv);
    TNCServer_destroy(srv);

    pthread_exit(EXIT_SUCCESS);

}
