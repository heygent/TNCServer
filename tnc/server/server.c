#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
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

struct _TNCServer
{
    char *local_path;
    uint16_t door;
    void *threadpool;
    size_t max_threads;
    int listen_socket;
    TNCList shutdown_cleanup_jobs;

    atomic_int shutdown;
};

int TNCServer_start(TNCServer self);

static int get_listen_socket(TNCServer self, int *listen_socket_ret);
static void connection_listener(TNCServer self, int connection_socket);
static int connection_handler(TNCServer self, int connection_socket);

static void *connection_listener_tr(void *connection_listener_param);
static void *connection_handler_tr(void *connection_handler_param);

struct connection_listener_param
{
    TNCServer self;
    int listening_socket;
};

struct connection_handler_param
{
    TNCServer self;
    int connection_socket;
};

TNCServer TNCServer_new(
    const char *localpath,
    uint16_t door,
    size_t max_threads
)
{
    TNCServer self = malloc(sizeof *self);

    if(self)
    {
        self->shutdown = ATOMIC_VAR_INIT(0);
        self->max_threads = max_threads > 2 ?
                            max_threads : default_max_threads;

        self->local_path = strdup(localpath);
        self->door = door;
        self->shutdown_cleanup_jobs = TNCList_new();


        if(!self->local_path)
        {
            free(self);
            self = NULL;
        }

    }
    return self;
}

void TNCServer_destroy(TNCServer self)
{
    if(self->shutdown == 0)
        TNCServer_shutdown(self, 0);

    TNCFixedThreadPool_destroy(self->threadpool);

    while(!TNCList_empty(self->shutdown_cleanup_jobs))
    {
        TNCJob *current = TNCList_pop_front(self->shutdown_cleanup_jobs);
        TNCJob_run(current);
        free(current);
    }

    TNCList_destroy(self->shutdown_cleanup_jobs);

    free(self->local_path);
    free(self);
}


int TNCServer_start(TNCServer self)
{

    int error_code;

    /*
    sigset_t sigpipe;

    sigemptyset(&sigpipe);
    sigaddset(&sigpipe, SIGPIPE);

    pthread_sigmask(SIG_BLOCK, &sigpipe, NULL);
    */

    if(!self->local_path || access(self->local_path, R_OK) != 0)
        return TNCServerError_invalid_path;

    error_code = get_listen_socket(self, &self->listen_socket);

    if(error_code != TNCError_good)
        return error_code;

    TNC_dbgprint(
        "Server in ascolto, vai su http://localhost:%" PRIu16
        " per inviare una richiesta\n",
        self->door
    );

    listen(self->listen_socket, 5);

    self->threadpool = TNCFixedThreadPool_new(self->max_threads);
    if(!self->threadpool) return TNCError_failed_alloc;

    TNCFixedThreadPool_start(self->threadpool);

    struct connection_listener_param *arg = malloc(sizeof *arg);

    arg->listening_socket = self->listen_socket;
    arg->self = self;

    TNCJob job =
    {
        .toexec = connection_listener_tr,
        .arg = arg,
        .result_callback = NULL
    };

    TNCFixedThreadPool_enqueue(self->threadpool, &job);

    return TNCError_good;

}

