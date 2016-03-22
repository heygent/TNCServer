#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <poll.h>
#include <fcntl.h>
#include "error.h"
#include "server.h"
#include "send_response.h"
#include "parse_request.h"
#include "make_response.h"
#include "tnc/core/debugutils.h"


#define BUFFERSIZE 8096


const static int default_max_threads = 8;

/** Struttura contenente i parametri interni del server. */
struct _TNCServer
{
    /** Il path su cui il server cerca i file da servire. */
    char *local_path;

    /** La porta su cui il server opera. */
    uint16_t door;

    /** Il threadpool su cui vengono eseguiti i task necessari all'esecuzione
     * del server. */
    TNCFixedThreadPool threadpool;

    /** Il massimo numero di thread che il threadpool ha il permesso di
     * avviare. */
    size_t max_threads;

    /** Il massimo numero di connessioni contemporanee gestibili dal server. */
    size_t max_connections;

    /** Il socket di ascolto. */
    int listen_socket;

    /** Valore che indica lo stato di spegnimento del server (0 no, != 0 sì) */
    atomic_int shutdown;

    /** Una pipe con cui vengono restituiti i socket ancora attivi al gestore
     * delle connessioni. I socket che arrivano da questa pipe sono conseguenza
     * della richiesta di una connessione persistente. */
    int alive_sockets_pipe[2];

    /** Questa pipe resta aperta finché il server è in funzione. Si può
     * verificare lo stato di spegnimento del server controllando se questa
     * pipe è ancora aperta. Viene usata per terminare varie chiamate a poll()
     * ancora attive durante lo spegnimento del server, facendo controllare a
     * queste, oltre ai socket di cui si occupano, se questa pipe è in stato
     * di errore. */
    int shutdown_pipe[2];
};

/** Tenta di ottenere un socket di ascolto
 *
 * @param self Il server che richiede il socket.
 *
 * @param listen_socket_ret Se la funzione ha successo, al suo termine il
 * valore puntato da questo parametro sarà il nuovo socket di ascolto,
 * altrimenti -1.
 *
 * @returns TNCError_good se la funzione ha successo, un altro codice di
 * errore altrimenti. I codici di errore sono consultabili nell'header error.h
 *
 * @see error.h
 */
static int get_listen_socket(TNCServer self, int *listen_socket_ret);

/** Funzione che ascolta le connessioni in arrivo e rimanda la loro gestione
 * a un handler.
 *
 * Esegue poll() sui socket di ascolto e connessione, sulla pipe dei socket
 * ancora attivi, e sulla pipe di shutdown.
 *
 * - Quando la pipe di shutdown viene chiusa, esegue operazioni di cleanup e
 *   termina.
 * - Quando c'è una nuova connessione nel socket di ascolto, la accetta e
 *   ricomincia il ciclo di attesa, aggiungendo l'arrivo di dati al socket di
 *   conessione agli eventi per cui poll() attende.
 * - Quando c'è qualcosa da leggere nei socket in connessione, accoda nel
 *   threadpool un TNCJob contenente un puntatore a
 *   request_handler_pthread_wrapper(), e un argomento contenente il socket
 *   su cui è possibile eseguire l'operazione di lettura. Accoda poi il job
 *   nel threadpool del server.
 * - Quando c'è qualcosa da leggere nella pipe dei socket ancora attivi,
 *   legge un int contenente il file descriptor per cui si è richiesta la
 *   connessione persistente, e aggiunge l'arrivo dei dati su questo socket
 *   all'elenco degli eventi per cui poll() attende.
 *
 */
void connection_listener(TNCServer self);

/** Esegue le operazioni necessarie alla gestione di una richiesta HTTP.
 *
 * La funzione:
 * - riceve la richiesta e la inserisce in un buffer;
 * - chiama le funzioni che eseguono il parsing di questa;
 * - chiama le funzioni che creano gli header di risposta;
 * - chiama la funzione che invia la risposta;
 *   * se è stata richiesta una connessione persistente, rimanda il socket al
 *     gestore delle connessione;
 *   * altriment lo chiude e termina.
 *
 */
int request_handler(TNCServer self, int connection_socket);


/** @defgroup pthread_wrappers
 * @brief Le funzioni il cui nome termina con `pthread_wrapper` eseguono le
 * funzioni che vengono eseguite dalle controparti senza il suffisso.
 *
 * Il parametro void* viene scompattato in una struttura contenente i parametri
 * che la funzione normalmente accetta, e che vengono passati a questa nella
 * sua chiamata.
 *
 * @ {
 */

static void *connection_listener_pthread_wrapper(
    void *self
);
static void *request_handler_pthread_wrapper(
    void *request_handler_param
);

struct request_handler_param
{
    TNCServer self;
    int connection_socket;
};

/** @} */

TNCServer TNCServer_new()
{
    TNCServer self = malloc(sizeof *self);
    if(!self) goto alloc_error;
    self->shutdown = ATOMIC_VAR_INIT(1);

    return self;

alloc_error:

    free(self);
    return NULL;

}

