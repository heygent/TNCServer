//
// Created by jeff on 11/01/16.
//

#ifndef PROGETTOSISTEMI_ERROR_H
#define PROGETTOSISTEMI_ERROR_H

#include "libtnc/error.h"

enum TNCServerError
{
    TNCServerError_start = TNCError_module_defined,
    TNCServerError_invalid_path,
    TNCServerError_connection_failed,
    TNCServerError_fn_getaddrinfo_failed,
    TNCServerError_fn_socket_failed,
    TNCServerError_fn_setsockopt_failed,
    TNCServerError_fn_bind_failed
};

#endif //PROGETTOSISTEMI_ERROR_H
