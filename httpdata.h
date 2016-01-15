#ifndef TNC_HTTPREQUESTDATA_H
#define TNC_HTTPREQUESTDATA_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "httpheaders.h"
#include "libtnc/list.h"

typedef struct HTTPRequestData HTTPRequestData;

struct HTTPRequestData
{
    FILE                *file_to_serve;
    struct stat         *file_to_serve_stat;
    char                *remote_path;
    char                *path_to_serve;
    int                 status_code;
    int                 http_version_major;
    int                 http_version_minor;
    enum HTTPMethod     method;
    uint8_t             flags;
};

typedef struct HTTPRequestHeader HTTPRequestHeader;

enum HTTPRequestData_flags
{
    HTTPRequestData_flags_stop_parsing      = 1 << 0,
    HTTPRequestData_flags_keep_alive        = 1 << 1,
    HTTPRequestData_flags_dont_send_payload = 1 << 2,
    HTTPRequestData_flags_get_error_page    = 1 << 3
};

struct HTTPRequestHeader
{
    char *field_name;
    char *field_content;
};

typedef struct HTTPResponseData HTTPResponseData;

struct HTTPResponseData
{
    const HTTPRequestData    *request_data;
    TNCList headers;
};

void HTTPRequestData_init(HTTPRequestData *data);
HTTPRequestData *HTTPRequestData_new();
void HTTPRequestData_destroy(HTTPRequestData *data);
void HTTPRequestData_destroy_members(HTTPRequestData *data);

void HTTPResponseData_init(HTTPResponseData *data, const HTTPRequestData *rd);
HTTPResponseData *HTTPResponseData_new(const HTTPRequestData *rd);
void HTTPResponseData_destroy_members(HTTPResponseData *data);
void HTTPResponseData_destroy(HTTPResponseData *data);

#endif //TNC_HTTPREQUESTDATA_H
