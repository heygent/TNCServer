#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include "send_response.h"
#include "error.h"

int send_response(int connection_socket, HTTPResponseData *response_data)
{
    // TODO: Verificare errori di invio

    int ret;

    const HTTPRequestData *request_data = response_data->request_data;

    int fd = request_data->file_to_serve;
    size_t filesize = request_data->file_info.size;
    TNCList headers = response_data->headers;

    TNCListNode current = TNCList_first(headers);

    while(current)
    {
        const char *str = TNCList_getvalue(current);
        send(connection_socket, str, strlen(str), 0 | MSG_MORE);
        current = TNCList_next(current);
    }

    send(connection_socket, CRLF, 2, 0);

    if(! (request_data->flags & HTTPRequestData_flags_dont_send_payload))
    {
#ifdef linux
        off_t zero = 0;
        sendfile(connection_socket, fd, &zero, filesize);
#endif

    //    ret = ferror(request_data->file_to_serve) == 0 ?
    //          TNCError_good : TNCServerError_sending_failed;
    }
     ret = TNCError_good;

    return ret;
}
