#ifndef TNC_PARSE_REQUEST_H
#define TNC_PARSE_REQUEST_H

#include "httpdata.h"

HTTPRequestData *parse_request(TNCServer self, char *request);

#endif //TNC_PARSE_REQUEST_H
