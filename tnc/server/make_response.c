#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "make_response.h"
#include "httpdate.h"

#define RHDR_STATUSLINE_BADREQUEST  "400 Bad Request"

#define RHDR_CONTENT_TYPE           "Content-Type: %s; charset=%s"
#define RHDR_CONTENT_LENGTH         "Content-Length: %s"
#define RHDR_LAST_MODIFIED          "Last-Modified: " HTTPDATE
#define RHDR_DATE                   "Date: " HTTPDATE

typedef char *response_header_producer(const HTTPRequestData *data);

response_header_producer make_status_line;
response_header_producer make_content_type_header;
response_header_producer make_content_length_header;
response_header_producer make_last_modified_header;
response_header_producer make_date_header;
response_header_producer make_connection_header;
response_header_producer make_server_header;


void make_response(HTTPResponseData *response_data)
{
    TNCList response_headers = response_data->headers;

    response_data->headers = response_headers;
    const HTTPRequestData *request_data = response_data->request_data;


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
        make_date_header(request_data)
    );

    if(! (request_data->flags & HTTPRequestData_flags_get_error_page))
    {
        TNCList_push_back(
            response_headers,
            make_last_modified_header(request_data)
        );
    }

    if(request_data->flags & HTTPRequestData_flags_keep_alive)
        TNCList_push_back(
            response_headers,
            make_connection_header(request_data)
        );


    TNCList_push_back(
        response_headers,
        make_server_header(request_data)
    );


    return;
}

char *make_status_line(const HTTPRequestData *data)
{
    char *ret;
    const char *second_part;
    int status_code = data->status_code;

    switch(status_code)
    {

#define TNC_XM(STATCODE, STATBRIEF, STATDESC) \
    case STATCODE: \
        second_part = " " #STATCODE " " STATBRIEF CRLF; \
        break;

#include "httpstatus.x.h"

#undef TNC_XM

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
    const char *mimetype = data->file_info.mimetype;
    const char *encoding = data->file_info.encoding;

    char *ret;

    // print to header string

    ret = malloc(
              strlen(RHDR_CONTENT_TYPE) + strlen(mimetype) + strlen(encoding) + 1
          );

    snprintf(ret, 512, RHDR_CONTENT_TYPE CRLF, mimetype, encoding);

    return ret;
}

char *make_content_length_header(const HTTPRequestData *data)
{
    assert(data);
    char filesize[32];
    char *ret;

    sprintf(filesize, "%zu", data->file_info.size);
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
    last_modified = gmtime(&data->file_info.last_edit);
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
