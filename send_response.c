#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include "send_response.h"

bool send_response(int connection_socket, HTTPResponseData *response_data)
{
    // TODO: Verificare errori di invio

    bool ret;
    const HTTPRequestData *request_data = response_data->request_data;

    int fd = fileno(request_data->file_to_serve);
    off_t filesize = request_data->file_to_serve_stat->st_size;
    TNCList headers = response_data->headers;

    for (TNCListNode current = TNCList_first(headers); current; current = TNCList_next(current))
    {
        const char *str = TNCList_getvalue(current);
        send(connection_socket, str, strlen(str), 0 | MSG_MORE);
    }

    send(connection_socket, CRLF, 2, 0);

    if(! (request_data->flags & HTTPRequestData_flags_dont_send_payload))
    {

        #ifdef linux
        sendfile(connection_socket, fd, 0, (size_t) filesize);
        #endif

        ret = ferror(request_data->file_to_serve) != 0;
    }
    else ret = true;

    return ret;
}
