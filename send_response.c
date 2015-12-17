#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include "send_response.h"
#include "debugmacro.h"

bool send_response(int connection_socket, HTTPResponseData *response_data)
{
    // TODO: Verificare errori di invio

    bool ret;
    const HTTPRequestData *request_data = response_data->request_data;

    int fd = fileno(request_data->file_to_serve);
    off_t filesize = request_data->file_to_serve_stat->st_size;
    TNCList_Node it = TNCList_begin(response_data->headers);

    while(it != NULL)
    {
        const char *str = TNCList_get_value(it);

        send(connection_socket, str, strlen(str), 0 || MSG_NOSIGNAL || MSG_MORE);

        it = TNCList_next(it);
    }

    send(connection_socket, "\n\n", 2, 0);

    if(request_data->method == HTTP_METHOD_GET) {
        sendfile(connection_socket, fd, 0, (size_t) filesize);
        ret = ferror(request_data->file_to_serve) != 0;
    }
    else ret = true;

    return ret;
}
