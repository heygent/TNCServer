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


void HTTPRequestData_cleanup(HTTPRequestData *data)
{
    if(!data) return;
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

void HTTPResponseData_cleanup(HTTPResponseData *data)
{
    if(!data) return;

    if(data->headers) TNCList_destroy_and_free(data->headers, free);
}