int TNCServer_start(
        TNCServer self,
        const char *localpath,
        uint16_t door,
        size_t max_threads
)
{
    int error_code;
    int listen_socket_flags;

    atomic_store(&self->shutdown, 0);
    self->door = door;
    self->max_threads = max_threads > 2 ?
                        max_threads : default_max_threads;
    self->max_connections = 200;
    self->local_path = strdup(localpath);

    if(!self->local_path)
        return TNCError_failed_alloc;

    pipe(self->alive_sockets_pipe);
    pipe(self->shutdown_pipe);


    if(!self->local_path || access(self->local_path, R_OK) != 0)
        return TNCServerError_invalid_path;

    error_code = get_listen_socket(self, &self->listen_socket);

    if(error_code != TNCError_good)
        return error_code;

    listen_socket_flags = fcntl(self->listen_socket, F_GETFL, 0);

    fcntl(
        self->listen_socket,
        F_SETFL,
        listen_socket_flags | O_NONBLOCK
    );

    listen(self->listen_socket, 10);

    self->threadpool = TNCFixedThreadPool_new(self->max_threads);
    if(!self->threadpool) return TNCError_failed_alloc;

    TNCFixedThreadPool_start(self->threadpool);


    TNCJob job =
    {
        .toexec = connection_listener_pthread_wrapper,
        .arg = self,
        .result_callback = NULL
    };

    TNCFixedThreadPool_enqueue(self->threadpool, &job);

    return TNCError_good;

}

void connection_listener(TNCServer self)
{
    int listen_socket = self->listen_socket;
    struct pollfd socket_events[self->max_connections];
    size_t last_socket = 2;

    TNCJob connection_handler_job =
        {
            .toexec = request_handler_pthread_wrapper,
            .result_callback = NULL
        };
    struct request_handler_param *connection_handler_param;

    struct sockaddr_storage client_address = {0};
    socklen_t client_address_len = sizeof (struct sockaddr_storage);

    socket_events[0].fd = self->shutdown_pipe[0];
    socket_events[0].events = 0;

    socket_events[1].fd = self->alive_sockets_pipe[0];
    socket_events[1].events = 0 | POLLIN;

    socket_events[2].fd = listen_socket;
    socket_events[2].events = 0 | POLLIN;


    sigset_t sigpipe;

    sigemptyset(&sigpipe);
    sigaddset(&sigpipe, SIGPIPE);

    pthread_sigmask(SIG_BLOCK, &sigpipe, NULL);

    while(atomic_load(&self->shutdown) == 0)
    {
        int current_revents;
        poll(socket_events, last_socket + 1, 60 * 1000);

        current_revents = socket_events[0].revents;

        if(current_revents & POLLERR ||
           current_revents & POLLHUP ||
           current_revents & POLLNVAL )
            break;

        current_revents = socket_events[1].revents;

        if(current_revents & POLLIN)
        {
            int still_alive_socket;
            read(self->alive_sockets_pipe[0], &still_alive_socket, sizeof(int));

            ++last_socket;

            socket_events[last_socket].fd = still_alive_socket;
            socket_events[last_socket].events = POLLIN;

            continue;

        }

        current_revents = socket_events[2].revents;

        if(current_revents & POLLIN)
        {
            int connection_socket =
                accept(
                    self->listen_socket,
                    (struct sockaddr *) &client_address,
                    &client_address_len
                );

            if(self->max_connections <= last_socket)
            {
                close(connection_socket);
                continue;
            }

            int connection_socket_flags = fcntl(connection_socket, F_GETFL, 0);

            fcntl(
                connection_socket,
                F_SETFL,
                connection_socket_flags | O_NONBLOCK
            );

            ++last_socket;

            socket_events[last_socket].fd = connection_socket;
            socket_events[last_socket].events = POLLIN;

            continue;
        }


        for(size_t i = 3; i <= last_socket; ++i)
        {
            current_revents = socket_events[i].revents;
            if(current_revents & POLLIN)
            {
                int ready_socket = socket_events[i].fd;

                connection_handler_param = malloc(sizeof *connection_handler_param);

                connection_handler_param->connection_socket = ready_socket;
                connection_handler_param->self = self;

                connection_handler_job.arg = connection_handler_param;

                TNCFixedThreadPool_enqueue(self->threadpool,
                                           &connection_handler_job);

                if(i != last_socket)
                {
                    socket_events[i] = socket_events[last_socket];
                }

                last_socket -= 1;
                i -= 1;

            }
            if(
                current_revents & POLLERR ||
                current_revents & POLLHUP ||
                current_revents & POLLNVAL
                )
            {
                close(socket_events[i].fd);

                if(i != last_socket)
                {
                    socket_events[i] = socket_events[last_socket];
                }

                last_socket -= 1;
                i -= 1;
            }
        }

    }
}

