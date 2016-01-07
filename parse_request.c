//
// Created by root on 12/12/15.
//

#include "tnc_adt/runtime_error.h"
#include "paths.h"
#include "httpdata.h"
#include "memzero.h"
#include <sys/stat.h>

#define HEADERSIZE 8096

void parse_main_header(HTTPRequestData *request_data, char *header);

HTTPRequestData parse_request(char *request) {

    HTTPRequestData request_data;

    char *request_line;
    char *saveptr_line;

    struct stat *fileinfo;

    // Tokenizza

    request_line = strtok_r(request, "\n", &saveptr_line);
    parse_main_header(&request_data, request_line);


    if(strncmp(request_data.status_code, "200", 3) != 0)
    {
        request_data.path_to_serve = malloc(strlen(PATH_DEFAULT_ERROR) + 1);
        sprintf(request_data.path_to_serve, PATH_DEFAULT_ERROR, request_data.status_code);
        request_data.file_to_serve = fopen(request_data.path_to_serve, "rb");
    }

    // TODO: Parsing del resto degli header

    fileinfo = malloc(sizeof(struct stat));
    fstat(fileno(request_data.file_to_serve), fileinfo);
    request_data.file_to_serve_stat = fileinfo;

    return request_data;

}


void parse_main_header(HTTPRequestData *request_data, char *header)
{

    char *current_token, *saveptr_space;
    char *path_to_serve;
    enum HTTPMethod method;
    HTTPRequestFlags *flags = &request_data->flags;

    MEMZERO(flags);

    current_token = strtok_r(header, " ", &saveptr_space);

    // Leggi il metodo HTTP

    if (!current_token) goto bad_request;
    else if (strcasecmp(current_token, GETSTR) == 0) method = HTTP_METHOD_GET;
    else if (strcasecmp(current_token, HEADSTR) == 0) method = HTTP_METHOD_HEAD;
    else if (strcasecmp(current_token, POSTSTR) == 0) method = HTTP_METHOD_POST;
    else goto bad_request;

    request_data->method = method;

    // Leggi il percorso richiesto

    current_token = strtok_r(NULL, " ", &saveptr_space);

    if (!current_token) goto bad_request;
    request_data->path_requested = strdup(current_token);

    // Leggi versione HTTP

    current_token = strtok_r(NULL, " ", &saveptr_space);

    if (!current_token || strncmp(HTTP10, current_token, strlen(HTTP10)) == 0)
    {
        strcpy(request_data->http_version, "1.0");
    }
    else if (strncmp(HTTP11, current_token, strlen(HTTP11)) == 0)
    {
        strcpy(request_data->http_version, "1.1");
        flags->keep_alive = true;
    }
    else goto bad_request;

    if (current_token) current_token = strtok_r(NULL, " ", &saveptr_space);
    if (current_token) goto bad_request;

    // Ottieni informazioni sul file richiesto

    switch(request_data->method)
    {
        case HTTP_METHOD_GET:
        case HTTP_METHOD_HEAD:
            break;

        case HTTP_METHOD_POST:
            strcpy(request_data->status_code, "405");
            return;

    }

    if (strcmp(request_data->path_requested, PATH_ROOT) == 0) {
        path_to_serve = get_index_path();
    }
    else {
        path_to_serve = malloc(strlen(local_path_to_serve) + strlen(request_data->path_requested) + 1);
        strcpy(path_to_serve, local_path_to_serve);
        strcat(path_to_serve, request_data->path_requested);
    }

    request_data->path_to_serve = path_to_serve;
    request_data->file_to_serve = fopen(path_to_serve, "rb");

    if (NULL == request_data->file_to_serve) strcpy(request_data->status_code, "404");
    else strcpy(request_data->status_code, "200");

    return;

bad_request:

    strcpy(request_data->status_code, "400");
    strcpy(request_data->http_version, "1.0");
    return;
}