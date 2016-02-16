//
// Created by root on 12/12/15.
//

#include <malloc.h>
#include <assert.h>
#include "httpdata.h"

void HTTPRequestData_init(HTTPRequestData *data)
{
    assert(NULL != data &&
        "HTTPRequestData: Attempting to initialize null pointer");

    data->file_to_serve = NULL;
    data->remote_path = NULL;
    data->path_to_serve = NULL;
    data->file_to_serve_stat = NULL;
    data->flags = 0;
}

HTTPRequestData *HTTPRequestData_new()
{

    HTTPRequestData *ret;
    ret = malloc(sizeof *ret);

    if(NULL != ret) HTTPRequestData_init(ret);

    return ret;
}

void HTTPRequestData_destroy_members(HTTPRequestData *data)
{

    assert(data);

    if(data->file_to_serve) fclose(data->file_to_serve);
    free(data->remote_path);
    free(data->path_to_serve);
    free(data->file_to_serve_stat);

}

void HTTPRequestData_destroy(HTTPRequestData *data)
{

    HTTPRequestData_destroy_members(data);
    free(data);
}

void HTTPResponseData_init(HTTPResponseData *data, const HTTPRequestData *rd)
{
    assert(NULL != data &&
        "HTTPRequestData: Attempting to initialize null pointer");

    data->headers = NULL;
    data->request_data = rd;

}

HTTPResponseData *HTTPResponseData_new(const HTTPRequestData *rd)
{

    HTTPResponseData *ret;
    ret = malloc(sizeof *ret);

    if(NULL != ret) HTTPResponseData_init(ret, rd);

    return ret;
}

void HTTPResponseData_destroy_members(HTTPResponseData *data)
{

    assert(data);

    if(data->headers) TNCList_destroy_and_free(data->headers, free);

}

void HTTPResponseData_destroy(HTTPResponseData *data)
{

    HTTPResponseData_destroy_members(data);
    free(data);
}
