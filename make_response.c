//
// Created by root on 13/12/15.
//

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include "make_response.h"

char *make_main_header(const HTTPRequestData *data);
char *make_content_type_header(const HTTPRequestData *data);
char *make_content_length_header(const HTTPRequestData *data);
char *make_last_modified_header(const HTTPRequestData *data);
char *make_date_header();

HTTPResponseData make_response(const HTTPRequestData *request_data)
{
    // Make headers
    TNCList response_headers;

    HTTPResponseData response_data;

    response_headers = TNCList_new();
    response_data.headers = response_headers;
    response_data.request_data = request_data;

    TNCList_add(response_headers, make_main_header(request_data));
    TNCList_add(response_headers, make_content_type_header(request_data));
    TNCList_add(response_headers, make_content_length_header(request_data));
    TNCList_add(response_headers, make_last_modified_header(request_data));
    TNCList_add(response_headers, make_date_header());

    return response_data;
}

char *make_main_header(const HTTPRequestData *data)
{
    char *ret;
    char *second_part;
    const char *http_version = data->http_version;
    const char *status_code = data->current_status_code;

    if(strcmp(status_code, "200") == 0)
    {
        second_part = strdup(" " RHDR_MAIN_OK "\n");
    }
    else if(strcmp(status_code, "404") == 0)
    {
        second_part = strdup(" " RHDR_MAIN_NOTFOUND "\n");
    }
    else
    {
        second_part = strdup(" " RHDR_MAIN_BADREQUEST "\n");
    }

    ret = malloc(sizeof("HTTP/1.0") + strlen(second_part));
    strcpy(ret, "HTTP/");
    strcat(ret, http_version);
    strcat(ret, second_part);

    free(second_part);
    return ret;

}

char *make_content_type_header(const HTTPRequestData *data)
{
    FILE *command_pipe;
    char *command_to_exec;

    char mimetype[256];
    char encoding[256];

    char *path_to_serve;
    char *ret;

    path_to_serve = data->path_to_serve;

    // get mimetype

    command_to_exec = malloc(strlen(path_to_serve) + strlen(COMMAND_MIMETYPE) + 1);

    sprintf(command_to_exec, COMMAND_MIMETYPE, path_to_serve);

    command_pipe = popen(command_to_exec, "r");
    fgets(mimetype, 256, command_pipe);
    pclose(command_pipe);

    free(command_to_exec);

    // get encoding

    command_to_exec = malloc(strlen(path_to_serve) + strlen(COMMAND_ENCODING) + 1);
    sprintf(command_to_exec, COMMAND_ENCODING, path_to_serve);

    command_pipe = popen(command_to_exec, "r");
    fgets(encoding, 256, command_pipe);
    pclose(command_pipe);

    free(command_to_exec);

    // print to header string

    ret = malloc(strlen(RHDR_CONTENT_TYPE) + strlen(mimetype) + strlen(encoding) + 1);

    snprintf(ret, 512, RHDR_CONTENT_TYPE "\n", mimetype, encoding);

    return ret;
}

char *make_content_length_header(const HTTPRequestData *data)
{
    char filesize[32];
    char *ret;

    sprintf(filesize, "%jd", (intmax_t) data->file_to_serve_stat->st_size);
    ret = malloc(strlen(RHDR_CONTENT_LENGTH) + strlen(filesize) + 2);
    sprintf(ret, RHDR_CONTENT_LENGTH "\n", filesize);

    return ret;
}

char *make_last_modified_header(const HTTPRequestData *data)
{
    struct tm *last_modified;
    size_t ret_len;
    char *ret;

    ret_len = strlen(RHDR_LAST_MODIFIED "\n") + 64;
    ret = malloc(ret_len);
    last_modified = gmtime(&data->file_to_serve_stat->st_mtime);
    strftime(ret, ret_len, RHDR_LAST_MODIFIED "\n", last_modified);

    return ret;
}

char *make_date_header()
{
    char *ret;
    size_t ret_len;

    time_t raw_time;
    struct tm *time_info;

    time(&raw_time);
    time_info = gmtime(&raw_time);

    ret_len = strlen(RHDR_DATE "\n") + 64;
    ret = malloc(ret_len);
    strftime(ret, ret_len, RHDR_DATE "\n", time_info);

    return ret;
}