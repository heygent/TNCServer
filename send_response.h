#ifndef TNC_SEND_RESPONSE_H
#define TNC_SEND_RESPONSE_H

#include <stdbool.h>
#include "httpdata.h"

bool send_response(int connection_socket, HTTPResponseData *);

#endif // TNC_SEND_RESPONSE_H
