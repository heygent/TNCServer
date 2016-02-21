#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include "make_response.h"

typedef char *response_header_producer(const HTTPRequestData *data);

response_header_producer make_status_line;
response_header_producer make_content_type_header;
response_header_producer make_content_length_header;
response_header_producer make_last_modified_header;
response_header_producer make_date_header;
response_header_producer make_connection_header;
response_header_producer make_server_header;


HTTPResponseData *make_response(const HTTPRequestData *request_data)
{
    TNCList response_headers;
    
    HTTPResponseData *response_data = HTTPResponseData_new(request_data);
    response_headers = TNCList_new();

    response_data->headers = response_headers;
    response_data->request_data = request_data;


    TNCList_push_back(
				response_headers,
				make_status_line(request_data)
		);

    TNCList_push_back(
				response_headers,
				make_content_type_header(request_data)
		);

    TNCList_push_back(
				response_headers,
				make_content_length_header(request_data)
		);

    TNCList_push_back(
				response_headers,
				make_last_modified_header(request_data)
		);

    TNCList_push_back(
				response_headers,
				make_date_header(request_data)
		);


    if(request_data->flags & HTTPRequestData_flags_keep_alive)
        TNCList_push_back(
				response_headers,
				make_connection_header(request_data)
		);


    TNCList_push_back(
				response_headers,
				make_server_header(request_data)
		);

    
    return response_data;
}

char *make_status_line(const HTTPRequestData *data)
{
    char *ret;
    const char *second_part;
    int status_code = data->status_code;

    switch(status_code)
    {
        case 200:
            second_part = " " RHDR_STATUSLINE_OK CRLF;
            break;

        case 304:
            second_part = " " RHDR_STATUSLINE_NOTMODIFIED CRLF;
            break;

        case 404:
            second_part = " " RHDR_STATUSLINE_NOTFOUND CRLF;
            break;

        case 501:
            second_part = " " RHDR_STATUSLINE_NOTIMPLEMENTED CRLF;
            break;

        default:
            second_part = " " RHDR_STATUSLINE_BADREQUEST CRLF;
            break;
    }

    ret = malloc(sizeof "HTTP/1.0" + strlen(second_part));
    strcpy(ret, "HTTP/1.0");
    strcat(ret, second_part);

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

    command_to_exec = malloc(
        strlen(path_to_serve) + strlen(COMMAND_MIMETYPE) + 1
    );

    sprintf(command_to_exec, COMMAND_MIMETYPE, path_to_serve);

    command_pipe = popen(command_to_exec, "r");
    fgets(mimetype, 256, command_pipe);
    pclose(command_pipe);

    free(command_to_exec);

    // get encoding

    command_to_exec = malloc(
        strlen(path_to_serve) + strlen(COMMAND_ENCODING) + 1
    );

    sprintf(command_to_exec, COMMAND_ENCODING, path_to_serve);

    command_pipe = popen(command_to_exec, "r");
    fgets(encoding, 256, command_pipe);
    pclose(command_pipe);

    free(command_to_exec);

    // print to header string

    ret = malloc(
        strlen(RHDR_CONTENT_TYPE) + strlen(mimetype) + strlen(encoding) + 1
    );

    snprintf(ret, 512, RHDR_CONTENT_TYPE CRLF, mimetype, encoding);

    return ret;
}

char *make_content_length_header(const HTTPRequestData *data)
{
    char filesize[32];
    char *ret;

    sprintf(filesize, "%jd", (intmax_t) data->file_to_serve_stat->st_size);
    ret = malloc(strlen(RHDR_CONTENT_LENGTH) + strlen(filesize) + 2);
    sprintf(ret, RHDR_CONTENT_LENGTH CRLF, filesize);

    return ret;
}

char *make_last_modified_header(const HTTPRequestData *data)
{
    struct tm *last_modified;
    size_t ret_len;
    char *ret;

    ret_len = strlen(RHDR_LAST_MODIFIED CRLF) + 64;
    ret = malloc(ret_len);
    last_modified = gmtime(&data->file_to_serve_stat->st_mtime);
    strftime(ret, ret_len, RHDR_LAST_MODIFIED CRLF, last_modified);

    return ret;
}

char *make_date_header(const HTTPRequestData *ignore)
{
    char *ret;
    size_t ret_len;

    time_t raw_time;
    struct tm *time_info;

    time(&raw_time);
    time_info = gmtime(&raw_time);

    ret_len = strlen(RHDR_DATE CRLF) + 64;
    ret = malloc(ret_len);
    strftime(ret, ret_len, RHDR_DATE CRLF, time_info);

    return ret;
}

char *make_connection_header(const HTTPRequestData *ignore)
{
    return strdup("Connection: keep-alive" CRLF);
}

char *make_server_header(const HTTPRequestData *ignore)
{
    return strdup("Server: TNC/0.1" CRLF);
}
