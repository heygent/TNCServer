//
// Created by root on 12/12/15.
//

#include <malloc.h>
#include "httpdata.h"


HTTPRequestData *HTTPRequestData_new() {

    HTTPRequestData *ret;
    ret = malloc(sizeof(HTTPRequestData));

    ret->path_requested = NULL;
    ret->path_to_serve = NULL;

    return ret;
}

void HTTPRequestData_delete(HTTPRequestData *this) {

}
