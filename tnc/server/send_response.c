#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "send_response.h"
#include "error.h"

#ifdef linux
#include <sys/sendfile.h>
#endif

#define CHUNKSIZE 1024

int send_response(int connection_socket, HTTPResponseData *response_data)
{
    int ret = TNCError_good;
    ssize_t last_fn_ret;

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
#ifdef other
        off_t zero = 0;
        last_fn_ret = sendfile(connection_socket, fd, &zero, filesize);
        if(last_fn_ret == -1)
            ret = TNCServerError_sending_failed;
#else
        char buffer[CHUNKSIZE];

        size_t read_bytes = 0;

        while(read_bytes < filesize)
        {
            last_fn_ret = read(fd, buffer, CHUNKSIZE);
            if(last_fn_ret == -1)
            {
                ret = TNCServerError_sending_failed;
                break;
            }
            read_bytes += last_fn_ret;

            last_fn_ret =
                write(connection_socket, buffer, (size_t) last_fn_ret);

            if(last_fn_ret == -1)
            {
                ret = TNCServerError_sending_failed;
                break;
            }
            
        }
#endif
    }

    return ret;
}