static int get_listen_socket(TNCServer self, int *listen_socket_ret)
{
    int listen_socket;

    struct addrinfo server_address_hints;
    struct addrinfo *server_address;

    char door[sizeof "65535"]; // TCP door is 16 bit
    int error_code;

    // Inizializzo il socket
    memset(&server_address_hints, 0, sizeof server_address_hints);

    server_address_hints.ai_family = AF_INET;
    server_address_hints.ai_socktype = SOCK_STREAM;
    server_address_hints.ai_flags = AI_PASSIVE;
    server_address_hints.ai_protocol = IPPROTO_TCP;

    sprintf(door, "%" PRIu16, self->door);

    // Ottiene informazioni sull'indirizzo del server
    error_code = getaddrinfo(
                     NULL,
                     door,
                     &server_address_hints,
                     &server_address
                 );

    if(error_code != 0)
    {
        TNC_dbgprint("[TNCServer_start]: Error on getaddrinfo()\n\t:%s",
                     gai_strerror(error_code));
        return TNCServerError_fn_getaddrinfo_failed;
    }


    int optval = 1;

    do
    {

        listen_socket = socket(server_address->ai_family,
                               server_address->ai_socktype,
                               server_address->ai_protocol);

        if (listen_socket == -1) continue;

        error_code = setsockopt(
                         listen_socket,
                         SOL_SOCKET,
                         SO_REUSEADDR,
                         &optval,
                         sizeof optval
                     );

        if(error_code != 0) continue;

        error_code = bind(
                         listen_socket,
                         server_address->ai_addr,
                         server_address->ai_addrlen
                     );

        if (error_code == 0) break;

        close(listen_socket);
    }
    while ((server_address = server_address->ai_next));

    int has_binded = server_address != NULL;

    freeaddrinfo(server_address);

    if(has_binded)
    {
        *listen_socket_ret = listen_socket;
        return TNCError_good;
    }

    *listen_socket_ret = -1;
    return TNCServerError_fn_bind_failed;
}

static void *connection_listener_pthread_wrapper(void *self)
{
    connection_listener(self);
    return NULL;
}

static void *request_handler_pthread_wrapper(void *_param)
{
    struct request_handler_param *param =
        (struct request_handler_param *) _param;

    request_handler(param->self, param->connection_socket);
    free(param);
    return NULL;
}

int request_handler(TNCServer self, int connection_socket)
{
    char request[BUFFERSIZE];
    ssize_t recv_ret;
    size_t request_size;
    int keep_alive = 0;

    TNC_dbgprint("\tConnection handler started.\n");

    memset(request, 0, BUFFERSIZE);
    request_size = 0;
    int stop_receiving;
    int revents;

    struct pollfd connection_socket_pollfd[2] =
    {
        {
            .fd = self->shutdown_pipe[0],
            .events = 0
        },
        {
            .fd = connection_socket,
            .events = 0 | POLLIN
        }
    };

    do
    {
        poll(connection_socket_pollfd, 2, 30 * 1000);

        revents = connection_socket_pollfd[0].revents;

        if(revents & POLLERR || revents & POLLNVAL || revents & POLLHUP)
            goto connection_loop_end;

        revents = connection_socket_pollfd[1].revents;

        if(revents & POLLIN)
        {
            recv_ret =
                recv(
                    connection_socket,
                    request + request_size,
                    BUFFERSIZE - request_size - 1, 0 | MSG_WAITALL
                );

            if(recv_ret == -1 || recv_ret == 0)
                goto connection_loop_end;


            request_size += recv_ret;

            if(request_size == BUFFERSIZE)
                goto connection_loop_end;
        }
        else if(
            revents & POLLERR ||
            revents & POLLHUP ||
            revents & POLLNVAL ||
            revents == 0
            )
            goto connection_loop_end;


        stop_receiving =
            strcmp((request + request_size - 2), "\n\n") == 0 ||
            strcmp((request + request_size - 4), CRLF CRLF) == 0;

    }
    while(!stop_receiving);

    HTTPRequestData request_data;
    HTTPResponseData response_data;

    HTTPRequestData_init(&request_data);

    parse_request(self, request, &request_data);

    keep_alive = request_data.flags & HTTPRequestData_flags_keep_alive;

    HTTPResponseData_init(&response_data, &request_data);

    make_response(&response_data);
    send_response(connection_socket, &response_data);

    HTTPRequestData_cleanup(&request_data);
    HTTPResponseData_cleanup(&response_data);


connection_loop_end:

    if(keep_alive)
    {
        write(self->alive_sockets_pipe[1], &connection_socket, sizeof(int));
    }
    else
    {
        close(connection_socket);
    }

    return TNCError_good;

}



const char *TNCServer_getlocalpath(TNCServer self)
{
    return self->local_path;
}

/* -------- Funzioni di cleanup ------------ */

void TNCServer_shutdown(TNCServer self)
{
    atomic_store(&self->shutdown, 1);
    shutdown(self->listen_socket, SHUT_RDWR);

    close(self->shutdown_pipe[1]);
    close(self->shutdown_pipe[0]);
    close(self->alive_sockets_pipe[0]);
    close(self->alive_sockets_pipe[1]);
    close(self->listen_socket);

    TNCFixedThreadPool_shutdown(self->threadpool, 0);

}

void TNCServer_destroy(TNCServer self)
{
    if(!self) return;

    if(self->shutdown == 0)
        TNCServer_shutdown(self);

    TNCFixedThreadPool_destroy(self->threadpool);

    free(self->local_path);
    free(self);
}
