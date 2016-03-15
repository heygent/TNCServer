#include <malloc.h>
#include <assert.h>
#include <tnc/core/job.h>
#include "httpdata.h"

void HTTPRequestData_init(HTTPRequestData *data)
{
    assert(NULL != data &&
           "HTTPRequestData: Attempting to initialize null pointer");

    data->file_to_serve = -1;
    data->remote_path = NULL;
    data->file_info.path = NULL;
    data->flags = 0;
    data->cleanup_jobs = TNCList_new();
}

HTTPRequestData *HTTPRequestData_new()
{

    HTTPRequestData *ret;
    ret = malloc(sizeof *ret);

    if(NULL != ret) HTTPRequestData_init(ret);

    return ret;
}

void HTTPRequestData_cleanup(HTTPRequestData *data)
{
    assert(data);
    while(!TNCList_empty(data->cleanup_jobs))
    {
        TNCJob *current_job = TNCList_pop_back(data->cleanup_jobs);
        TNCJob_run(current_job);
        free(current_job);
    }
    TNCList_destroy(data->cleanup_jobs);
}

void HTTPResponseData_init(HTTPResponseData *data, const HTTPRequestData *rd)
{
    assert(NULL != data &&
           "HTTPRequestData: Attempting to initialize null pointer");

    data->headers = TNCList_new();
    data->request_data = rd;

}

HTTPResponseData *HTTPResponseData_new(const HTTPRequestData *rd)
{

    HTTPResponseData *ret;
    ret = malloc(sizeof *ret);

    if(NULL != ret) HTTPResponseData_init(ret, rd);

    return ret;
}

void HTTPResponseData_cleanup(HTTPResponseData *data)
{

    assert(data);

    if(data->headers) TNCList_destroy_and_free(data->headers, free);

}

void HTTPResponseData_destroy(HTTPResponseData *data)
{

    HTTPResponseData_cleanup(data);
    free(data);
}