void TNCServer_shutdown(TNCServer self, int shutdown_flags)
{
    atomic_store(&self->shutdown, shutdown_flags);
    shutdown(self->listen_socket, SHUT_RDWR);
    close(self->listen_socket);
    TNCFixedThreadPool_shutdown(self->threadpool, shutdown_flags);

    while(!TNCList_empty(self->shutdown_cleanup_jobs))
    {
        TNCJob *cleanup_job = TNCList_pop_front(self->shutdown_cleanup_jobs);

        void *result = cleanup_job->toexec(cleanup_job->arg);
        if(cleanup_job->result_callback)
            cleanup_job->result_callback(result);

        free(cleanup_job);
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
static void connection_listener(TNCServer self, int listen_socket)
{
    struct connection_handler_param *connection_handler_param;

    TNC_dbgprint("\tConnection listener started\n");

    TNCJob job =
    {
        .toexec = connection_handler_tr,
        .result_callback = NULL
    };

    while(atomic_load(&self->shutdown) == 0)
    {

        int connection_socket = -1, attempts = 0;
        int connection_socket_flags;

        struct sockaddr_storage client_address;

        socklen_t client_address_len;

        client_address_len = sizeof client_address;

        while(
            connection_socket == -1 &&
            attempts++ < 3 &&
            atomic_load(&self->shutdown) == 0
        )
        {
            connection_socket = accept(self->listen_socket,
                                       (struct sockaddr *) &client_address,
                                       &client_address_len);

            connection_socket_flags = fcntl(connection_socket, F_GETFL, 0);

            fcntl(
                connection_socket,
                F_SETFL,
                connection_socket_flags | O_NONBLOCK
            );

        }
        if(connection_socket == -1)
            continue;


        connection_handler_param = malloc(sizeof *connection_handler_param);

        connection_handler_param->connection_socket = connection_socket;
        connection_handler_param->self = self;

        job.arg = connection_handler_param;

        TNCFixedThreadPool_enqueue(self->threadpool, &job);

    }
}
#pragma clang diagnostic pop

static int get_listen_socket(TNCServer self, int *listen_socket_ret)
{
    int listen_socket = -1;

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

static void *connection_listener_tr(void *_param)
{
    struct connection_listener_param *param =
        (struct connection_listener_param *) _param;

    connection_listener(param->self, param->listening_socket);
    free(param);
    return NULL;
}

static void *connection_handler_tr(void *_param)
{
    struct connection_handler_param *param =
        (struct connection_handler_param *) _param;

    connection_handler(param->self, param->connection_socket);
    free(param);
    return NULL;
}

static int connection_handler(TNCServer self, int connection_socket)
{
    char *request = malloc(BUFFERSIZE);
    ssize_t recv_ret;
    size_t request_size;
    int keep_alive;

    TNC_dbgprint("\tConnection handler started.\n");

    do
    {
        memset(request, 0, BUFFERSIZE);
        request_size = 0;
        int stop_receiving;
        int revents;

        struct pollfd connection_socket_pollfd =
            {
                .fd = connection_socket,
                .events = 0 | POLLIN
            };

        do
        {
            poll(&connection_socket_pollfd, 1, 60000);

            revents = connection_socket_pollfd.revents;

            if(revents & POLLIN)
            {
                recv_ret =
                    recv(
                        connection_socket,
                        request + request_size,
                        BUFFERSIZE - request_size - 1, 0
                    );

                if(recv_ret == -1)
                    goto connection_loop_end;


                request_size += recv_ret;
            }
            else if(revents & POLLERR || revents & POLLHUP || revents == 0)
                goto connection_loop_end;


            stop_receiving =
                strcmp((request + request_size - 2), "\n\n") == 0 ||
                strcmp((request + request_size - 4), CRLF CRLF) == 0;

            if(stop_receiving) break;

        }
        while(1);

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

    }
    while(keep_alive);

connection_loop_end:

    close(connection_socket);
    free(request);

    return TNCError_good;

}


const char *TNCServer_getlocalpath(TNCServer self)
{
    return self->local_path;
}

void TNCServer_cleanup_push(TNCServer self, TNCJob job)
{
    TNCJob *job_copy = malloc(sizeof(job));
    *job_copy = job;
    TNCList_push_front(self->shutdown_cleanup_jobs, job_copy);
}

void TNCServer_cleanup_pop(TNCServer self, int execute)
{
    TNCJob *popped_job = TNCList_pop_front(self->shutdown_cleanup_jobs);
    if(execute)
    {
        void *result = popped_job->toexec(popped_job->arg);
        if(popped_job->result_callback)
            popped_job->result_callback(result);
    }
    free(popped_job);
}

