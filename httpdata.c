//
// Created by root on 12/12/15.
//

#include <malloc.h>
#include <assert.h>
#include "httpdata.h"

void HTTPRequestData_init(HTTPRequestData *this)
{
    assert(NULL != this && "HTTPRequestData: Attempting to initialize null pointer");

    this->path_requested = NULL;
    this->path_to_serve = NULL;
    this->file_to_serve_stat = NULL;
}

HTTPRequestData *HTTPRequestData_new() {

    HTTPRequestData *ret;
    ret = malloc(sizeof *ret);

    if(NULL != ret) HTTPRequestData_init(ret);

    return ret;
}

void HTTPRequestData_delete(HTTPRequestData **this) {

    if(NULL != *this) {

        free((**this).path_requested);
        free((**this).path_to_serve);
        free((**this).file_to_serve_stat);

        *this = NULL;
    }
}
