#include <stdlib.h>
#include <stdio.h>
#include <tnc/core/error.h>
#include "server.h"
#include "error.h"


int main(int argc, char **argv)
{
    TNCServer srv;
    int error;

    if(argc < 2)
    {
        printf("Usage: %s DIRECTORY\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    srv = TNCServer_new(argv[1], 8080, 8);

    if(!srv) return EXIT_FAILURE;

    error = TNCServer_start(srv);

    if(error != TNCError_good)
    {
        const char *exit_msg;

        switch(error)
        {
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

        fprintf(stderr, "%s\n", exit_msg);
        return EXIT_FAILURE;
    }


    fprintf(stderr, "Press enter to exit\n");
    getchar();
    fprintf(stderr, "Shutting down, please wait...\n");

    TNCServer_shutdown(srv, TNCServer_shutdown_finish_pending);
    TNCServer_destroy(srv);

    pthread_exit(EXIT_SUCCESS);

}
