#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <setjmp.h>
#include <pthread.h>
#include "tnc_adt/runtime_error.h"
#include "paths.h"
#include "send_response.h"
#include "parse_request.h"
#include "make_response.h"
#include "memzero.h"

#define TNC_DOOR "8080"

jmp_buf interrupted_jmp;

void *connection_handler(void *connection_socket);
TNCList connection_handling_threads;
void stop_handler(int ignore);

int main(int argc, char **argv) {

    int listen_socket;
    int interrupted;

    struct addrinfo server_address_params;
    struct addrinfo *server_address;

    //struct sigaction exit_handler;

    int error_code;

    if(argc > 1)
    {
        size_t path_size = strlen(argv[1]);

        while(argv[1][path_size] == '/')
        {
            argv[1][path_size--] = '\0';
        }

        local_path_to_serve = strdup(argv[1]);

    }
    else
    {
        local_path_to_serve = strdup(".");
    }

    if(access(local_path_to_serve, R_OK) != 0)
    {
        printf("Server: path %s not accessible.", local_path_to_serve);
        return EXIT_FAILURE;
    }

    // Inizializzo il socket
    MEMZERO(&server_address_params);

    server_address_params.ai_family = AF_UNSPEC;
    server_address_params.ai_socktype = SOCK_STREAM;
    server_address_params.ai_flags = AI_PASSIVE;

    // Ottiene informazioni sull'indirizzo del server
    error_code = getaddrinfo(NULL, TNC_DOOR, &server_address_params, &server_address);

    if(error_code != 0) TNC_RUNTIME_ERROR_ERRNO("ERRORE su getaddrinfo()");

    // Crea il socket di ascolto
    listen_socket = socket(server_address->ai_family,
                           server_address->ai_socktype,
                           server_address->ai_protocol);

    if(listen_socket == -1) TNC_RUNTIME_ERROR_ERRNO("ERRORE su socket()");

    int optval = 1;
    error_code = setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    if(error_code == -1) TNC_RUNTIME_ERROR_ERRNO("ERRORE su setsockopt()");

    // Lega il socket di ascolto alle informazioni sull'indirizzo messe prima
    error_code = bind(listen_socket, server_address->ai_addr, server_address->ai_addrlen);

    if(error_code == -1) TNC_RUNTIME_ERROR_ERRNO("ERRORE su bind()");

    fprintf(stderr, "Server in ascolto, vai su http://localhost:%s per inviare una richiesta\n", TNC_DOOR);

    listen(listen_socket, 5);

    // Gestione degli handler dei segnali

    /*
    exit_handler.sa_handler = exit;
    sigemptyset(&exit_handler.sa_mask);
    exit_handler.sa_flags = 0;
    */

    interrupted = setjmp(interrupted_jmp);

    if(interrupted == 0) {

       // sigaction(SIGINT, stop_handler);
        signal(SIGTSTP, stop_handler);
        signal(SIGPIPE, SIG_IGN);

        while(true) {

            int connection_socket = -1, attempts = 0;
            pthread_t connection_thread;
            int *connection_socket_p;

            struct sockaddr_storage client_address;
            socklen_t client_address_len;

            client_address_len = sizeof client_address;

            while (connection_socket == -1 && attempts++ < 3)
                connection_socket = accept(listen_socket,
                                           (struct sockaddr *) &client_address,
                                           &client_address_len);

            if(connection_socket == -1) continue;
            
            connection_socket_p = malloc(sizeof *connection_socket_p);
            *connection_socket_p = connection_socket;

            pthread_create(&connection_thread, NULL, connection_handler, connection_socket_p);

        }
    }

    close(listen_socket);
    return EXIT_SUCCESS;

}

void *connection_handler(void *connection_socket_p)
{
    int keep_alive;
    int connection_socket = *(int*) connection_socket_p;

    free(connection_socket_p);

    do {
        char request[HEADERSIZE];

        ssize_t recv_ret;

        MEMZERO(request);

        recv_ret = recv(connection_socket, request, HEADERSIZE - 1, 0);

        if(recv_ret == -1) TNC_RUNTIME_ERROR_ERRNO("ERRORE su recv()");

        HTTPRequestData request_data;
        HTTPResponseData response_data;

        request_data = parse_request(request);
        keep_alive = request_data.flags.keep_alive;

        response_data = make_response(&request_data);
        send_response(connection_socket, &response_data);
        
        // TODO: Gestire memoria

    }
    while(keep_alive);

    close(connection_socket);
    
    return NULL;

}

void stop_handler(int ignore)
{
    exit(EXIT_SUCCESS);
}

