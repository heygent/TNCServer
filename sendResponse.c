#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include "sendResponse.h"
#include "debugmacro.h"

bool sendResponse(int socketConnessione, requestInfo *info)
{
    bool ret;
    int fd = fileno(info->toSend);
    off_t filesize = info->fileinfo->st_size;
    ssize_t rc;
    headerLine *headers = info->firstHeaderLine;

    while(headers != NULL)
    {
        send(socketConnessione, headers->string, strlen(headers->string),
             0 || MSG_NOSIGNAL || MSG_MORE);
        headers = headers->nextLine;
    }

    rc = send(socketConnessione, "\n\n", 2, 0);
    CONTROLLAERRORI(rc, "ERRORE su sendresponse");
    

    if(info->methodCode == M_GET) {
        rc = sendfile(socketConnessione, fd, 0, filesize);
        if (rc != filesize) rc = -1;
        CONTROLLAERRORI(rc, "ERRORE su sendresponse");
        ret = ferror(info->toSend) != 0;
    }
    else ret = true;

    return ret;
}
