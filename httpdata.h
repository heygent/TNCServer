//
// Created by root on 12/12/15.
//

#ifndef PROGETTOSISTEMI_HTTPREQUESTDATA_H
#define PROGETTOSISTEMI_HTTPREQUESTDATA_H

#include <stdbool.h>
#include <stdio.h>
#include "httpheaders.h"
#include "tnc_adt/list.h"

typedef struct t_httpreq_flags HTTPRequestFlags;
struct t_httpreq_flags
{

    int bad_request : 1;
    int not_found : 1;
    int method_not_allowed : 1;
    int keep_alive : 1;

};

typedef struct t_httpreqdata HTTPRequestData;
struct t_httpreqdata
{
    FILE            *file_to_serve;
    struct stat     *file_to_serve_stat;
    char            *path_requested;
    char            *path_to_serve;

    // TODO: Cambiare meccanismo di risposta agli errori usando flag invece di status code

    char            current_status_code[4];
    char            http_version[4];
    enum HTTPMethod method;
    HTTPRequestFlags flags;
};

typedef struct t_httpresdata HTTPResponseData;
struct t_httpresdata
{
    const HTTPRequestData    *request_data;
    char status_code[4];
    TNCList headers;
};



HTTPRequestData *HTTPRequestData_new();
void HTTPRequestData_delete(HTTPRequestData *this);

#endif //PROGETTOSISTEMI_HTTPREQUESTDATA_H
