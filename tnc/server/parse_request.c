#include "paths.h"
#include "httpdata.h"
#include "httpdate.h"
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define HEADERSIZE 8096

static void parse_request_line(
    TNCServer self,
    HTTPRequestData *request_data,
    char *header
);

static void parse_header(
    TNCServer self,
    HTTPRequestData *request_data,
    HTTPRequestHeader *header
);

HTTPRequestData *parse_request(TNCServer self, char *request) {

    HTTPRequestData *request_data = HTTPRequestData_new();

    char *request_line;
    char *saveptr_line;

    struct stat *fileinfo;

    request_line = strtok_r(request, "\n", &saveptr_line);
    parse_request_line(self, request_data, request_line);

    if(request_data->flags & HTTPRequestData_flags_get_error_page)
    {
        request_data->path_to_serve = malloc(strlen(PATH_DEFAULT_ERROR) + 3);
        sprintf(
            request_data->path_to_serve,
            PATH_DEFAULT_ERROR,
            request_data->status_code
        );
        request_data->file_to_serve = fopen(request_data->path_to_serve, "rb");
    }

    fileinfo = malloc(sizeof *fileinfo);
    fstat(fileno(request_data->file_to_serve), fileinfo);
    request_data->file_to_serve_stat = fileinfo;

    while(! (request_data->flags & HTTPRequestData_flags_stop_parsing))
    {
        HTTPRequestHeader header;

        request_line = strtok_r(NULL, CRLF, &saveptr_line);
        if(!request_line) break;

        header.field_name = strsep(&request_line, ":");
        if(!header.field_name) continue;

        header.field_content = request_line;
        if(!header.field_content) continue;

        while(isspace(*header.field_content))
            ++header.field_content;

        parse_header(self, request_data, &header);
    }

    return request_data;

}


static void parse_request_line(
    TNCServer self,
    HTTPRequestData *request_data, 
    char *header
)
{

    char *current_token, *saveptr_space;
    char *path_to_serve;
    enum HTTPMethod method;
    uint8_t *flags = &request_data->flags;

    *flags = 0;
    current_token = strtok_r(header, " ", &saveptr_space);

    // Leggi il metodo HTTP

    if (!current_token) goto bad_request;
    else if (strcasecmp(current_token, "GET") == 0) method = HTTPMethod_GET;
    else if (strcasecmp(current_token, "HEAD") == 0) method = HTTPMethod_HEAD;
    else method = HTTPMethod_unrecognized;



    request_data->method = method;

    // Leggi il percorso richiesto

    current_token = strtok_r(NULL, " ", &saveptr_space);

    if (!current_token) goto bad_request;
    request_data->remote_path = strdup(current_token);

    // Leggi versione HTTP

    current_token = strtok_r(NULL, " ", &saveptr_space);


    if(!current_token)
    {
        request_data->http_version_major = 0;
        request_data->http_version_minor = 9;
    }
    else if (strncmp(HTTP10, current_token, strlen(HTTP10)) == 0)
    {
        request_data->http_version_major = 1;
        request_data->http_version_minor = 0;
    }
    else if (strncmp(HTTP11, current_token, strlen(HTTP11)) == 0)
    {
        request_data->http_version_major = 1;
        request_data->http_version_minor = 1;
    }
    else goto bad_request;

    if (current_token) current_token = strtok_r(NULL, " ", &saveptr_space);
    if (current_token) goto bad_request;

    // Ottieni informazioni sul file richiesto

    switch(request_data->method)
    {
        case HTTPMethod_HEAD:
            request_data->flags |= HTTPRequestData_flags_dont_send_payload;
            break;

        case HTTPMethod_GET:
            break;

        default:
            request_data->status_code = 501;

            request_data->flags |= HTTPRequestData_flags_stop_parsing;
            request_data->flags |= HTTPRequestData_flags_get_error_page;

            return;
    }

    if (strcmp(request_data->remote_path, PATH_ROOT) == 0)
    {
        path_to_serve = get_index_path(self);
    }
    else
    {
        path_to_serve = malloc(strlen(self->local_path) + 
            strlen(request_data->remote_path) + 1);
        
        strcpy(path_to_serve, self->local_path);
        strcat(path_to_serve, request_data->remote_path);
    }

    request_data->path_to_serve = path_to_serve;
    request_data->file_to_serve = fopen(path_to_serve, "rb");

    if (NULL == request_data->file_to_serve)
    {
        request_data->status_code = 404;
        request_data->flags |= HTTPRequestData_flags_get_error_page;
    }
    else request_data->status_code = 200;

    return;

bad_request:

    request_data->status_code = 400;
    request_data->flags |= HTTPRequestData_flags_stop_parsing;
    request_data->flags |= HTTPRequestData_flags_get_error_page;

    request_data->http_version_major = 1;
    request_data->http_version_minor = 0;

    return;
}

static void parse_header(
    TNCServer self, 
    HTTPRequestData *request_data, 
    HTTPRequestHeader *header
)
{
    if(strcmp(header->field_name, "Connection") == 0)
    {
        if(strcasecmp(header->field_content, "keep-alive") == 0)
        {
            request_data->flags |= HTTPRequestData_flags_keep_alive;
        }
        return;
    }

    if(strcmp(header->field_name, "If-Modified-Since") == 0)
    {
        if(request_data->status_code != 200) return;

        struct tm if_modified_since;
        char *date_parse_last_char;
        date_parse_last_char = strptime_httpdate(
            header->field_content, &if_modified_since);

        if(date_parse_last_char != NULL && *date_parse_last_char == '\0')
        {
            time_t modified_since = mktime(&if_modified_since);

            /* If the date provided is more or equal than date of last edit */
            if(modified_since >= request_data->file_to_serve_stat->st_mtime)
            {
                request_data->status_code = 304;
                request_data->flags |= HTTPRequestData_flags_dont_send_payload;

                return;
            }
        }

        return;
    }
}
