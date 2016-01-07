#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "send_response.h"

bool send_response(int connection_socket, HTTPResponseData *response_data)
{
    // TODO: Verificare errori di invio

    bool ret;
    const HTTPRequestData *request_data = response_data->request_data;

    int fd = fileno(request_data->file_to_serve);
    off_t filesize = request_data->file_to_serve_stat->st_size;
    TNCList headers = response_data->headers;

    while (TNCList_length(headers))
    {
        char *str = TNCList_pop_front(headers);
        send(connection_socket, str, strlen(str), 0 | MSG_NOSIGNAL | MSG_MORE);
        free(str);
    }

    TNCList_destroy(headers);

    send(connection_socket, "\n", 1, 0);

    if(request_data->method == HTTP_METHOD_GET) {

        #ifdef linux
        sendfile(connection_socket, fd, 0, (size_t) filesize);
        #endif

        ret = ferror(request_data->file_to_serve) != 0;
    }
    else ret = true;

    return ret;
}
